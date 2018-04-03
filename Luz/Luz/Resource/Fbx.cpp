#include "stdafx.h"
#include "Fbx.h"
#include "gmath.h"
#include <fbxsdk.h>
#include <fstream>

#define FBXSDK_SHARED
#pragma comment(lib, "libfbxsdk.lib")

#if _WIN32 || _WIN64
#define FBX_DEFAULT_AXIS FbxAxisSystem::EPreDefinedAxisSystem::eDirectX
#elif
#define FBX_DEFAULT_AXIS FbxAxisSystem::EPreDefinedAxisSystem::OpenGL
#endif


using namespace gmath;

namespace Resource
{
    enum TriWindingOrder
    {
        TRI_WINDING_ORDER_CW,
        TRI_WINDING_ORDER_CCW
    };

#if _WIN32 || _WIN64
#define FBX_WINDING_ORDER TriWindingOrder::TRI_WINDING_ORDER_CCW
#elif
#define FBX_WINDING_ORDER TriWindingOrder::TRI_WINDING_ORDER_CW
#endif

    static bool g_convertCoordinate = false;
    static bool g_reverseWindingOrder = false;

    static void GetNodeAttributes(Fbx* pResource, FbxNode* pNode);
    static void GetNodeAttributesRecursively(Fbx* pResource, FbxNode* pNode);
    static void GetMeshAttributes(Fbx* pResource, FbxNode* pNode);

    static bool TryWindingOrder(Fbx* pResource, u32 i0, u32 i1, u32 i2);
    static void TriangulatePolygonAdditive(FbxMesh* pMesh, Fbx* pResource, const FbxStringList& uvSetNames, int polygon, int numPolygonVertices);
    static void TriangulatePolygon(FbxMesh* pMesh, Fbx* pResource, const FbxStringList& uvSetNames, int polygon, int numPolygonVertices);

    static bool TryGetVertexPosition(FbxMesh* pMesh, int polygon, int polygonVertex, float(&outPosition)[3]);
    static bool TryGetVertexNormal(FbxMesh* pMesh, int polygon, int polygonVertex, float(&outNormal)[3]);
    static bool TryGetVertexUV(FbxMesh* pMesh, int polygon, int polygonVertex, const FbxStringList& uvSetNames, float(&outUV)[2]);
    static void GetVertexAttributes(FbxMesh* pMesh, int polygon, int polygonVertex, const FbxStringList& uvSetNames, FbxVector4& outPosition, FbxVector4& outNormal, FbxVector2& outTexCoord);
    static void CreateVertex(FbxMesh* pMesh, int polygon, int polygonVertex, const FbxStringList& uvSetNames, Fbx::Vertex& outVertex);

    static void ConvertCoordinateSystem(FbxVector4& vector);
    static void ConvertCoordinateSystem(FbxVector2& vector);
    static void ConvertCoordinateSystem(FbxAMatrix& matrix);

    template<typename ... Args> static std::string string_format(const std::string& format, Args ... args);
    static std::string ToString(FbxVector4& vector);
    static std::string ToString(FbxVector2& vector);
    static std::string ToString(float(&vector)[4]);
    static std::string ToString(float(&vector)[3]);
    static std::string ToString(float(&vector)[2]);

    template<i32 T0, i32 T1> static void ExtractFloat(double(&in)[T0], float(&out)[T1]);
    template<i32 T> static void ExtractFloat(double(&in)[T], float(&out)[T]);

    void GetNodeAttributes(Fbx* pResource, FbxNode* pNode)
    {
        LUZASSERT(pResource && pNode);

        for (int i = 0, n = pNode->GetNodeAttributeCount(); i < n; ++i)
        {
            if (FbxNodeAttribute* pNodeAttribute = pNode->GetNodeAttributeByIndex(i))
            {
                FbxNodeAttribute::EType type = pNodeAttribute->GetAttributeType();
                switch (type) {
                case FbxNodeAttribute::eUnknown:
                case FbxNodeAttribute::eNull:
                case FbxNodeAttribute::eMarker:
                case FbxNodeAttribute::eSkeleton:
                    break;
                case FbxNodeAttribute::eMesh:
                    GetMeshAttributes(pResource, pNode);
                    break;
                case FbxNodeAttribute::eNurbs:
                case FbxNodeAttribute::ePatch:
                case FbxNodeAttribute::eCamera:
                case FbxNodeAttribute::eCameraStereo:
                case FbxNodeAttribute::eCameraSwitcher:
                case FbxNodeAttribute::eLight:
                case FbxNodeAttribute::eOpticalReference:
                case FbxNodeAttribute::eOpticalMarker:
                case FbxNodeAttribute::eNurbsCurve:
                case FbxNodeAttribute::eTrimNurbsSurface:
                case FbxNodeAttribute::eBoundary:
                case FbxNodeAttribute::eNurbsSurface:
                case FbxNodeAttribute::eShape:
                case FbxNodeAttribute::eLODGroup:
                case FbxNodeAttribute::eSubDiv:
                default:
                    break;
                }
            }
        }
    }

    void GetNodeAttributesRecursively(Fbx* pResource, FbxNode* pNode)
    {
        LUZASSERT(pResource);
        if (pNode)
        {
            GetNodeAttributes(pResource, pNode);

            for (int i = 0, n = pNode->GetChildCount(); i < n; ++i)
            {
                GetNodeAttributesRecursively(pResource, pNode->GetChild(i));
            }
        }
    }

    void GetMeshAttributes(Fbx* pResource, FbxNode* pNode)
    {
        auto pMesh = pNode->GetMesh();
        if (!pMesh) return;

        bool isTri = pMesh->IsTriangleMesh();

        FbxStringList uvSetNames;
        pMesh->GetUVSetNames(uvSetNames);
        bool hasUV = uvSetNames.GetCount() != 0;

        for (int polygon = 0, numPoly = pMesh->GetPolygonCount(); polygon < numPoly; ++polygon)
        {
            int numPolygonVertices = pMesh->GetPolygonSize(polygon);

            if (numPolygonVertices > 3)
            {
                // TODO: Figure out when it would be better to add a vertex to avoid degenerate triangles...
                //TriangulatePolygonAdditive(pMesh, pResource, uvSetNames, polygon, numPolygonVertices);
                TriangulatePolygon(pMesh, pResource, uvSetNames, polygon, numPolygonVertices);
            }
            else
            {
                int polygonVertexBegin, polygonVertexEnd, polygonVertexIncr;
                if (g_reverseWindingOrder)
                {
                    polygonVertexBegin = numPolygonVertices - 1;
                    polygonVertexEnd = -1;
                    polygonVertexIncr = -1;
                }
                else
                {
                    polygonVertexBegin = 0;
                    polygonVertexEnd = numPolygonVertices;
                    polygonVertexIncr = 1;
                }

                int polygonVertex = polygonVertexBegin;
                while (polygonVertex != polygonVertexEnd)
                {
                    FbxVector4 position = FbxVector4(0.0, 0.0, 0.0);
                    FbxVector4 normal = FbxVector4(0.0, 0.0, 0.0);
                    FbxVector2 uv = FbxVector2(0.0, 0.0);

                    GetVertexAttributes(pMesh, polygon, polygonVertex, uvSetNames, position, normal, uv);
                    u32 index = pResource->AddVertex(position.mData, normal.mData, uv.mData);
                    pResource->AddIndex(index);

                    polygonVertex += polygonVertexIncr;
                }
            }
        }
    }

    bool TryWindingOrder(Fbx* pResource, u32 i0, u32 i1, u32 i2)
    {
        auto v0 = pResource->GetVertex(i0);
        auto v1 = pResource->GetVertex(i1);
        auto v2 = pResource->GetVertex(i2);

        float e0[3], e1[3], n[3];
        TVec<float, 3>::subtract(v1->Position, v0->Position, e0);
        TVec<float, 3>::subtract(v1->Position, v2->Position, e1);
        TVec<float, 3>::cross(e0, e1, n);

        float i[3] = { 1.0f, 0.0f, 0.0f };
        float j[3] = { 0.0f, 1.0f, 0.0f };
        float k[3] = { 0.0f, 0.0f, 1.0f };

        float di = TVec<float, 3>::dot(n, i);
        float dj = TVec<float, 3>::dot(n, j);
        float dk = TVec<float, 3>::dot(n, k);

        return (di < 0.0f || dj < 0.0f || dk < 0.0f);
    }

    void TriangulatePolygonAdditive(FbxMesh* pMesh, Fbx* pResource, const FbxStringList& uvSetNames, int polygon, int numPolygonVertices)
    {
        std::vector<u32> polygonVertexIndices;
        polygonVertexIndices.reserve(static_cast<size_t>(numPolygonVertices));

        // Triangulate
        float position[3] = { 0.0f, 0.0f, 0.0f };
        float normal[3] = { 0.0f, 0.0f, 0.0f };
        float uv[2] = { 0.0f, 0.0f };

        for (int polygonVertex = 0; polygonVertex < numPolygonVertices; ++polygonVertex)
        {
            u32 polygonIndex = pResource->AddVertex();
            auto vertex = pResource->GetVertex(polygonIndex);
            CreateVertex(pMesh, polygon, polygonVertex, uvSetNames, *vertex);
            polygonVertexIndices.push_back(polygonIndex);

            TVec<float, 3>::add(position, vertex->Position);
            TVec<float, 3>::add(normal, vertex->Normal);
            TVec<float, 2>::add(uv, vertex->UV);
        }

        float inv = 1.0f / numPolygonVertices;
        TVec<float, 3>::mul(position, inv);
        TVec<float, 3>::mul(normal, inv);
        TVec<float, 2>::mul(uv, inv);

        auto index = pResource->AddVertex(position, normal, uv);

        for (int polygonVertexIndex = 0, numPolygonVertexIndices = (int)polygonVertexIndices.size(); polygonVertexIndex < numPolygonVertexIndices; ++polygonVertexIndex)
        {
            u32 currentIndex = polygonVertexIndices[polygonVertexIndex];
            u32 nextIndex = (polygonVertexIndex == numPolygonVertexIndices - 1) ? polygonVertexIndices[0] : polygonVertexIndices[polygonVertexIndex + 1];

            if (g_reverseWindingOrder)
            {
                pResource->AddIndex(nextIndex);
                pResource->AddIndex(currentIndex);
                pResource->AddIndex(index);
            }
            else
            {
                pResource->AddIndex(index);
                pResource->AddIndex(currentIndex);
                pResource->AddIndex(nextIndex);

                /*          std::cout << index << ": " << ToString(pResource->GetVertex(index)->Position) << std::endl;
                std::cout << currentIndex << ": " << ToString(pResource->GetVertex(currentIndex)->Position) << std::endl;
                std::cout << nextIndex << ": " << ToString(pResource->GetVertex(nextIndex)->Position) << std::endl;*/
            }
        }
    }

    void TriangulatePolygon(FbxMesh* pMesh, Fbx* pResource, const FbxStringList& uvSetNames, int polygon, int numPolygonVertices)
    {
        std::vector<u32> polygonVertexIndices;
        polygonVertexIndices.reserve(static_cast<size_t>(numPolygonVertices));

        for (int polygonVertex = 0; polygonVertex < numPolygonVertices; ++polygonVertex)
        {
            u32 polygonIndex = pResource->AddVertex();
            auto vertex = pResource->GetVertex(polygonIndex);
            CreateVertex(pMesh, polygon, polygonVertex, uvSetNames, *vertex);
            polygonVertexIndices.push_back(polygonIndex);
        }

        // TODO: Pick best starting vertex (for now picking vertex @ first index)
        u32 index = polygonVertexIndices[0];

        for (int polygonVertexIndex = 1, numPolygonVertexIndices = static_cast<int>(polygonVertexIndices.size()) - 1; polygonVertexIndex < numPolygonVertexIndices; ++polygonVertexIndex)
        {
            u32 currentIndex = polygonVertexIndices[polygonVertexIndex];
            u32 nextIndex = polygonVertexIndices[polygonVertexIndex + 1];

            if (g_reverseWindingOrder)
            {
                pResource->AddIndex(nextIndex);
                pResource->AddIndex(currentIndex);
                pResource->AddIndex(index);
            }
            else
            {

                pResource->AddIndex(index);
                pResource->AddIndex(currentIndex);
                pResource->AddIndex(nextIndex);

                //std::cout << nextIndex << ": " << ToString(pResource->GetVertex(nextIndex)->Position) << std::endl;
                //std::cout << currentIndex << ": " << ToString(pResource->GetVertex(currentIndex)->Position) << std::endl;
                //std::cout << index << ": " << ToString(pResource->GetVertex(index)->Position) << std::endl;
            }
        }
    }

    bool TryGetVertexPosition(FbxMesh* pMesh, int polygon, int polygonVertex, float(&outPosition)[3])
    {
        int controlPoint = pMesh->GetPolygonVertex(polygon, polygonVertex);
        FbxVector4 position = pMesh->GetControlPointAt(controlPoint);
        ExtractFloat<4, 3>(position.mData, outPosition);
        return controlPoint != -1;
    }

    bool TryGetVertexNormal(FbxMesh* pMesh, int polygon, int polygonVertex, float(&outNormal)[3])
    {
        FbxVector4 normal;
        bool success = pMesh->GetPolygonVertexNormal(polygon, polygonVertex, normal);
        if (success)
        {
            ExtractFloat<4, 3>(normal.mData, outNormal);
        }

        return success;
    }

    bool TryGetVertexUV(FbxMesh* pMesh, int polygon, int polygonVertex, const FbxStringList& uvSetNames, float(&outUV)[2])
    {
        FbxVector2 uv;
        bool unmapped;
        bool success = (pMesh->GetPolygonVertexUV(polygon, polygonVertex, uvSetNames.GetStringAt(0), uv, unmapped) || unmapped);
        if (success)
        {
            ExtractFloat<2>(uv.mData, outUV);
        }

        return success;
    }

    void GetVertexAttributes(FbxMesh* pMesh, int polygon, int polygonVertex, const FbxStringList& uvSetNames, FbxVector4& outPosition, FbxVector4& outNormal, FbxVector2& outTexCoord)
    {
        bool unmapped;
        int controlPoint = pMesh->GetPolygonVertex(polygon, polygonVertex);
        outPosition = pMesh->GetControlPointAt(controlPoint);
        pMesh->GetPolygonVertexNormal(polygon, polygonVertex, outNormal);
        pMesh->GetPolygonVertexUV(polygon, polygonVertex, uvSetNames.GetStringAt(0), outTexCoord, unmapped);

        if (g_convertCoordinate)
        {
            ConvertCoordinateSystem(outPosition);
            ConvertCoordinateSystem(outNormal);
            ConvertCoordinateSystem(outTexCoord);
        }
    }

    void CreateVertex(FbxMesh* pMesh, int polygon, int polygonVertex, const FbxStringList& uvSetNames, Fbx::Vertex& outVertex)
    {
        FbxVector4 position = FbxVector4(0.0, 0.0, 0.0);
        FbxVector4 normal = FbxVector4(0.0, 0.0, 0.0);
        FbxVector2 uv = FbxVector2(0.0, 0.0);

        GetVertexAttributes(pMesh, polygon, polygonVertex, uvSetNames, position, normal, uv);

        ExtractFloat<4, 3>(position.mData, outVertex.Position);
        ExtractFloat<4, 3>(normal.mData, outVertex.Normal);
        ExtractFloat<2>(uv.mData, outVertex.UV);
    }

    void ConvertCoordinateSystem(FbxVector4& vector)
    {
        // Negate x-axis
        vector.mData[0] = -vector.mData[0];
    }

    void ConvertCoordinateSystem(FbxVector2& vector)
    {
        vector.mData[0] = 1.0 - vector.mData[0];
    }

    void ConvertCoordinateSystem(FbxAMatrix& matrix)
    {
        auto pDouble = static_cast<double*>(matrix);

        // Negate first row
        pDouble[ 4] = -pDouble[ 4];
        pDouble[ 8] = -pDouble[ 8];
        pDouble[12] = -pDouble[12];

        // Negate first column
        pDouble[1] = -pDouble[1];
        pDouble[2] = -pDouble[2];
        pDouble[3] = -pDouble[3];
    }

    template<typename ... Args>
    std::string string_format(const std::string& format, Args ... args)
    {
        size_t size = 64;
        std::unique_ptr<char[]> buf(new char[size]);
        int i = _snprintf_s(buf.get(), size, size, format.c_str(), args ...);
        return std::string(buf.get(), buf.get() + size); // We don't want the '\0' inside
    }

    std::string ToString(FbxVector4& vector)
    {
        return string_format("(%f, %f, %f, %f)", vector.mData[0], vector.mData[1], vector.mData[2], vector.mData[3]);
    }
    
    std::string ToString(FbxVector2& vector)
    {
        return string_format("(%f, %f)", vector.mData[0], vector.mData[1]);
    }
    
    std::string ToString(float(&vector)[4])
    {
        return string_format("(%f, %f, %f, %f)", vector[0], vector[1], vector[2], vector[3]);
    }

    std::string ToString(float(&vector)[3])
    {
        return string_format("(%f, %f, %f)", vector[0], vector[1], vector[2]);
    }

    std::string ToString(float(&vector)[2])
    {
        return string_format("(%f, %f)", vector[0], vector[1]);
    }

    template<i32 T0, i32 T1>
    void ExtractFloat(double(&in)[T0], float(&out)[T1])
    {
        for (int i = 0; i < T1; ++i)
        {
            out[i] = static_cast<float>(in[i]);
        }
    }

    template<i32 T>
    void ExtractFloat(double(&in)[T], float(&out)[T])
    {
        ExtractFloat<T, T>(in, out);
    }

    Fbx::Fbx()
    {

    }

    Fbx::~Fbx()
    {

    }

    Fbx::Vertex* Fbx::GetVertex(u32 i)
    {
        Fbx::Vertex* vertex = nullptr;
        if (m_vertices.size() && i < (u32)m_vertices.size())
        {
            vertex = &m_vertices[i];
        }

        return vertex;
    }

    u32 Fbx::AddVertex()
    {
        u32 i = static_cast<u32>(m_vertices.size());
        m_vertices.emplace_back();
        return i;
    }

    u32 Fbx::AddVertex(float(&position)[3], float(&normal)[3], float(&uv)[2])
    {
        u32 i = static_cast<u32>(m_vertices.size());
        m_vertices.emplace_back(position, normal, uv);
        return i;
    }

    u32 Fbx::AddVertex(double(&position)[4], double(&normal)[4], double(&uv)[2])
    {
        float pos[3];
        float norm[3];
        float tex[2];

        ExtractFloat<4, 3>(position, pos);
        ExtractFloat<4, 3>(normal, norm);
        ExtractFloat<2>(uv, tex);

        return AddVertex(pos, norm, tex);
    }

    void Fbx::AddIndex(u32 i)
    {
        m_indices.push_back(i);
    }

    std::shared_ptr<const Fbx> Fbx::Load(const std::string& filename)
    {
        auto fbx = std::make_shared<Fbx>();

        FbxManager* fbxManager = FbxManager::Create();

        FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
        fbxManager->SetIOSettings(ios);

        FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
        if (!fbxImporter->Initialize(filename.c_str(), -1, fbxManager->GetIOSettings()))
        {
            return nullptr;
        }

        FbxScene* fbxScene = FbxScene::Create(fbxManager, "");
        fbxImporter->Import(fbxScene);
        fbxImporter->Destroy();

        g_convertCoordinate = fbxScene->GetGlobalSettings().GetAxisSystem() != FBX_DEFAULT_AXIS;
        g_reverseWindingOrder = (FBX_WINDING_ORDER != TriWindingOrder::TRI_WINDING_ORDER_CW);

        FbxNode* rootNode = fbxScene->GetRootNode();
        if (rootNode)
        {
            GetNodeAttributesRecursively(fbx.get(), rootNode);
        }

        fbxScene->Destroy();
        fbxManager->Destroy();

        return fbx;
    }

}


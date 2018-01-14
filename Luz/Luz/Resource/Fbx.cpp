#include "stdafx.h"
#include "Fbx.h"
#include "gmath.h"
#include <fbxsdk.h>
#include <fstream>

#define FBXSDK_SHARED
#pragma comment(lib, "libfbxsdk.lib")

using namespace gmath;

namespace Resource
{
    static void GetNodeAttributes(Fbx* pResource, FbxNode* pNode);
    static void GetNodeAttributesRecursively(Fbx* pResource, FbxNode* pNode);
    static void GetMeshAttributes(Fbx* pResource, FbxNode* pNode, bool ccw = false);

    static bool TryGetVertexPosition(FbxMesh* pMesh, int polygon, int polygonVertex, float(&outPosition)[3]);
    static bool TryGetVertexNormal(FbxMesh* pMesh, int polygon, int polygonVertex, float(&outNormal)[3]);
    static bool TryGetVertexUV(FbxMesh* pMesh, int polygon, int polygonVertex, const FbxStringList& uvSetNames, float(&outUV)[2]);
    static void GetVertexAttributes(FbxMesh* pMesh, int polygon, int polygonVertex, const FbxStringList& uvSetNames, FbxVector4& outPosition, FbxVector4& outNormal, FbxVector2& outTexCoord);

    template<i32 T0, i32 T1> static void ExtractFloat(double(&in)[T0], float(&out)[T1]);
    template<i32 T> static void ExtractFloat(double(&in)[T], float(&out)[T]);

    /* Tab character ("\t") counter */
    int numTabs = 0;

    /**
    * Print the required number of tabs.
    */
    void PrintTabs(std::ofstream& fstream) {
        for (int i = 0; i < numTabs; i++)
            fstream << "  ";
    }

    /**
    * Return a string-based representation based on the attribute type.
    */
    FbxString GetAttributeTypeName(FbxNodeAttribute::EType type) {
        switch (type) {
        case FbxNodeAttribute::eUnknown: return "unidentified";
        case FbxNodeAttribute::eNull: return "null";
        case FbxNodeAttribute::eMarker: return "marker";
        case FbxNodeAttribute::eSkeleton: return "skeleton";
        case FbxNodeAttribute::eMesh: return "mesh";
        case FbxNodeAttribute::eNurbs: return "nurbs";
        case FbxNodeAttribute::ePatch: return "patch";
        case FbxNodeAttribute::eCamera: return "camera";
        case FbxNodeAttribute::eCameraStereo: return "stereo";
        case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
        case FbxNodeAttribute::eLight: return "light";
        case FbxNodeAttribute::eOpticalReference: return "optical reference";
        case FbxNodeAttribute::eOpticalMarker: return "marker";
        case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
        case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
        case FbxNodeAttribute::eBoundary: return "boundary";
        case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
        case FbxNodeAttribute::eShape: return "shape";
        case FbxNodeAttribute::eLODGroup: return "lodgroup";
        case FbxNodeAttribute::eSubDiv: return "subdiv";
        default: return "unknown";
        }
    }

    /**
    * Print an attribute.
    */
    void PrintAttribute(std::ofstream& fstream, FbxNodeAttribute* pAttribute) {
        if (!pAttribute) return;

        FbxString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
        FbxString attrName = pAttribute->GetName();
        PrintTabs(fstream);
        // Note: to retrieve the character array of a FbxString, use its Buffer() method.
        fstream << "<attribute type=" << typeName.Buffer() << " name=" << attrName.Buffer() << "/>" << std::endl;
        //printf("<attribute type='%s' name='%s'/>\n", typeName.Buffer(), attrName.Buffer());
    }

    void PrintNode(std::ofstream& fstream, FbxNode* pNode)
    {
        PrintTabs(fstream);
        const char* nodeName = pNode->GetName();
        FbxDouble3 translation = pNode->LclTranslation.Get();
        FbxDouble3 rotation = pNode->LclRotation.Get();
        FbxDouble3 scaling = pNode->LclScaling.Get();

        fstream << "<node name=" << nodeName
            << "translation=" << "(" << translation[0] << ", " << translation[1] << ", " << translation[2] << ")"
            << "rotation=" << "(" << rotation[0] << ", " << rotation[1] << ", " << rotation[2] << ")"
            << "scaling=" << "(" << scaling[0] << ", " << scaling[1] << ", " << scaling[2] << ")>" << std::endl;
        /*printf("<node name='%s' translation='(%f, %f, %f)' rotation='(%f, %f, %f)' scaling='(%f, %f, %f)'>\n",
            nodeName,
            translation[0], translation[1], translation[2],
            rotation[0], rotation[1], rotation[2],
            scaling[0], scaling[1], scaling[2]
        );*/
        numTabs++;

        // Print the node's attributes.
        for (int i = 0; i < pNode->GetNodeAttributeCount(); i++)
            PrintAttribute(fstream, pNode->GetNodeAttributeByIndex(i));

        // Recursively print the children.
        for (int j = 0; j < pNode->GetChildCount(); j++)
            PrintNode(fstream, pNode->GetChild(j));

        numTabs--;
        PrintTabs(fstream);
        fstream << "</node>\n";
        //printf("</node>\n");
    }

    template<i32 T0, i32 T1>
    static void ExtractFloat(double(&in)[T0], float(&out)[T1])
    {
        for (int i = 0; i < T1; ++i)
        {
            out[i] = static_cast<float>(in[i]);
        }
    }

    template<i32 T>
    static void ExtractFloat(double(&in)[T], float(&out)[T])
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
        m_indices.push_back(i);
        m_vertices.emplace_back();
        return i;
    }

    u32 Fbx::AddVertex(float(&position)[3], float(&normal)[3], float(&uv)[2])
    {
        u32 i = static_cast<u32>(m_vertices.size());
        m_indices.push_back(i);
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

    std::shared_ptr<const Fbx> Fbx::Load(const std::wstring& filename)
    {
        FbxManager* fbxManager = FbxManager::Create();

        FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
        fbxManager->SetIOSettings(ios);

        FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
        if (!fbxImporter->Initialize(std::string(filename.begin(), filename.end()).c_str(), -1, fbxManager->GetIOSettings()))
        {
            return nullptr;
        }

        FbxScene* fbxScene = FbxScene::Create(fbxManager, "");
        fbxImporter->Import(fbxScene);
        fbxImporter->Destroy();

        std::ofstream fileStream;
        fileStream.open("fbx.txt");
        if (!fileStream.is_open())
        {
            return nullptr;
        }

        FbxNode* rootNode = fbxScene->GetRootNode();
        if (rootNode)
        {
            for (int i = 0, numNodes = rootNode->GetChildCount(); i < numNodes; ++i)
            {
                PrintNode(fileStream, rootNode->GetChild(i));
            }
        }

        fileStream.close();
        

        auto fbx = std::make_shared<Fbx>();
        GetNodeAttributesRecursively(fbx.get(), rootNode);
        fbxScene->Destroy();
        fbxManager->Destroy();

        return fbx;
    }

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

    static bool TryGetVertexPosition(FbxMesh* pMesh, int polygon, int polygonVertex, float(&outPosition)[3])
    {
        int controlPoint = pMesh->GetPolygonVertex(polygon, polygonVertex);
        FbxVector4 position = pMesh->GetControlPointAt(controlPoint);
        ExtractFloat<4, 3>(position.mData, outPosition);
        return controlPoint != -1;
    }

    static bool TryGetVertexNormal(FbxMesh* pMesh, int polygon, int polygonVertex, float(&outNormal)[3])
    {
        FbxVector4 normal;
        bool success = pMesh->GetPolygonVertexNormal(polygon, polygonVertex, normal);
        if (success)
        {
            ExtractFloat<4, 3>(normal.mData, outNormal);
        }

        return success;
    }

    static bool TryGetVertexUV(FbxMesh* pMesh, int polygon, int polygonVertex, const FbxStringList& uvSetNames, float(&outUV)[2])
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

    static void GetVertexAttributes(FbxMesh* pMesh, int polygon, int polygonVertex, const FbxStringList& uvSetNames, FbxVector4& outPosition, FbxVector4& outNormal, FbxVector2& outTexCoord)
    {
        bool unmapped;
        int controlPoint = pMesh->GetPolygonVertex(polygon, polygonVertex);
        outPosition = pMesh->GetControlPointAt(controlPoint);
        pMesh->GetPolygonVertexNormal(polygon, polygonVertex, outNormal);
        pMesh->GetPolygonVertexUV(polygon, polygonVertex, uvSetNames.GetStringAt(0), outTexCoord, unmapped);
    }

    static void CreateVertex(FbxMesh* pMesh, int polygon, int polygonVertex, const FbxStringList& uvSetNames, Fbx::Vertex& outVertex)
    {
        FbxVector4 position = FbxVector4(0.0, 0.0, 0.0);
        FbxVector4 normal = FbxVector4(0.0, 0.0, 0.0);
        FbxVector2 uv = FbxVector2(0.0, 0.0);

        GetVertexAttributes(pMesh, polygon, polygonVertex, uvSetNames, position, normal, uv);

        ExtractFloat<4, 3>(position.mData, outVertex.Position);
        ExtractFloat<4, 3>(normal.mData, outVertex.Normal);
        ExtractFloat<2>(uv.mData, outVertex.UV);
    }

    void GetMeshAttributes(Fbx* pResource, FbxNode* pNode, bool ccw /*= false*/)
    {
#if _DEBUG
        std::cout << pNode->GetName() << std::endl;
#endif

        auto pMesh = pNode->GetMesh();
        if (!pMesh) return;

        bool isTri = pMesh->IsTriangleMesh();

        FbxStringList uvSetNames;
        pMesh->GetUVSetNames(uvSetNames);
        bool hasUV = uvSetNames.GetCount() != 0;


        for (int polygon = 0, numPoly = pMesh->GetPolygonCount(); polygon < numPoly; ++polygon)
        {
            int numPolygonVertices = pMesh->GetPolygonSize(polygon);
          
            int polygonVertexBegin, polygonVertexEnd, polygonVertexIncr;
            if (ccw)
            {
                polygonVertexBegin = 0;
                polygonVertexEnd = numPolygonVertices;
                polygonVertexIncr = 1;
            }
            else
            {
                polygonVertexBegin = numPolygonVertices - 1;
                polygonVertexEnd = -1;
                polygonVertexIncr = -1;
            }

            if (numPolygonVertices > 3)
            {
                std::vector<Fbx::Vertex> polygonVertices;
                polygonVertices.reserve(static_cast<size_t>(numPolygonVertices));

                // Triangulate
                FbxVector4 pos = FbxVector4(0.0, 0.0, 0.0);
                FbxVector4 norm = FbxVector4(0.0, 0.0, 0.0);
                FbxVector2 uv = FbxVector2(0.0, 0.0);

                int polygonVertex = polygonVertexBegin;
                while (polygonVertex != polygonVertexEnd)
                {
                    polygonVertices.emplace_back();
                    auto vertex = &polygonVertices.back();

                    CreateVertex(pMesh, polygon, polygonVertex, uvSetNames, *vertex);

                    pos += FbxVector4(vertex->Position[0], vertex->Position[1], vertex->Position[2]);
                    norm += FbxVector4(vertex->Normal[0], vertex->Normal[1], vertex->Normal[2]);
                    uv += FbxVector2(vertex->UV[0], vertex->UV[1]);

                    polygonVertex += polygonVertexIncr;
                }


                pos /= numPolygonVertices;
                norm /= numPolygonVertices;
                uv /= numPolygonVertices;

                auto index = pResource->AddVertex(pos.mData, norm.mData, uv.mData);

                // Get winding order
                //float e0[3], e1[3], n[3];
                //TVec<float, 3>::subtract(polygonVertices[1].Position, polygonVertices[0].Position, e0);
                //TVec<float, 3>::subtract(polygonVertices[2].Position, polygonVertices[1].Position, e1);
                //TVec<float, 3>::cross(e0, e1, n);
                //TVec<float, 3>::normalize(n);

                for (int polygonVertex = 0; polygonVertex < numPolygonVertices; ++polygonVertex)
                {
                    auto currentVertex = &polygonVertices[polygonVertex];
                    auto nextVertex = (polygonVertex == numPolygonVertices - 1) ? &polygonVertices[0] : &polygonVertices[polygonVertex + 1];

                    // Adds the center vertex, current vertex, and next vertex in order (triangle fan fashion).
                    // This will already be in the winding order determined by ccw in the prior block.
                    pResource->AddIndex(index);
                    pResource->AddVertex(currentVertex->Position, currentVertex->Normal, currentVertex->UV);
                    pResource->AddVertex(nextVertex->Position, nextVertex->Normal, nextVertex->UV);
                }
            }
            else
            {
                int polygonVertex = polygonVertexBegin;
                while (polygonVertex != polygonVertexEnd)
                {
                    FbxVector4 position = FbxVector4(0.0, 0.0, 0.0);
                    FbxVector4 normal = FbxVector4(0.0, 0.0, 0.0);
                    FbxVector2 uv = FbxVector2(0.0, 0.0);

                    GetVertexAttributes(pMesh, polygon, polygonVertex, uvSetNames, position, normal, uv);
                    pResource->AddVertex(position.mData, normal.mData, uv.mData);

                    polygonVertex += polygonVertexIncr;
                }
            }
        }
    }

    void Fbx::WriteVertexData(void* pData, size_t stride, size_t size, Vertex::Flag flags) const
    {
        LUZASSERT(size == stride * m_vertices.size())

        for (int i = 0, count = (int)m_vertices.size(); i < count; ++i)
        {
            u8* pVertexData = reinterpret_cast<u8*>(pData) + i * stride;
            auto& vertex = m_vertices[i];

            if ((flags & Fbx::Vertex::Flag::POSITION) != 0)
            {
                memcpy_s(pVertexData, 12, vertex.Position, 12);
                pVertexData += 12;
            }

            if ((flags & Fbx::Vertex::Flag::NORMAL) != 0)
            {
                memcpy_s(pVertexData, 12, vertex.Normal, 12);
                pVertexData += 12;
            }

            if ((flags & Fbx::Vertex::Flag::UV) != 0)
            {
                memcpy_s(pVertexData, 8, vertex.UV, 8);
                pVertexData += 8;
            }
        }
    }
}


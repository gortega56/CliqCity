#include "stdafx.h"
#include "Fbx.h"
#include <fbxsdk.h>
#include <fstream>

#define FBXSDK_SHARED
#pragma comment(lib, "libfbxsdk.lib")

namespace Resource
{
    static int s_counterClockwiseOrder[] = { 0, 2, 1 };

    static void GetNodeAttributes(Fbx* pResource, FbxNode* pNode);
    static void GetNodeAttributesRecursively(Fbx* pResource, FbxNode* pNode);
    static void GetMeshAttributes(Fbx* pResource, FbxNode* pNode);

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

    void GetMeshAttributes(Fbx* pResource, FbxNode* pNode)
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
            int numPolyVert = pMesh->GetPolygonSize(polygon);
            if (numPolyVert > 3)
            {
                // Triangulate
                FbxVector4 posAvg = FbxVector4(0.0, 0.0, 0.0);
                FbxVector4 norAvg = FbxVector4(0.0, 0.0, 0.0);
                FbxVector2 texAvg = FbxVector2(0.0, 0.0);

                for (int polyVert = 0; polyVert < numPolyVert; ++polyVert)
                {
                    FbxVector4 p = FbxVector4(0.0, 0.0, 0.0);
                    FbxVector4 n = FbxVector4(0.0, 0.0, 0.0);
                    FbxVector2 t = FbxVector2(0.0, 0.0);

                    GetVertexAttributes(pMesh, polygon, polyVert, uvSetNames, p, n, t);

                    posAvg += p;
                    norAvg += n;
                    texAvg += t;
                }

                posAvg /= numPolyVert;
                norAvg /= numPolyVert;
                texAvg /= numPolyVert;

                auto index = pResource->AddVertex();
                auto vertex = pResource->GetVertex(index);
                ExtractFloat<4, 3>(posAvg.mData, vertex->Position);
                ExtractFloat<4, 3>(norAvg.mData, vertex->Normal);
                ExtractFloat<2>(texAvg.mData, vertex->UV);

                for (int polyVert = 0; polyVert < numPolyVert - 1; ++polyVert)
                {
                    FbxVector4 p = FbxVector4(0.0, 0.0, 0.0);
                    FbxVector4 n = FbxVector4(0.0, 0.0, 0.0);
                    FbxVector2 t = FbxVector2(0.0, 0.0);

                    GetVertexAttributes(pMesh, polygon, polyVert, uvSetNames, p, n, t);

                    pResource->AddVertex(p.mData, n.mData, t.mData);

                    if (polyVert % 2 == 0)
                    {
                        pResource->AddIndex(index);
                    }
                }
            }
            else
            {
                pResource->m_triangles.emplace_back();
                Fbx::Triangle& tri = pResource->m_triangles.back();
                for (int polyVert = 0; polyVert < numPolyVert; ++polyVert)
                {
                    FbxVector4 pos, norm;
                    FbxVector2 tex;
                    GetVertexAttributes(pMesh, polygon, polyVert, uvSetNames, pos, norm, tex);
                    pResource->AddVertex(pos.mData, norm.mData, tex.mData);
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


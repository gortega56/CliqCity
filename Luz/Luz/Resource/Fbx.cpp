#include "stdafx.h"
#include "Fbx.h"
#include <fbxsdk.h>
#include <fstream>

#define FBXSDK_SHARED
#pragma comment(lib, "libfbxsdk.lib")

namespace Resource
{
    static int s_counterClockwiseOrder[] = { 0, 2, 1 };

    static void GetAttributes(Fbx* pResource, FbxNode* pNode);
    static void GetAttributesRecursively(Fbx* pResource, FbxNode* pNode);
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

    //template<int T>
    //static void ReadElement(Fbx* pResource, FbxMesh* pMesh, i32 controlPointIndex, i32 uvIndex, i32 uvLayer, float(&outElement)[T])
    //{
    //    FbxGeometryElementUV* uv = pMesh->GetElementUV()
    //}

    Fbx::Fbx()
    {

    }

    Fbx::~Fbx()
    {

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
        fbxManager->Destroy();

        return nullptr;
    }

    void GetAttributes(Fbx* pResource, FbxNode* pNode)
    {
        LUZASSERT(pResource);
        if (!pNode) return;

        GetAttributesRecursively(pResource, pNode);
    }

    void GetAttributesRecursively(Fbx* pResource, FbxNode* pNode)
    {
        LUZASSERT(pResource);
        if (!pNode) return;

        FbxNodeAttribute* pNodeAttr = pNode->GetNodeAttribute();
        if (!pNodeAttr) return;

        FbxNodeAttribute::EType type = pNodeAttr->GetAttributeType();
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

        for (int i = 0, count = pNode->GetChildCount(); i < count; ++i)
        {
            GetAttributesRecursively(pResource, pNode->GetChild(i));
        }
    }

    template<i32 T0, i32 T1>
    static void ExtractFloat(double (&in)[T0], float (&out)[T1])
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

    void GetMeshAttributes(Fbx* pResource, FbxNode* pNode)
    {
        auto pAttribute = pNode->GetNodeAttribute();
        if (!pAttribute || pAttribute->GetAttributeType() != FbxNodeAttribute::eMesh) return;

        auto pMesh = pNode->GetMesh();
        if (!pMesh) return;

        if (pMesh->IsTriangleMesh()) __debugbreak();

        FbxStringList uvSetNames;
        pMesh->GetUVSetNames(uvSetNames);
        if (uvSetNames.GetCount() == 0) __debugbreak();

        for (int polygon = 0, numPoly = pMesh->GetPolygonCount(); polygon < numPoly; ++polygon)
        {
            for (int polyVert = 0, numPolyVert = pMesh->GetPolygonSize(polygon); polyVert < numPolyVert; ++polyVert)
            {
                int controlPoint = pMesh->GetPolygonVertex(polygon, s_counterClockwiseOrder[polyVert]);
                if (controlPoint == -1) __debugbreak();

                pResource->m_indices.push_back((u32)pResource->m_vertices.size());
                pResource->m_vertices.emplace_back();
                Fbx::Vertex& vertex = pResource->m_vertices.back();

                FbxVector4 tangent;

                FbxVector4 position = pMesh->GetControlPointAt(controlPoint);
                ExtractFloat<4, 3>(position.mData, vertex.Postion);

                FbxVector4 outNormal;
                if (pMesh->GetPolygonVertexNormal(polygon, polyVert, outNormal))
                {
                    ExtractFloat<4, 3>(outNormal.mData, vertex.Normal);
                }

                FbxVector2 outUv;
                bool unmapped;
                if (pMesh->GetPolygonVertexUV(polygon, polyVert, uvSetNames.GetStringAt(0), outUv, unmapped))
                {
                    if (unmapped) __debugbreak();
                    ExtractFloat<2>(outUv.mData, vertex.UV);
                }
            }
        }
    }

    void GetMeshAttributesRecursively(Fbx* pResource, FbxNode* pNode)
    {
        if (!pNode) return;

        auto pAttribute = pNode->GetNodeAttribute();
        if (!pAttribute) return;

        if (pAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
        {
            GetMeshAttributes(pResource, pNode);
        }

        for (int i = 0, n = pNode->GetChildCount(); i < n; ++i)
        {
            GetMeshAttributesRecursively(pResource, pNode->GetChild(i));
        }
    }
}


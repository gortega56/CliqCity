#include "stdafx.h"
#include "Fbx.h"
#include <fbxsdk.h>

#define FBXSDK_SHARED
#pragma comment(lib, "libfbxsdk.lib")

namespace Resource
{
    /* Tab character ("\t") counter */
    int numTabs = 0;

    /**
    * Print the required number of tabs.
    */
    void PrintTabs() {
        for (int i = 0; i < numTabs; i++)
            printf("\t");
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
    void PrintAttribute(FbxNodeAttribute* pAttribute) {
        if (!pAttribute) return;

        FbxString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
        FbxString attrName = pAttribute->GetName();
        PrintTabs();
        // Note: to retrieve the character array of a FbxString, use its Buffer() method.
        printf("<attribute type='%s' name='%s'/>\n", typeName.Buffer(), attrName.Buffer());
    }

    void PrintNode(FbxNode* pNode)
    {
        PrintTabs();
        const char* nodeName = pNode->GetName();
        FbxDouble3 translation = pNode->LclTranslation.Get();
        FbxDouble3 rotation = pNode->LclRotation.Get();
        FbxDouble3 scaling = pNode->LclScaling.Get();

        printf("<node name='%s' translation='(%f, %f, %f)' rotation='(%f, %f, %f)' scaling='(%f, %f, %f)'>\n",
            nodeName,
            translation[0], translation[1], translation[2],
            rotation[0], rotation[1], rotation[2],
            scaling[0], scaling[1], scaling[2]
        );
        numTabs++;

        // Print the node's attributes.
        for (int i = 0; i < pNode->GetNodeAttributeCount(); i++)
            PrintAttribute(pNode->GetNodeAttributeByIndex(i));

        // Recursively print the children.
        for (int j = 0; j < pNode->GetChildCount(); j++)
            PrintNode(pNode->GetChild(j));

        numTabs--;
        PrintTabs();
        printf("</node>\n");
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

        FbxNode* rootNode = fbxScene->GetRootNode();
        if (rootNode)
        {
            for (int i = 0, numNodes = rootNode->GetChildCount(); i < numNodes; ++i)
            {
                PrintNode(rootNode->GetChild(i));
            }
        }

        fbxManager->Destroy();
        return nullptr;
    }
}


#include "stdafx.h"
#include "Fbx.h"
//#include "lina.h"
#include <fbxsdk.h>
#include <fstream>

#define FBXSDK_SHARED
#pragma comment(lib, "libfbxsdk.lib")

#if _WIN32 || _WIN64
#define FBX_DEFAULT_AXIS FbxAxisSystem::EPreDefinedAxisSystem::eDirectX
#elif
#define FBX_DEFAULT_AXIS FbxAxisSystem::EPreDefinedAxisSystem::OpenGL
#endif

//using namespace lina;

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

    static constexpr size_t s_szPosition = sizeof(float) * 3;
    static constexpr size_t s_szNormal = sizeof(float) * 3;
    static constexpr size_t s_szUV = sizeof(float) * 2;

    struct FbxContext
    {
        std::vector<Fbx::Position>* pPositions;
        std::vector<Fbx::Normal>* pNormals;
        std::vector<Fbx::UV>* pUVs;
        std::vector<Fbx::Triangle>* pTris;
        std::vector<Fbx::Surface>* pSurfaces;
    };

    static void ConvertCoordinateSystem(FbxVector4& vector);

    static void ConvertCoordinateSystem(FbxVector2& vector);

    static void ConvertCoordinateSystem(FbxAMatrix& matrix);

    template<typename ... Args> static std::string string_format(const std::string& format, Args ... args);
    
    static std::string ToString(FbxVector4& vector);
    
    static std::string ToString(FbxVector2& vector);
    
    static std::string ToString(float(&vector)[4]);
    
    static std::string ToString(float(&vector)[3]);
    
    static std::string ToString(float(&vector)[2]);

    static void GetNodeAttributes(FbxNode* pNode, FbxContext* pContext);

    static void GetNodeAttributesRecursively(FbxNode* pNode, FbxContext* pContext);

    static void GetMeshAttributes(FbxNode* pNode, FbxContext* pContext);

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

    template<size_t N>
    void ExtractFloats(const FbxDouble* pIn, float* pOut)
    {
        for (size_t i = 0; i < N; ++i)
        {
            pOut[i] = static_cast<float>(pIn[i]);
        }
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

    void GetNodeAttributes(FbxNode* pNode, FbxContext* pContext)
    {
        LUZASSERT(pContext && pNode);

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
                    GetMeshAttributes(pNode, pContext);
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

    void GetNodeAttributesRecursively(FbxNode* pNode, FbxContext* pContext)
    {
        LUZASSERT(pContext);

        if (pNode)
        {
            GetNodeAttributes(pNode, pContext);

            for (int i = 0, n = pNode->GetChildCount(); i < n; ++i)
            {
                GetNodeAttributesRecursively(pNode->GetChild(i), pContext);
            }
        }
    }

    void GetMeshAttributes(FbxNode* pNode, FbxContext* pContext)
    {
        FbxMesh* pMesh = pNode->GetMesh();
        LUZASSERT(pMesh);

        // Check for normals, uvs by checking first vertex
        FbxStringList uvSetNames;
        FbxVector4 normal;
        FbxVector2 uv;
        bool bUnmapped;

        pMesh->GetUVSetNames(uvSetNames);

        unsigned int triStart = static_cast<unsigned int>(pContext->pTris->size());
        bool bHasNormals = pMesh->GetPolygonVertexNormal(0, 0, normal);
        bool bHasUVs = (uvSetNames.GetCount() != 0 && pMesh->GetPolygonVertexUV(0, 0, uvSetNames.GetStringAt(0), uv, bUnmapped));

        // Reserve the memory if we can easily predict it
        bool bIsTriangleMesh = pMesh->IsTriangleMesh();
        if (bIsTriangleMesh)
        {
            pContext->pPositions->reserve(pContext->pPositions->size() + static_cast<size_t>(pMesh->GetPolygonCount() * 3));

            if (bHasNormals)
            {
                pContext->pNormals->reserve(pContext->pNormals->size() + static_cast<size_t>(pMesh->GetPolygonCount() * 3));
            }

            if (bHasUVs)
            {
                pContext->pUVs->reserve(pContext->pUVs->size() + static_cast<size_t>(pMesh->GetPolygonCount() * 3));
            }
        }

        for (int iPolygon = 0, nPolygon = pMesh->GetPolygonCount(); iPolygon < nPolygon; ++iPolygon)
        {
            int nVertices = pMesh->GetPolygonSize(iPolygon);

            // Extract vertex info
            for (int iVertex = 0; iVertex < nVertices; ++iVertex)
            {
                int iControlPoint = pMesh->GetPolygonVertex(iPolygon, iVertex);

                FbxVector4 fbxPosition = pMesh->GetControlPointAt(iControlPoint);
                Fbx::Position& position = pContext->pPositions->emplace_back();
                ExtractFloats<3>(fbxPosition.mData, position.Data);

                FbxVector4 fbxNormal;
                if (pMesh->GetPolygonVertexNormal(iPolygon, iVertex, fbxNormal))
                {
                    Fbx::Normal& normal = pContext->pNormals->emplace_back();
                    ExtractFloats<3>(fbxNormal.mData, normal.Data);
                }
                else
                {
                    LUZASSERT(!bHasNormals);
                }

                FbxVector2 fbxUV;
                if (pMesh->GetPolygonVertexUV(iPolygon, iVertex, uvSetNames.GetStringAt(0), fbxUV, bUnmapped))
                {
                    Fbx::UV& uv = pContext->pUVs->emplace_back();
                    ExtractFloats<2>(fbxUV.mData, uv.Data);
                }
                else
                {
                    LUZASSERT(!bHasUVs);
                }
            }

            // Build triangles
            if (nVertices == 3)
            {
                int nPositions = static_cast<int>(pContext->pPositions->size());
                int nNormals = static_cast<int>(pContext->pNormals->size());
                int nUVs = static_cast<int>(pContext->pUVs->size());

                Fbx::Triangle& tri = pContext->pTris->emplace_back();

                for (int iVertex = 0; iVertex < nVertices; ++iVertex)
                {
                    int offset = (nVertices - iVertex);
                    tri.Positions[iVertex] = nPositions - offset;
                    tri.Normals[iVertex] = nNormals - offset;
                    tri.UVs[iVertex] = nUVs - offset;
                }
            }
            else
            {
                // need to triangulate polygon
                int iBaseVertex = 0;
                int iNextVertex = iBaseVertex + 1;

                int nPositions = static_cast<int>(pContext->pPositions->size());
                int nNormals = static_cast<int>(pContext->pNormals->size());
                int nUVs = static_cast<int>(pContext->pUVs->size());

                for (int iTri = 0, nTris = nVertices - 2; iTri < nTris; ++iTri)
                {
                    Fbx::Triangle& tri = pContext->pTris->emplace_back();
                    tri.Positions[iBaseVertex] = nPositions - nVertices;
                    tri.Normals[iBaseVertex] = nNormals - nVertices;
                    tri.UVs[iBaseVertex] = nUVs - nVertices;

                    for (int i = 0; i < 2; ++i)
                    {
                        int iVertex = iNextVertex + i;
                        int offset = (nVertices - iVertex);
                        tri.Positions[iVertex] = nPositions - offset;
                        tri.Normals[iVertex] = nNormals - offset;
                        tri.UVs[iVertex] = nUVs - offset;
                    }

                    iNextVertex += 1;
                }
            }
        }

        // Add the surface
        Fbx::Surface& surface = pContext->pSurfaces->emplace_back();
        surface.TriStart = triStart;
        surface.NumTris = static_cast<unsigned int>(pContext->pTris->size()) - triStart;
        surface.bHasNormals = bHasNormals;
        surface.bHasUVs = bHasUVs;
    }

    int Fbx::NumSurfaces() const
    {
        return static_cast<int>(m_surfaces.size());
    }

    int Fbx::NumTris(int i) const
    {
        return static_cast<int>(m_surfaces[i].NumTris);
    }

    const Fbx::Position* Fbx::GetPosition(int i) const
    {
        return &m_positions[i];
    }

    const Fbx::Normal* Fbx::GetNormal(int i) const
    {
        return &m_normals[i];
    }

    const Fbx::UV* Fbx::GetUV(int i) const
    {
        return &m_uvs[i];
    }

    const Fbx::Surface* Fbx::GetSurface(int i) const
    {
        return &m_surfaces[i];
    }

    const Fbx::Triangle* Fbx::GetTriangle(int i) const
    {
        return &m_triangles[i];
    }

    void Fbx::ConvertCoordinateSystem()
    {
        for (int i = 0, n = static_cast<int>(m_positions.size()); i < n; ++i)
        {
            m_positions[i].Data[0] = -m_positions[i].Data[0];
        }

        for (int i = 0, n = static_cast<int>(m_normals.size()); i < n; ++i)
        {
            m_normals[i].Data[0] = -m_normals[i].Data[0];
        }

        for (int i = 0, n = static_cast<int>(m_uvs.size()); i < n; ++i)
        {
            m_uvs[i].Data[0] = 1.0f - m_uvs[i].Data[0];
        }
    }

    void Fbx::ReverseWindingOrder()
    {
        auto swap = [](unsigned int* p1, unsigned int* p2)
        {
            unsigned int temp = *p1;
            *p1 = *p2;
            *p2 = temp;
        };

        for (int i = 0, n = static_cast<int>(m_triangles.size()); i < n; ++i)
        {
            swap(&m_triangles[i].Positions[1], &m_triangles[i].Positions[2]);
            swap(&m_triangles[i].Normals[1], &m_triangles[i].Normals[2]);
            swap(&m_triangles[i].UVs[1], &m_triangles[i].UVs[2]);
        }
    }

    std::shared_ptr<const Fbx> Fbx::Load(const Desc& desc)
    {
        std::shared_ptr<Fbx> pResource = nullptr;

        FbxManager* fbxManager = FbxManager::Create();

        FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
        fbxManager->SetIOSettings(ios);

        FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
        if (fbxImporter->Initialize(desc.filename, -1, fbxManager->GetIOSettings()))
        {
            pResource = std::make_shared<Fbx>();

            FbxScene* fbxScene = FbxScene::Create(fbxManager, "");
            fbxImporter->Import(fbxScene);
            fbxImporter->Destroy();

            FbxNode* rootNode = fbxScene->GetRootNode();
            if (rootNode)
            {
                FbxContext ctx;
                ctx.pPositions = &pResource->m_positions;
                ctx.pNormals = &pResource->m_normals;
                ctx.pUVs = &pResource->m_uvs;
                ctx.pTris = &pResource->m_triangles;
                ctx.pSurfaces = &pResource->m_surfaces;

                GetNodeAttributesRecursively(rootNode, &ctx);
            }

            fbxScene->Destroy();
        }

        fbxImporter->Destroy();
        fbxManager->Destroy();

        if (desc.bConvertCoordinateSystem) pResource->ConvertCoordinateSystem();

        if (desc.bReverseWindingOrder) pResource->ReverseWindingOrder();

        return pResource;
    }
}


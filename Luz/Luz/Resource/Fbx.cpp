#include "stdafx.h"
#include "Fbx.h"
//#include "lina.h"
#include <fstream>

#define FBXSDK_SHARED
#include <fbxsdk.h>
#pragma comment(lib, "libfbxsdk.lib")

//using namespace lina;

namespace Resource
{
    static bool g_convertCoordinate = false;
    static bool g_reverseWindingOrder = false;

    static constexpr size_t s_szPosition = sizeof(float) * 3;
    static constexpr size_t s_szNormal = sizeof(float) * 3;
    static constexpr size_t s_szUV = sizeof(float) * 3;

	static constexpr int s_nLayerChannels = FbxLayerElement::eTypeCount;
	static constexpr int s_iTextureCoordinateStartIndex = FbxLayerElement::eTextureDiffuse;
	static constexpr int s_nTextureCoordinateChannels = s_nLayerChannels - s_iTextureCoordinateStartIndex;

    struct FbxContext
    {
        std::vector<Fbx::Position>* pPositions;
        std::vector<Fbx::Normal>* pNormals;
        std::vector<Fbx::UV>* pUVs;
        std::vector<Fbx::Triangle>* pTris;
        std::vector<Fbx::Surface>* pSurfaces;
        std::vector<Fbx::Material>* pMaterials;
        std::vector<std::string>* pTextures;
		Fbx::Flags eFlags;
		bool bConvertYUp;
		bool bNegateZ;
    };

	static void InvertTextureCoordinatesU(Fbx::UV* pUVs, unsigned int nUVs)
	{
		for (unsigned int i = 0; i < nUVs; ++i)
		{
			pUVs[i].Data[0] = 1.0f - pUVs[i].Data[0];
		}
	}

	static void InvertTextureCoordinatesV(Fbx::UV* pUVs, unsigned int nUVs)
	{
		for (unsigned int i = 0; i < nUVs; ++i)
		{
			pUVs[i].Data[1] = 1.0f - pUVs[i].Data[1];
		}
	}

	static void InvertTextureCoordinates(Fbx::UV* pUVs, unsigned int nUVs)
	{
		for (unsigned int i = 0; i < nUVs; ++i)
		{
			pUVs[i].Data[0] = 1.0f - pUVs[i].Data[0];
			pUVs[i].Data[1] = 1.0f - pUVs[i].Data[1];
		}
	}

	static void ConvertCoordinateSystem(FbxVector4& vector)
    {
        // Negate x-axis
        vector.mData[0] = -vector.mData[0];
    }

	static void ConvertCoordinateSystem(FbxVector2& vector)
    {
        vector.mData[0] = 1.0 - vector.mData[0];
    }

	static void ConvertCoordinateSystem(FbxAMatrix& matrix)
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
	static void ExtractFloats(const FbxDouble* pIn, float* pOut)
    {
        for (size_t i = 0; i < N; ++i)
        {
            pOut[i] = static_cast<float>(pIn[i]);
        }
    }

    template<typename ... Args>
	static std::string string_format(const std::string& format, Args ... args)
    {
        size_t size = 64;
        std::unique_ptr<char[]> buf(new char[size]);
        int i = _snprintf_s(buf.get(), size, size, format.c_str(), args ...);
        return std::string(buf.get(), buf.get() + size); // We don't want the '\0' inside
    }

	static std::string ToString(FbxVector4& vector)
    {
        return string_format("(%f, %f, %f, %f)", vector.mData[0], vector.mData[1], vector.mData[2], vector.mData[3]);
    }
    
	static std::string ToString(FbxVector2& vector)
    {
        return string_format("(%f, %f)", vector.mData[0], vector.mData[1]);
    }
    
	static std::string ToString(float(&vector)[4])
    {
        return string_format("(%f, %f, %f, %f)", vector[0], vector[1], vector[2], vector[3]);
    }

	static std::string ToString(float(&vector)[3])
    {
        return string_format("(%f, %f, %f)", vector[0], vector[1], vector[2]);
    }

	static std::string ToString(float(&vector)[2])
    {
        return string_format("(%f, %f)", vector[0], vector[1]);
    }

	static const char* TrimFileName(const char* pFileName)
	{
		size_t i = strlen(pFileName);

		while (i > 0)
		{
			if (pFileName[i] == '\\' || pFileName[i] == '/')
			{
				return pFileName + i + 1;
			}

			i -= 1;
		}

		if (pFileName[0] == '\\' || pFileName[0] == '/')
		{
			return pFileName + 1;
		}

		return pFileName;
	}

	static short CreateUniqueMaterialByName(const char* pName, std::vector<Fbx::Material>* pMaterials)
	{
		short n = static_cast<short>(pMaterials->size());
		for (short i = 0; i < n; ++i)
		{
			if (strcmp((*pMaterials)[i].pName, pName) == 0)
			{
				return i;
			}
		}

		Fbx::Material* pMaterial = &pMaterials->emplace_back();
		pMaterial->pName = pName;

		return n;
	}

	static short CreateUniqueTextureFileName(const char* pName, std::vector<std::string>* pTextureFileNames)
	{
		short n = static_cast<short>(pTextureFileNames->size());
		for (short i = 0; i < n; ++i)
		{
			if (strcmp((*pTextureFileNames)[i].c_str(), pName) == 0)
			{
				return i;
			}
		}

		pTextureFileNames->emplace_back(pName);

		return n;
	}

	static void GetMeshAttributes(FbxNode* pNode, FbxContext* pContext)
	{
		FbxMesh* pMesh = pNode->GetMesh();
		LUZASSERT(pMesh);

		// Get materials for this mesh
		pContext->pMaterials->reserve(pContext->pMaterials->size() + static_cast<size_t>(pNode->GetMaterialCount()));

		short iExportMaterial = -1;

		for (int iMaterial = 0, nMaterials = pNode->GetMaterialCount(); iMaterial < nMaterials; ++iMaterial)
		{
			FbxSurfaceMaterial* pSurfaceMaterial = pNode->GetMaterial(iMaterial);

			iExportMaterial = CreateUniqueMaterialByName(pSurfaceMaterial->GetName(), pContext->pMaterials);

			Fbx::Material* pExportMaterial = &pContext->pMaterials->at(static_cast<size_t>(iExportMaterial));

			// init texture indices
			pExportMaterial->iDiffuse = -1;
			pExportMaterial->iEmissive = -1;
			pExportMaterial->iAmbient = -1;
			pExportMaterial->iSpecular = -1;
			pExportMaterial->iShininess = -1;
			pExportMaterial->iNormal = -1;
			pExportMaterial->iBump = -1;
			pExportMaterial->iAlpha = -1;
			pExportMaterial->iReflection = -1;
			pExportMaterial->iDisplacement = -1;
			pExportMaterial->iDisplacementVector = -1;

			if (FbxSurfaceLambert * pLambert = FbxCast<FbxSurfaceLambert>(pSurfaceMaterial))
			{
				ExtractFloats<3>(pLambert->Emissive.Get().mData, pExportMaterial->EmissiveColor.Data);
				ExtractFloats<3>(pLambert->Ambient.Get().mData, pExportMaterial->AmbientColor.Data);
				ExtractFloats<3>(pLambert->Diffuse.Get().mData, pExportMaterial->DiffuseColor.Data);
				ExtractFloats<3>(pLambert->NormalMap.Get().mData, pExportMaterial->NormalMap.Data);
				ExtractFloats<3>(pLambert->Bump.Get().mData, pExportMaterial->Bump.Data);
				ExtractFloats<3>(pLambert->TransparentColor.Get().mData, pExportMaterial->TransparentColor.Data);
				ExtractFloats<3>(pLambert->DisplacementColor.Get().mData, pExportMaterial->DisplacementColor.Data);
				ExtractFloats<3>(pLambert->VectorDisplacementColor.Get().mData, pExportMaterial->VectorDisplacementColor.Data);

				pExportMaterial->EmissiveFactor = static_cast<float>(pLambert->EmissiveFactor.Get());
				pExportMaterial->AmbientFactor = static_cast<float>(pLambert->AmbientFactor.Get());
				pExportMaterial->DiffuseFactor = static_cast<float>(pLambert->DiffuseFactor.Get());
				pExportMaterial->BumpFactor = static_cast<float>(pLambert->BumpFactor.Get());
				pExportMaterial->TransparencyFactor = static_cast<float>(pLambert->TransparencyFactor.Get());
				pExportMaterial->DisplacementFactor = static_cast<float>(pLambert->DisplacementFactor.Get());
				pExportMaterial->VectorDisplacementFactor = static_cast<float>(pLambert->VectorDisplacementFactor.Get());

				pExportMaterial->eSurface = Fbx::FBX_SURFACE_TYPE_LAMBERT;

				if (FbxSurfacePhong * pPhong = FbxCast<FbxSurfacePhong>(pLambert))
				{
					ExtractFloats<3>(pPhong->Emissive.Get().mData, pExportMaterial->EmissiveColor.Data);
					ExtractFloats<3>(pPhong->Ambient.Get().mData, pExportMaterial->AmbientColor.Data);
					ExtractFloats<3>(pPhong->Diffuse.Get().mData, pExportMaterial->DiffuseColor.Data);
					ExtractFloats<3>(pPhong->NormalMap.Get().mData, pExportMaterial->NormalMap.Data);
					ExtractFloats<3>(pPhong->Bump.Get().mData, pExportMaterial->Bump.Data);
					ExtractFloats<3>(pPhong->TransparentColor.Get().mData, pExportMaterial->TransparentColor.Data);
					ExtractFloats<3>(pPhong->DisplacementColor.Get().mData, pExportMaterial->DisplacementColor.Data);
					ExtractFloats<3>(pPhong->VectorDisplacementColor.Get().mData, pExportMaterial->VectorDisplacementColor.Data);
					ExtractFloats<3>(pPhong->Specular.Get().mData, pExportMaterial->SpecularColor.Data);
					ExtractFloats<3>(pPhong->Reflection.Get().mData, pExportMaterial->ReflectionColor.Data);

					pExportMaterial->EmissiveFactor = static_cast<float>(pPhong->EmissiveFactor.Get());
					pExportMaterial->AmbientFactor = static_cast<float>(pPhong->AmbientFactor.Get());
					pExportMaterial->DiffuseFactor = static_cast<float>(pPhong->DiffuseFactor.Get());
					pExportMaterial->BumpFactor = static_cast<float>(pPhong->BumpFactor.Get());
					pExportMaterial->TransparencyFactor = static_cast<float>(pPhong->TransparencyFactor.Get());
					pExportMaterial->DisplacementFactor = static_cast<float>(pPhong->DisplacementFactor.Get());
					pExportMaterial->VectorDisplacementFactor = static_cast<float>(pPhong->VectorDisplacementFactor.Get());
					pExportMaterial->SpecularFactor = static_cast<float>(pPhong->SpecularFactor.Get());
					pExportMaterial->Shininess = static_cast<float>(pPhong->Shininess.Get());
					pExportMaterial->ReflectionFactor = static_cast<float>(pPhong->ReflectionFactor.Get());

					pExportMaterial->eSurface = Fbx::FBX_SURFACE_TYPE_PHONG;
				}
			}

			int iTextureChannelName;
			FBXSDK_FOR_EACH_TEXTURE(iTextureChannelName)
			{
				FbxProperty prop = pSurfaceMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[iTextureChannelName]);
				if (!prop.IsValid())
				{
					continue;
				}

				for (int iTexture = 0, nTextures = prop.GetSrcObjectCount<FbxTexture>(); iTexture < nTextures; ++iTexture)
				{
					FbxLayeredTexture* pLayeredTexture = prop.GetSrcObject<FbxLayeredTexture>(iTexture);
					if (pLayeredTexture)
					{
						LUZASSERT(0);
						// TODO: Support this
					}
					else
					{
						FbxTexture* pTexture = prop.GetSrcObject<FbxTexture>(iTexture);
						if (pTexture)
						{
							if (FbxFileTexture * pFileTexture = FbxCast<FbxFileTexture>(pTexture))
							{
								short iTextureFileName = CreateUniqueTextureFileName(TrimFileName(pFileTexture->GetFileName()), pContext->pTextures);

								FbxLayerElement::EType eTextureType = static_cast<FbxLayerElement::EType>(FbxLayerElement::sTypeTextureStartIndex + iTextureChannelName);

								switch (eTextureType)
								{
								case (FbxLayerElement::eTextureDiffuse): pExportMaterial->iDiffuse = iTextureFileName; break;
								case (FbxLayerElement::eTextureDiffuseFactor): break;
								case (FbxLayerElement::eTextureEmissive): pExportMaterial->iEmissive = iTextureFileName; break;
								case (FbxLayerElement::eTextureEmissiveFactor): break;
								case (FbxLayerElement::eTextureAmbient): pExportMaterial->iAmbient = iTextureFileName;  break;
								case (FbxLayerElement::eTextureAmbientFactor): break;
								case (FbxLayerElement::eTextureSpecular): pExportMaterial->iSpecular = iTextureFileName; break;
								case (FbxLayerElement::eTextureSpecularFactor): break;
								case (FbxLayerElement::eTextureShininess): pExportMaterial->iShininess = iTextureFileName; break;
								case (FbxLayerElement::eTextureNormalMap): pExportMaterial->iNormal = iTextureFileName; break;
								case (FbxLayerElement::eTextureBump): pExportMaterial->iBump = iTextureFileName; break;
								case (FbxLayerElement::eTextureTransparency): pExportMaterial->iAlpha = iTextureFileName; break;
								case (FbxLayerElement::eTextureTransparencyFactor): break;
								case (FbxLayerElement::eTextureReflection): pExportMaterial->iReflection = iTextureFileName; break;
								case (FbxLayerElement::eTextureReflectionFactor): break;
								case (FbxLayerElement::eTextureDisplacement): pExportMaterial->iDisplacement = iTextureFileName; break;
								case (FbxLayerElement::eTextureDisplacementVector): pExportMaterial->iDisplacementVector = iTextureFileName; break;
								default: LUZASSERT(0) break;
								};
							}
							else if (FbxProceduralTexture * pProceduralTexture = FbxCast<FbxProceduralTexture>(pTexture))
							{
								LUZASSERT(0);
								// TODO: Support this
							}
						}
					}

				}
			}
		}

		/* Get geometry for this mesh */

		if (!pMesh->GetElementNormal())
		{
			if (pContext->eFlags & Fbx::FBX_FLAG_GEN_NORMALS)
			{
				pMesh->InitNormals();
				pMesh->GenerateNormals();
			}
		}

		if (!pMesh->GetElementTangent())
		{
			if (pContext->eFlags & Fbx::FBX_FLAG_GEN_TANGENTS)
			{
				pMesh->InitTangents();
				pMesh->GenerateTangentsDataForAllUVSets();
			}
		}

		FbxStringList uvSetNames;
		pMesh->GetUVSetNames(uvSetNames);
		LUZASSERT(uvSetNames.GetCount() <= 1 /*Only support one uv set at the moment*/);

		// Reserve the memory. If we aren't a tri mesh assume quads
		{
			size_t szPolygons = static_cast<size_t>(pMesh->GetPolygonCount());
			size_t szElements, szTris;

			bool bIsTriangleMesh = pMesh->IsTriangleMesh();
			if (bIsTriangleMesh)
			{
				szElements = szPolygons * 3;
				szTris = szPolygons;
			}
			else
			{
				szElements = szPolygons * 4;
				szTris = szPolygons * 2;
			}

			pContext->pPositions->reserve(pContext->pPositions->size() + szElements);
			pContext->pNormals->reserve(pContext->pNormals->size() + szElements);
			pContext->pUVs->reserve(pContext->pUVs->size() + szElements);
			pContext->pTris->reserve(pContext->pTris->size() + szTris);
		}


		unsigned int iTriStart = static_cast<unsigned int>(pContext->pTris->size());

		for (int iPolygon = 0, nPolygon = pMesh->GetPolygonCount(); iPolygon < nPolygon; ++iPolygon)
		{
			int nVertices = pMesh->GetPolygonSize(iPolygon);

			// Extract vertex info
			for (int iVertex = 0; iVertex < nVertices; ++iVertex)
			{
				int iControlPoint = pMesh->GetPolygonVertex(iPolygon, iVertex);

				FbxVector4 position = pMesh->GetControlPointAt(iControlPoint);
				ExtractFloats<3>(position.mData, pContext->pPositions->emplace_back().Data);

				FbxVector4 normal;
				pMesh->GetPolygonVertexNormal(iPolygon, iVertex, normal);
				ExtractFloats<3>(normal.mData, pContext->pNormals->emplace_back().Data);

				for (int iUVSet = 0, nUVSets = uvSetNames.GetCount(); iUVSet < nUVSets; ++iUVSet)
				{
					bool bUnmapped;
					FbxVector2 uv;
					pMesh->GetPolygonVertexUV(iPolygon, iVertex, uvSetNames.GetStringAt(iUVSet), uv, bUnmapped);
					ExtractFloats<2>(uv.mData, pContext->pUVs->emplace_back().Data);

					if (pContext->eFlags & Fbx::FBX_FLAG_PACK_MATERIAL_TEXTUREW)
					{
						pContext->pUVs->back().Data[2] = static_cast<float>(iExportMaterial);
					}
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
						tri.Positions[i + 1] = nPositions - offset;
						tri.Normals[i + 1] = nNormals - offset;
						tri.UVs[i + 1] = nUVs - offset;
					}

					iNextVertex += 1;
				}
			}
		}

		// Add the surface
		Fbx::Surface& surface = pContext->pSurfaces->emplace_back();
		surface.Name = pMesh->GetName();
		surface.TriStart = iTriStart;
		surface.NumTris = static_cast<unsigned int>(pContext->pTris->size()) - iTriStart;
		surface.bHasNormals = true;
		surface.bHasUVs = true;
		surface.Material = iExportMaterial;
	}

    static void GetNodeAttributes(FbxNode* pNode, FbxContext* pContext)
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

	static void GetNodeAttributesRecursively(FbxNode* pNode, FbxContext* pContext)
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

	const char* Fbx::GetDirectory() const
	{
		return (iDirectory > -1) ? m_names[iDirectory].c_str() : nullptr;
	}

	const char* Fbx::GetTextureDirectory() const
	{
		return (iTextureDirectory > -1) ? m_names[iTextureDirectory].c_str() : nullptr;
	}

	unsigned int Fbx::GetNumPositions() const
	{
		return static_cast<unsigned int>(m_positions.size());
	}

	unsigned int Fbx::GetNumNormals() const
	{
		return static_cast<unsigned int>(m_normals.size());
	}

	unsigned int Fbx::GetNumUVs() const
	{
		return static_cast<unsigned int>(m_uvs.size());
	}

	unsigned int Fbx::GetNumSurfaces() const
    {
        return static_cast<unsigned int>(m_surfaces.size());
    }

	unsigned int Fbx::GetNumTriangles() const
	{
		return static_cast<unsigned int>(m_triangles.size());
	}

	unsigned int Fbx::GetNumMaterials() const
	{
		return static_cast<unsigned int>(m_materials.size());
	}

	const Fbx::Position* Fbx::GetPositions() const
	{
		return m_positions.data();
	}
		  
	const Fbx::Normal* Fbx::GetNormals() const
	{
		return m_normals.data();
	}
		  
	const Fbx::UV* Fbx::GetUVs() const
	{
		return m_uvs.data();
	}
		  
	const Fbx::Surface* Fbx::GetSurfaces() const
	{
		return m_surfaces.data();
	}
		  
	const Fbx::Triangle* Fbx::GetTriangles() const
	{
		return m_triangles.data();
	}

	const Fbx::Material* Fbx::GetMaterials() const
	{
		return m_materials.data();
	}

	const char* Fbx::GetTextureFileName(int i) const
	{
		return (i > -1) ? m_names[i].c_str() : nullptr;
	}

    std::shared_ptr<const Fbx> Fbx::Load(const Desc& desc)
    {
        std::shared_ptr<Fbx> pResource = nullptr;

        FbxManager* pManager = FbxManager::Create();

        FbxIOSettings* pSettings = FbxIOSettings::Create(pManager, IOSROOT);
		pManager->SetIOSettings(pSettings);

        FbxImporter* pImporter = FbxImporter::Create(pManager, "");
        if (pImporter->Initialize(desc.pFileName, -1, pManager->GetIOSettings()))
        {
            pResource = std::make_shared<Fbx>();
			pResource->iTextureDirectory = CreateUniqueTextureFileName(desc.pTextureDirectory, &pResource->m_names);

            FbxScene* pScene = FbxScene::Create(pManager, "");
			pImporter->Import(pScene);
			pImporter->Destroy();

			FbxAxisSystem axis = pScene->GetGlobalSettings().GetAxisSystem();
			FbxAxisSystem::MayaYUp.ConvertScene(pScene);

			int sign;
			FbxAxisSystem::EUpVector up = axis.GetUpVector(sign);
			bool bConvertYUp = (up == FbxAxisSystem::eZAxis);

			FbxAxisSystem::EFrontVector fwd = axis.GetFrontVector(sign);
			bool bNegateZ = (sign == -1);

            FbxNode* pNode = pScene->GetRootNode();
            if (pNode)
            {
                FbxContext ctx;
                ctx.pPositions = &pResource->m_positions;
                ctx.pNormals = &pResource->m_normals;
                ctx.pUVs = &pResource->m_uvs;
                ctx.pTris = &pResource->m_triangles;
                ctx.pSurfaces = &pResource->m_surfaces;
                ctx.pTextures = &pResource->m_names;
                ctx.pMaterials = &pResource->m_materials;
				ctx.eFlags = desc.eFlags;
				ctx.bConvertYUp = bConvertYUp;
				ctx.bNegateZ = bNegateZ;

                GetNodeAttributesRecursively(pNode, &ctx);
            }

            pScene->Destroy();
        }

		pManager->Destroy();

		if (desc.eFlags & Fbx::FBX_FLAG_INVERT_TEXTUREU)
		{
			InvertTextureCoordinatesU(pResource->m_uvs.data(), (unsigned int)pResource->m_uvs.size());
		}
		else if (desc.eFlags & Fbx::FBX_FLAG_INVERT_TEXTUREV)
		{
			InvertTextureCoordinatesV(pResource->m_uvs.data(), (unsigned int)pResource->m_uvs.size());

		}
		else if (desc.eFlags & Fbx::FBX_FLAG_INVERT_TEXTUREUV)
		{
			InvertTextureCoordinates(pResource->m_uvs.data(), (unsigned int)pResource->m_uvs.size());
		}

        return pResource;
    }
}


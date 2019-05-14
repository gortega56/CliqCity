#include "stdafx.h"
#include "Fbx.h"
#include <fstream>

#define FBXSDK_SHARED
#include <fbxsdk.h>
#pragma comment(lib, "libfbxsdk.lib")

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
		std::vector<Fbx::Joint>* pJoints;
		std::vector<Fbx::Skeleton>* pSkeletons;
		std::vector<Fbx::Float4x4>* pTransforms;
		std::vector<Fbx::Influence>* pInfluences;
		std::vector<Fbx::BlendWeight>* pBlendWeights;
		std::vector<Fbx::KeyFrame>* pKeyFrames;
		std::vector<Fbx::Motion>* pMotions;
		std::vector<Fbx::Animation>* pAnimations;
        std::vector<Fbx::Triangle>* pTris;
        std::vector<Fbx::Surface>* pSurfaces;
        std::vector<Fbx::Material>* pMaterials;
        std::vector<std::string>* pStrings;
		Fbx::Flags eFlags;
		bool bConvertYUp;
		bool bNegateZ;
    };

	static void TransformVectors(Fbx::Float3* pVectors, unsigned int nPositions, bool bConvertYUp, bool bNegateZ)
	{
		if (bConvertYUp)
		{
			for (unsigned int i = 0; i < nPositions; ++i)
			{
				pVectors[i].Data[0] = pVectors[i].Data[0];
				pVectors[i].Data[1] = pVectors[i].Data[2];
				pVectors[i].Data[2] = pVectors[i].Data[1];
			}
		}
		else if (bNegateZ)
		{
			for (unsigned int i = 0; i < nPositions; ++i)
			{
				pVectors[i].Data[2] = pVectors[i].Data[2] * -1.0f;
			}
		}
	}

	static void TransformMatrices(Fbx::Float4x4* pMatrices, unsigned int nMatrices)
	{
		// Convert handedness of the transform premultiplying
		// by LH to RH and postmultiplying by RH to LH results
		// in negation of the 3rd row and column.
		for (unsigned int i = 0; i < nMatrices; ++i)
		{
			pMatrices[i].Data[ 2] = -pMatrices[i].Data[ 2];
			pMatrices[i].Data[ 6] = -pMatrices[i].Data[ 6];
			pMatrices[i].Data[14] = -pMatrices[i].Data[14];

			pMatrices[i].Data[ 8] = -pMatrices[i].Data[ 8];
			pMatrices[i].Data[ 9] = -pMatrices[i].Data[ 9];
			pMatrices[i].Data[11] = -pMatrices[i].Data[11];
		}
	}

	static void TransformMatrices(Fbx::Float3x3* pMatrices, unsigned int nMatrices)
	{
		// Convert handedness of the transform premultiplying
		// by LH to RH and postmultiplying by RH to LH results
		// in negation of the 3rd row and column.
		for (unsigned int i = 0; i < nMatrices; ++i)
		{
			pMatrices[i].Data[2] = -pMatrices[i].Data[2];
			pMatrices[i].Data[5] = -pMatrices[i].Data[5];

			pMatrices[i].Data[6] = -pMatrices[i].Data[6];
			pMatrices[i].Data[7] = -pMatrices[i].Data[7];
		}
	}

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

	static short FindOrPushString(const char* pName, std::vector<std::string>* pStrings)
	{
		short n = static_cast<short>(pStrings->size());
		for (short i = 0; i < n; ++i)
		{
			if (strcmp((*pStrings)[i].c_str(), pName) == 0)
			{
				return i;
			}
		}

		pStrings->emplace_back(pName);

		return n;
	}

	static short FindOrPushJoint(unsigned int iName, std::vector<Fbx::Joint>* pJoints)
	{
		short n = static_cast<short>(pJoints->size());
		for (short i = 0; i < n; ++i)
		{
			if ((*pJoints)[i].Name == iName)
			{
				return i;
			}
		}

		Fbx::Joint& joint = pJoints->emplace_back();
		joint.Name = iName;
		
		return n;
	}

	static void GetSkeletonAttributes(FbxNode* pNode, FbxContext* pContext, int iNode, int iParent)
	{
		FbxSkeleton* pSkeleton = pNode->GetSkeleton();
		LUZASSERT(pSkeleton);
		
		if (pSkeleton->IsSkeletonRoot())
		{
			// Add new skeleton
			Fbx::Skeleton& skeleton = pContext->pSkeletons->emplace_back();
			skeleton.Name = FindOrPushString(pSkeleton->GetName(), pContext->pStrings);
			skeleton.JointStart = 0;
			skeleton.NumJoints = 1;

			// Always want root parent index = -1
			iParent = -1;
		}
		else
		{
			// Get current skeleton
			Fbx::Skeleton& skeleton = pContext->pSkeletons->back();
			skeleton.NumJoints += 1;
		}

		// Add joint
		short iNodeName = FindOrPushString(pNode->GetName(), pContext->pStrings);
		short iJoint = FindOrPushJoint(iNodeName, pContext->pJoints);
		Fbx::Joint& joint = (*pContext->pJoints)[iJoint];
		joint.Parent = iParent;
	}

	static void GetMeshAttributes(FbxNode* pNode, FbxContext* pContext, int iNode, int iParent)
	{
		// Get node materials
		short iSurfaceMaterial = -1;
		{
			pContext->pMaterials->reserve(pContext->pMaterials->size() + static_cast<size_t>(pNode->GetMaterialCount()));

			for (int iMaterial = 0, nMaterials = pNode->GetMaterialCount(); iMaterial < nMaterials; ++iMaterial)
			{
				FbxSurfaceMaterial* pSurfaceMaterial = pNode->GetMaterial(iMaterial);

				iSurfaceMaterial = CreateUniqueMaterialByName(pSurfaceMaterial->GetName(), pContext->pMaterials);

				Fbx::Material& material = (*pContext->pMaterials)[iMaterial];

				// init texture indices
				material.iDiffuse = -1;
				material.iEmissive = -1;
				material.iAmbient = -1;
				material.iSpecular = -1;
				material.iShininess = -1;
				material.iNormal = -1;
				material.iBump = -1;
				material.iAlpha = -1;
				material.iReflection = -1;
				material.iDisplacement = -1;
				material.iDisplacementVector = -1;

				if (FbxSurfaceLambert * pLambert = FbxCast<FbxSurfaceLambert>(pSurfaceMaterial))
				{
					ExtractFloats<3>(pLambert->Emissive.Get().mData, material.EmissiveColor.Data);
					ExtractFloats<3>(pLambert->Ambient.Get().mData, material.AmbientColor.Data);
					ExtractFloats<3>(pLambert->Diffuse.Get().mData, material.DiffuseColor.Data);
					ExtractFloats<3>(pLambert->NormalMap.Get().mData, material.NormalMap.Data);
					ExtractFloats<3>(pLambert->Bump.Get().mData, material.Bump.Data);
					ExtractFloats<3>(pLambert->TransparentColor.Get().mData, material.TransparentColor.Data);
					ExtractFloats<3>(pLambert->DisplacementColor.Get().mData, material.DisplacementColor.Data);
					ExtractFloats<3>(pLambert->VectorDisplacementColor.Get().mData, material.VectorDisplacementColor.Data);

					material.EmissiveFactor = static_cast<float>(pLambert->EmissiveFactor.Get());
					material.AmbientFactor = static_cast<float>(pLambert->AmbientFactor.Get());
					material.DiffuseFactor = static_cast<float>(pLambert->DiffuseFactor.Get());
					material.BumpFactor = static_cast<float>(pLambert->BumpFactor.Get());
					material.TransparencyFactor = static_cast<float>(pLambert->TransparencyFactor.Get());
					material.DisplacementFactor = static_cast<float>(pLambert->DisplacementFactor.Get());
					material.VectorDisplacementFactor = static_cast<float>(pLambert->VectorDisplacementFactor.Get());

					material.eSurface = Fbx::FBX_SURFACE_TYPE_LAMBERT;

					if (FbxSurfacePhong * pPhong = FbxCast<FbxSurfacePhong>(pLambert))
					{
						ExtractFloats<3>(pPhong->Emissive.Get().mData, material.EmissiveColor.Data);
						ExtractFloats<3>(pPhong->Ambient.Get().mData, material.AmbientColor.Data);
						ExtractFloats<3>(pPhong->Diffuse.Get().mData, material.DiffuseColor.Data);
						ExtractFloats<3>(pPhong->NormalMap.Get().mData, material.NormalMap.Data);
						ExtractFloats<3>(pPhong->Bump.Get().mData, material.Bump.Data);
						ExtractFloats<3>(pPhong->TransparentColor.Get().mData, material.TransparentColor.Data);
						ExtractFloats<3>(pPhong->DisplacementColor.Get().mData, material.DisplacementColor.Data);
						ExtractFloats<3>(pPhong->VectorDisplacementColor.Get().mData, material.VectorDisplacementColor.Data);
						ExtractFloats<3>(pPhong->Specular.Get().mData, material.SpecularColor.Data);
						ExtractFloats<3>(pPhong->Reflection.Get().mData, material.ReflectionColor.Data);

						material.EmissiveFactor = static_cast<float>(pPhong->EmissiveFactor.Get());
						material.AmbientFactor = static_cast<float>(pPhong->AmbientFactor.Get());
						material.DiffuseFactor = static_cast<float>(pPhong->DiffuseFactor.Get());
						material.BumpFactor = static_cast<float>(pPhong->BumpFactor.Get());
						material.TransparencyFactor = static_cast<float>(pPhong->TransparencyFactor.Get());
						material.DisplacementFactor = static_cast<float>(pPhong->DisplacementFactor.Get());
						material.VectorDisplacementFactor = static_cast<float>(pPhong->VectorDisplacementFactor.Get());
						material.SpecularFactor = static_cast<float>(pPhong->SpecularFactor.Get());
						material.Shininess = static_cast<float>(pPhong->Shininess.Get());
						material.ReflectionFactor = static_cast<float>(pPhong->ReflectionFactor.Get());

						material.eSurface = Fbx::FBX_SURFACE_TYPE_PHONG;
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
									short iTextureFileName = FindOrPushString(TrimFileName(pFileTexture->GetFileName()), pContext->pStrings);

									FbxLayerElement::EType eTextureType = static_cast<FbxLayerElement::EType>(FbxLayerElement::sTypeTextureStartIndex + iTextureChannelName);

									switch (eTextureType)
									{
									case (FbxLayerElement::eTextureDiffuse): material.iDiffuse = iTextureFileName; break;
									case (FbxLayerElement::eTextureDiffuseFactor): break;
									case (FbxLayerElement::eTextureEmissive): material.iEmissive = iTextureFileName; break;
									case (FbxLayerElement::eTextureEmissiveFactor): break;
									case (FbxLayerElement::eTextureAmbient): material.iAmbient = iTextureFileName;  break;
									case (FbxLayerElement::eTextureAmbientFactor): break;
									case (FbxLayerElement::eTextureSpecular): material.iSpecular = iTextureFileName; break;
									case (FbxLayerElement::eTextureSpecularFactor): break;
									case (FbxLayerElement::eTextureShininess): material.iShininess = iTextureFileName; break;
									case (FbxLayerElement::eTextureNormalMap): material.iNormal = iTextureFileName; break;
									case (FbxLayerElement::eTextureBump): material.iBump = iTextureFileName; break;
									case (FbxLayerElement::eTextureTransparency): material.iAlpha = iTextureFileName; break;
									case (FbxLayerElement::eTextureTransparencyFactor): break;
									case (FbxLayerElement::eTextureReflection): material.iReflection = iTextureFileName; break;
									case (FbxLayerElement::eTextureReflectionFactor): break;
									case (FbxLayerElement::eTextureDisplacement): material.iDisplacement = iTextureFileName; break;
									case (FbxLayerElement::eTextureDisplacementVector): material.iDisplacementVector = iTextureFileName; break;
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
		}

		FbxMesh* pMesh = pNode->GetMesh();
		LUZASSERT(pMesh);

		// Get mesh deformers
		std::unordered_map<int, std::vector<Fbx::BlendWeight>> blendWeightsByControlPointIndex;
		{
			for (int iDeformer = 0, nDeformers = pMesh->GetDeformerCount(FbxDeformer::eSkin); iDeformer < nDeformers; ++iDeformer)
			{
				FbxSkin* pSkin = FbxCast<FbxSkin>(pMesh->GetDeformer(iDeformer, FbxDeformer::eSkin));

				for (int iCluster = 0, nClusters = pSkin->GetClusterCount(); iCluster < nClusters; ++iCluster)
				{
					FbxCluster* pCluster = pSkin->GetCluster(iCluster);

					FbxAMatrix bindPose;
					pCluster->GetTransformLinkMatrix(bindPose);

					FbxAMatrix global;
					pCluster->GetTransformMatrix(global);

					FbxMatrix invBindPose = global.Inverse() * bindPose;

					Fbx::Float4x4& transform = pContext->pTransforms->emplace_back();
					ExtractFloats<4>(invBindPose.GetRow(0).mData, &transform.Data[0]);
					ExtractFloats<4>(invBindPose.GetRow(1).mData, &transform.Data[4]);
					ExtractFloats<4>(invBindPose.GetRow(2).mData, &transform.Data[8]);
					ExtractFloats<4>(invBindPose.GetRow(3).mData, &transform.Data[12]);

					FbxNode* pLinkedNode = pCluster->GetLink();
					short iLinkedNodeName = FindOrPushString(pLinkedNode->GetName(), pContext->pStrings);
					short iJoint = FindOrPushJoint(iLinkedNodeName, pContext->pJoints);

					Fbx::Joint& joint = (*pContext->pJoints)[iJoint];
					joint.InverseBindPose = static_cast<short>(pContext->pTransforms->size());

					int* pIndices = pCluster->GetControlPointIndices();
					double* pWeights = pCluster->GetControlPointWeights();

					for (int iControlPoint = 0, nControlPoints = pCluster->GetControlPointIndicesCount(); iControlPoint < nControlPoints; ++iControlPoint)
					{
						std::vector<Fbx::BlendWeight>& blendWeights = blendWeightsByControlPointIndex[pIndices[iControlPoint]];
						Fbx::BlendWeight& bw = blendWeights.emplace_back();
						bw.Joint = iJoint;
						bw.Weight = static_cast<float>(pWeights[iControlPoint]);
					}
				}
			}
		}

		// Get mesh attributes
		unsigned int iTriStart = static_cast<unsigned int>(pContext->pTris->size());
		unsigned short maxBlends = 0;
		bool bHasNormals, bHasTangents, bHasUVs, bHasBlends;
		{
			bHasNormals = (pMesh->GetElementNormal() != nullptr);
			if (!bHasNormals && (pContext->eFlags & Fbx::FBX_FLAG_GEN_NORMALS))
			{
				pMesh->InitNormals();
				pMesh->GenerateNormals();
				bHasNormals = true;
			}

			bHasTangents = (pMesh->GetElementTangent() != nullptr);
			if (!bHasTangents && (pContext->eFlags & Fbx::FBX_FLAG_GEN_TANGENTS))
			{
				pMesh->InitTangents();
				pMesh->GenerateTangentsDataForAllUVSets();
				bHasTangents = true;
			}

			FbxStringList uvSetNames;
			pMesh->GetUVSetNames(uvSetNames);
			LUZASSERT(uvSetNames.GetCount() <= 1 /*Only support one uv set at the moment*/);

			bHasUVs = uvSetNames.GetCount() != 0;

			bHasBlends = blendWeightsByControlPointIndex.size() != 0;

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

				pContext->pTris->reserve(pContext->pTris->size() + szTris);
				pContext->pPositions->reserve(pContext->pPositions->size() + szElements);

				if (bHasNormals) pContext->pNormals->reserve(pContext->pNormals->size() + szElements);
				if (bHasUVs) pContext->pUVs->reserve(pContext->pUVs->size() + szElements);
			}

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
							pContext->pUVs->back().Data[2] = static_cast<float>(iSurfaceMaterial);
						}
					}

					auto iter = blendWeightsByControlPointIndex.find(iControlPoint);
					if (iter != blendWeightsByControlPointIndex.end())
					{
						std::vector<Fbx::BlendWeight>& blendWeights = iter->second;
						unsigned short blendWeightStart = static_cast<unsigned short>(pContext->pBlendWeights->size());
						unsigned short nBlendWeights = static_cast<unsigned short>(blendWeights.size());

						pContext->pBlendWeights->resize(pContext->pBlendWeights->size() + blendWeights.size());
						pContext->pBlendWeights->insert(pContext->pBlendWeights->end(), blendWeights.begin(), blendWeights.end());

						Fbx::Influence & influence = pContext->pInfluences->emplace_back();
						influence.BlendWeightStart = blendWeightStart;
						influence.NumBlendWeights = nBlendWeights;

						// Book keeping
						maxBlends = (std::max)(maxBlends, nBlendWeights);
					}
				}

				// Build triangles
				if (nVertices == 3)
				{
					int nPositions = static_cast<int>(pContext->pPositions->size());
					int nNormals = static_cast<int>(pContext->pNormals->size());
					int nUVs = static_cast<int>(pContext->pUVs->size());
					int nInfluences = static_cast<int>(pContext->pInfluences->size());

					Fbx::Triangle& tri = pContext->pTris->emplace_back();

					for (int iVertex = 0; iVertex < nVertices; ++iVertex)
					{
						int offset = (nVertices - iVertex);
						tri.Positions[iVertex] = nPositions - offset;
						tri.Normals[iVertex] = nNormals - offset;
						tri.UVs[iVertex] = nUVs - offset;
						tri.Influences[iVertex] = nInfluences - offset;
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
					int nInfluences = static_cast<int>(pContext->pInfluences->size());

					for (int iTri = 0, nTris = nVertices - 2; iTri < nTris; ++iTri)
					{
						Fbx::Triangle& tri = pContext->pTris->emplace_back();
						tri.Positions[iBaseVertex] = nPositions - nVertices;
						tri.Normals[iBaseVertex] = nNormals - nVertices;
						tri.UVs[iBaseVertex] = nUVs - nVertices;
						tri.Influences[iBaseVertex] = nInfluences - nVertices;

						for (int i = 0; i < 2; ++i)
						{
							int iVertex = iNextVertex + i;
							int offset = (nVertices - iVertex);
							tri.Positions[i + 1] = nPositions - offset;
							tri.Normals[i + 1] = nNormals - offset;
							tri.UVs[i + 1] = nUVs - offset;
							tri.Influences[i + 1] = nInfluences - offset;
						}

						iNextVertex += 1;
					}
				}
			}
		}
		
		// Add the surface
		Fbx::Surface& surface = pContext->pSurfaces->emplace_back();
		surface.Name = pMesh->GetName();
		surface.TriStart = iTriStart;
		surface.NumTris = static_cast<unsigned int>(pContext->pTris->size()) - iTriStart;
		surface.Material = iSurfaceMaterial;
		surface.MaxBlends = maxBlends;
		surface.bHasNormals = bHasNormals;
		surface.bHasUVs = bHasUVs;
		surface.bHasBlends = bHasBlends;
	}

    static int GetNodeAttributes(FbxNode* pNode, FbxNodeAttribute::EType eNodeAttributeType, FbxContext* pContext, int iNode, int iParent)
    {
        LUZASSERT(pContext && pNode);

		int nFound = 0;

		for (int iAttribute = 0, nAttributes = pNode->GetNodeAttributeCount(); iAttribute < nAttributes; ++iAttribute)
		{
			if (FbxNodeAttribute * pNodeAttribute = pNode->GetNodeAttributeByIndex(iAttribute))
			{
				FbxNodeAttribute::EType eAttributeType = pNodeAttribute->GetAttributeType();

				if (eAttributeType == eNodeAttributeType)
				{
					switch (eAttributeType)
					{
					case FbxNodeAttribute::eUnknown:
					case FbxNodeAttribute::eNull:
					case FbxNodeAttribute::eMarker:
					case FbxNodeAttribute::eSkeleton:
						GetSkeletonAttributes(pNode, pContext, iNode, iParent);
						break;
					case FbxNodeAttribute::eMesh:
						GetMeshAttributes(pNode, pContext, iNode, iParent);
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
						LUZASSERT(0); // Modify supported types
						break;
					}

					nFound += 1;
				}
			}
		}

		return nFound;
    }

	static void GetNodeAttributesRecursively(FbxNode* pNode, FbxNodeAttribute::EType eNodeAttributeType, FbxContext* pContext, int iNode, int iParent, int& nNodes)
    {
		LUZASSERT(pNode);
        LUZASSERT(pContext);

		int nFound = GetNodeAttributes(pNode, eNodeAttributeType, pContext, iNode, iParent);
		if (nFound == 0)
		{
			iNode = iParent;
		}

		nNodes += nFound;

		for (int i = 0, n = pNode->GetChildCount(); i < n; ++i)
		{
			GetNodeAttributesRecursively(pNode->GetChild(i), eNodeAttributeType, pContext, nNodes, iNode, nNodes);
		}
    }

	static void GetAnimations(FbxScene* pScene, FbxContext* pContext)
	{
		for (int iAnimStack = 0, nAnimStacks = pScene->GetSrcObjectCount<FbxAnimStack>(); iAnimStack < nAnimStacks; ++iAnimStack)
		{
			FbxAnimStack* pAnimStack = pScene->GetSrcObject<FbxAnimStack>(iAnimStack);			
			
			FbxTime singleFrameDuration;
			singleFrameDuration.SetTime(0, 0, 0, 1, 0, pScene->GetGlobalSettings().GetTimeMode());

			double frameDuration = singleFrameDuration.GetSecondDouble();
			double frameSampleRate = frameDuration / 1.0f; // one sample per frame... expose this
			LUZASSERT(frameDuration > 0.0);

			unsigned short nFrames;
			double startTime, stopTime;
			if (FbxTakeInfo* pTakeInfo = pScene->GetTakeInfo(pAnimStack->GetName()))
			{
				FbxTimeSpan& span = pTakeInfo->mLocalTimeSpan;
				startTime = span.GetStart().GetSecondDouble();
				stopTime = span.GetStop().GetSecondDouble();
				nFrames = static_cast<unsigned short>(span.GetStop().GetFrameCount(pScene->GetGlobalSettings().GetTimeMode()));
			}
			else
			{
				FbxTimeSpan span;
				pScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(span);
				startTime = span.GetStart().GetSecondDouble();
				stopTime = span.GetStop().GetSecondDouble();
				nFrames = static_cast<unsigned short>(span.GetStop().GetFrameCount(pScene->GetGlobalSettings().GetTimeMode()));
			}
			
			double animDuration = stopTime - startTime;
			
			for (int iSkeleton = 0, nSkeletons = static_cast<int>(pContext->pSkeletons->size()); iSkeleton < nSkeletons; ++iSkeleton)
			{
				Fbx::Skeleton& skeleton = (*pContext->pSkeletons)[iSkeleton];

				unsigned short motionsStart = static_cast<unsigned short>(pContext->pMotions->size());

				for (int iJoint = skeleton.JointStart; iJoint < skeleton.NumJoints; ++iSkeleton)
				{
					unsigned short keyFrameStart = static_cast<unsigned short>(pContext->pKeyFrames->size());

					// Process an entire sequence of key frame for each joint. We want to preserve this order
					// on extraction because on play back we need 2 key frames for interpolation
					double currentTime = startTime;

					while (currentTime <= stopTime)
					{
						FbxTime animTime;
						animTime.SetSecondDouble(currentTime);

						Fbx::Joint& joint = (*pContext->pJoints)[iJoint];
						const char* pNodeName = (*pContext->pStrings)[joint.Name].c_str();
						FbxNode* pNode = pScene->FindNodeByName(pNodeName);

						FbxAMatrix pose = pScene->GetAnimationEvaluator()->GetNodeGlobalTransform(pNode, animTime);
						FbxQuaternion q = pose.GetQ();
						FbxVector4 t = pose.GetT();
						FbxVector4 s = pose.GetS();

						Fbx::KeyFrame& keyFrame = pContext->pKeyFrames->emplace_back();
						ExtractFloats<4>(q.mData, keyFrame.Quaternion.Data);
						ExtractFloats<3>(t.mData, keyFrame.Translation.Data);
						ExtractFloats<3>(s.mData, keyFrame.Scale.Data);
						keyFrame.Time = static_cast<float>(currentTime);

						currentTime += frameSampleRate;
					}

					Fbx::Motion& motion = pContext->pMotions->emplace_back();
					motion.Joint = iJoint;
					motion.KeyFrameStart = keyFrameStart;
					motion.NumKeyFrames = static_cast<unsigned short>(pContext->pKeyFrames->size()) - keyFrameStart;
				}

				Fbx::Animation& anim = pContext->pAnimations->emplace_back();
				anim.Name = FindOrPushString(pAnimStack->GetName(), pContext->pStrings);
				anim.Skeleton = iSkeleton;
				anim.MotionsStart = motionsStart;
				anim.NumMotions = static_cast<unsigned short>(pContext->pMotions->size()) - motionsStart;
				anim.Duration = static_cast<float>(animDuration);
				anim.NumFrames = nFrames;
			}
		}
	}

	static void FinalizeContext(FbxContext* pContext)
	{
		unsigned short iJoint = 0;
		for (int iSkeleton = 0, nSkeletons = static_cast<int>(pContext->pSkeletons->size()); iSkeleton < nSkeletons; ++iSkeleton)
		{
			Fbx::Skeleton& skeleton = (*pContext->pSkeletons)[iSkeleton];
			skeleton.JointStart = iJoint;
			iJoint += skeleton.NumJoints;
		}
	}

	const char* Fbx::GetDirectory() const
	{
		return (iDirectory > -1) ? m_strings[iDirectory].c_str() : nullptr;
	}

	const char* Fbx::GetTextureDirectory() const
	{
		return (iTextureDirectory > -1) ? m_strings[iTextureDirectory].c_str() : nullptr;
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

	unsigned int Fbx::GetNumKeyFrames() const
	{
		return static_cast<int>(m_keyFrames.size());
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
		return (i > -1) ? m_strings[i].c_str() : nullptr;
	}

	const Fbx::KeyFrame* Fbx::GetKeyFrames() const
	{
		return m_keyFrames.data();
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
			pResource->iTextureDirectory = FindOrPushString(desc.pTextureDirectory, &pResource->m_strings);

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
				ctx.pJoints = &pResource->m_joints;
				ctx.pSkeletons = &pResource->m_skeletons;
				ctx.pTransforms = &pResource->m_transforms;
				ctx.pBlendWeights = &pResource->m_blendWeights;
				ctx.pInfluences = &pResource->m_influences;
				ctx.pKeyFrames = &pResource->m_keyFrames;
				ctx.pMotions = &pResource->m_motions;
				ctx.pAnimations = &pResource->m_animations;
                ctx.pTris = &pResource->m_triangles;
                ctx.pSurfaces = &pResource->m_surfaces;
                ctx.pStrings = &pResource->m_strings;
                ctx.pMaterials = &pResource->m_materials;
				ctx.eFlags = desc.eFlags;
				ctx.bConvertYUp = bConvertYUp;
				ctx.bNegateZ = bNegateZ;

				int nSkeletonNodes = 0, nMeshNodes = 0;
				GetNodeAttributesRecursively(pNode, FbxNodeAttribute::eSkeleton, &ctx, 0, -1, nSkeletonNodes);
				GetNodeAttributesRecursively(pNode, FbxNodeAttribute::eMesh, &ctx, 0, -1, nSkeletonNodes);

				FinalizeContext(&ctx);

				GetAnimations(pScene, &ctx);
            }
			
            pScene->Destroy();

			TransformVectors(pResource->m_positions.data(), static_cast<unsigned int>(pResource->m_positions.size()), bConvertYUp, bNegateZ);
			TransformVectors(pResource->m_normals.data(), static_cast<unsigned int>(pResource->m_normals.size()), bConvertYUp, bNegateZ);

			if (bNegateZ)
			{
				TransformMatrices(pResource->m_transforms.data(), static_cast<unsigned int>(pResource->m_transforms.size()));
			}
        }

		pManager->Destroy();

		if (desc.eFlags & Fbx::FBX_FLAG_INVERT_TEXTUREUV)
		{
			if ((desc.eFlags & Fbx::FBX_FLAG_INVERT_TEXTUREU) == 0)
			{
				InvertTextureCoordinatesV(pResource->m_uvs.data(), (unsigned int)pResource->m_uvs.size());
			}
			else if ((desc.eFlags & Fbx::FBX_FLAG_INVERT_TEXTUREV) == 0)
			{
				InvertTextureCoordinatesU(pResource->m_uvs.data(), (unsigned int)pResource->m_uvs.size());
			}
			else
			{
				InvertTextureCoordinates(pResource->m_uvs.data(), (unsigned int)pResource->m_uvs.size());
			}
		}

        return pResource;
    }
}


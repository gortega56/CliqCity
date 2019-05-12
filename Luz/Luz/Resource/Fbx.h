#pragma once
#ifndef FBXRESOURCE_H
#define FBXRESOURCE_H

#ifndef MESHRESOURCEUTILITY_H
#include "MeshResourceUtility.h"
#endif

namespace Resource
{
    class Fbx
    {
    public:
		using Float4x4	 = TArray<float, 16>;
		using Float3x3	 = TArray<float, 9>;
		using Float3	 = TArray<float, 3>;
        using Position	 = Float3;
        using Normal	 = Float3;
        using UV		 = Float3;
        using Color		 = Float3;

		enum Flags
		{
			FBX_FLAG_NONE = 0,
			FBX_FLAG_INVERT_TEXTUREU	= 1 << 1,
			FBX_FLAG_INVERT_TEXTUREV	= 1 << 2,
			FBX_FLAG_INVERT_TEXTUREUV	= FBX_FLAG_INVERT_TEXTUREU | FBX_FLAG_INVERT_TEXTUREV,
			FBX_FLAG_GEN_NORMALS	= 1 << 3,
			FBX_FLAG_GEN_TANGENTS	= 1 << 4,
			FBX_FLAG_GEN_BINORMALS	= 1 << 5,
			FBX_FLAG_PACK_MATERIAL_TEXTUREW = 1 << 6
		};

		struct LUZ_API Skeleton
		{
			unsigned short Name;
			unsigned short JointStart;
			unsigned short NumJoints;
		};

		struct LUZ_API Joint
		{
			short Name;
			short InverseBindPose;
			short Parent;
		};

		struct LUZ_API BlendWeight
		{
			int Joint;
			float Weight;
		};

		struct LUZ_API Influence
		{
			unsigned short BlendWeightStart;
			unsigned short NumBlendWeights;
		};

        struct LUZ_API Triangle
        {
            unsigned int Positions[3];
            unsigned int Normals[3];
            unsigned int UVs[3];
			unsigned int Influences[3];
        };

        struct LUZ_API Surface
        {
            const char* Name;
            unsigned int TriStart;
            unsigned int NumTris;
            short Material;
			short MaxBlends;
            bool bHasNormals;
            bool bHasUVs;
			bool bHasBlends;
        };

        struct LUZ_API Desc
        {
            const char* pFileName;
			const char* pTextureDirectory;
            bool bConvertCoordinateSystem;
            bool bReverseWindingOrder;
			Flags eFlags;
        };

        enum SurfaceType
        {
            FBX_SURFACE_TYPE_PHONG,
            FBX_SURFACE_TYPE_LAMBERT
        };

        struct Material
        {
			const char* pName;
			
			Color EmissiveColor;
			float EmissiveFactor;
			Color AmbientColor;
			float AmbientFactor;
			Color DiffuseColor;
			float DiffuseFactor;
			Float3 NormalMap;
			Float3 Bump;
			float BumpFactor;
			Color TransparentColor;
			float TransparencyFactor;
			Color DisplacementColor;
			float DisplacementFactor;
			Color VectorDisplacementColor;
			float VectorDisplacementFactor;
			Color SpecularColor;
			float SpecularFactor;
			float Shininess;
			Color ReflectionColor;
			float ReflectionFactor;

            short iDiffuse;
			short iEmissive;
            short iAmbient;
            short iSpecular;
			short iShininess;
            short iNormal;
			short iBump;
            short iAlpha;
            short iReflection;
            short iDisplacement;
			short iDisplacementVector;

            SurfaceType eSurface;
        };

        Fbx() = default;
       
        ~Fbx() = default;

		const LUZ_API char* GetDirectory() const;

		const LUZ_API char* GetTextureDirectory() const;

		LUZ_API unsigned int GetNumPositions() const;
		 
		LUZ_API unsigned int GetNumNormals() const;
		
		LUZ_API unsigned int GetNumUVs() const;

        LUZ_API unsigned int GetNumSurfaces() const;

		LUZ_API unsigned int GetNumTriangles() const;

		LUZ_API unsigned int GetNumMaterials() const;

		const LUZ_API Position* GetPositions() const;

		const LUZ_API Normal* GetNormals() const;

		const LUZ_API UV* GetUVs() const;

		const LUZ_API Surface* GetSurfaces() const;

		const LUZ_API Triangle* GetTriangles() const;

		const LUZ_API Material* GetMaterials() const;

		const LUZ_API char* GetTextureFileName(int i) const;
        
        static LUZ_API std::shared_ptr<const Fbx> Load(const Desc& desc);

    private:
        std::vector<Position> m_positions;
        std::vector<Normal> m_normals;
        std::vector<UV> m_uvs;
		std::vector<Joint> m_joints;
		std::vector<Skeleton> m_skeletons;
		std::vector<Float4x4> m_transforms;
		std::vector<Influence> m_influences;
		std::vector<BlendWeight> m_blendWeights;
		std::vector<Triangle> m_triangles;
        std::vector<Surface> m_surfaces;
        std::vector<Material> m_materials;
		std::vector<std::string> m_strings;

		int iDirectory = -1;
		int iTextureDirectory = -1;
    };

	inline Fbx::Flags operator|(const Fbx::Flags a, const Fbx::Flags b)
	{
		return static_cast<Fbx::Flags>(static_cast<unsigned>(a) | static_cast<unsigned>(b));
	}

	inline Fbx::Flags operator&(const Fbx::Flags a, const Fbx::Flags b)
	{
		return static_cast<Fbx::Flags>(static_cast<unsigned>(a) & static_cast<unsigned>(b));
	}
}



#endif // !FBXRESOURCE_H

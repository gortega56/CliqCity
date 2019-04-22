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
		using Float3 = TArray<float, 3>;
        using Position = Float3;
        using Normal =Float3;
        using UV = Float3;
        using Color = Float3;

		enum Flags
		{
			FBX_FLAG_NONE = 0,
			FBX_FLAG_INVERT_UV_X = 1,
			FBX_FLAG_INVERT_UV_Y = 2,
			FBX_FLAG_INVERT_UV	 = FBX_FLAG_INVERT_UV_X | FBX_FLAG_INVERT_UV_Y
		};

        struct LUZ_API JointBlend
        {
            unsigned int Index;
            unsigned int Weight;
        };

        struct LUZ_API Triangle
        {
            unsigned int Positions[3];
            unsigned int Normals[3];
            unsigned int UVs[3];
        };

        struct LUZ_API Surface
        {
            const char* Name;
            unsigned int TriStart;
            unsigned int NumTris;
            short Material;
            bool bHasNormals;
            bool bHasUVs;
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

        LUZ_API int GetNumSurfaces() const;

		LUZ_API int GetNumMaterials() const;

		const LUZ_API Position* GetPositions() const;

		const LUZ_API Normal* GetNormals() const;

		const LUZ_API UV* GetUVs() const;

		const LUZ_API Surface* GetSurfaces() const;

		const LUZ_API Triangle* GetTriangles() const;

		const LUZ_API Material* GetMaterials() const;

        const LUZ_API Position* GetPosition(int i) const;

        const LUZ_API Normal* GetNormal(int i) const;

        const LUZ_API UV* GetUV(int i) const;

        const LUZ_API Surface* GetSurface(int i) const;

        const LUZ_API Triangle* GetTriangle(int i) const;

		const LUZ_API char* GetTextureFileName(int i) const;

        LUZ_API void ConvertCoordinateSystem();

        LUZ_API void ReverseWindingOrder();
        
        static LUZ_API std::shared_ptr<const Fbx> Load(const Desc& desc);

    private:
        std::vector<Position> m_positions;
        std::vector<Normal> m_normals;
        std::vector<UV> m_uvs;
        std::vector<Triangle> m_triangles;
        std::vector<Surface> m_surfaces;
        std::vector<Material> m_materials;
        std::vector<std::string> m_names;
        std::unordered_map<i32, u32> m_indicesByControlPointIndex;
        std::unordered_map<i32, JointBlend> m_blendByControlPointIndex;

		int iDirectory = -1;
		int iTextureDirectory = -1;
    };
}

#endif // !FBXRESOURCE_H

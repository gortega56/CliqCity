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
        using Position = TArray<float, 3>;
        using Normal = TArray<float, 3>;
        using UV = TArray<float, 3>;

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
            unsigned char* Name;
            unsigned int TriStart;
            unsigned int NumTris;

            bool bHasNormals;
            bool bHasUVs;
        };

        struct LUZ_API Desc
        {
            const char* filename;
            bool bConvertCoordinateSystem;
            bool bReverseWindingOrder;
        };

        Fbx() = default;
       
        ~Fbx() = default;

        LUZ_API int NumSurfaces() const;

        LUZ_API int NumTris(int i) const;

        const LUZ_API Position* GetPosition(int i) const;

        const LUZ_API Normal* GetNormal(int i) const;

        const LUZ_API UV* GetUV(int i) const;

        const LUZ_API Surface* GetSurface(int i) const;

        const LUZ_API Triangle* GetTriangle(int i) const;

        LUZ_API void ConvertCoordinateSystem();

        LUZ_API void ReverseWindingOrder();
        
        static LUZ_API std::shared_ptr<const Fbx> Load(const Desc& desc);

    private:
        std::vector<Position> m_positions;
        std::vector<Normal> m_normals;
        std::vector<UV> m_uvs;
        std::vector<Triangle> m_triangles;
        std::vector<Surface> m_surfaces;

        std::unordered_map<i32, u32> m_indicesByControlPointIndex;
        std::unordered_map<i32, JointBlend> m_blendByControlPointIndex;
    };
}

#endif // !FBXRESOURCE_H

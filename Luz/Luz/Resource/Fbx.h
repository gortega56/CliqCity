#pragma once
#ifndef FBXRESOURCE_H
#define FBXRESOURCE_H

namespace Resource
{
    class Fbx
    {
    public:
        LUZ_API Fbx();
        LUZ_API ~Fbx();

        static std::shared_ptr<const Fbx> LUZ_API Load(const std::wstring& filename);

        struct Vertex
        {
            u32 BlendIndices[4];
            float BlendWeights[4];
            float Postion[3];
            float Normal[3];
            float Tangent[3];
            float UV[2];

            Vertex() {}
        };

        struct Triangle
        {
            Vertex Corner[3];
        };

        struct JointBlend
        {
            i32 Index;
            float Weight;

            JointBlend() {}
        };

        std::vector<Triangle> m_triangles;
        std::vector<Vertex> m_vertices;
        std::vector<u32> m_indices;
        std::unordered_map<i32, u32> m_indicesByControlPointIndex;
        std::unordered_map<i32, JointBlend> m_blendByControlPointIndex;
    };
}

#endif // !FBXRESOURCE_H

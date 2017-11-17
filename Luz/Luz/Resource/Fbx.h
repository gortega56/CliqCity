#pragma once
#ifndef FBXRESOURCE_H
#define FBXRESOURCE_H

namespace Resource
{
    class LUZ_API Fbx
    {
    public:
        Fbx();
        ~Fbx();

        static std::shared_ptr<const Fbx> Load(const std::wstring& filename);

    private:
        struct Vertex
        {
            u32 BlendIndices[4];
            float BlendWeights[4];
            float Postion[3];
            float Normal[3];
            float Tangent[3];
            float UV[2];
        };

        std::vector<Vertex> m_vertices;
        std::vector<u32> m_indices;

    };
}

#endif // !FBXRESOURCE_H

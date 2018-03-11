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
        typedef Vertex2 Vertex;

        static std::shared_ptr<const Fbx> LUZ_API Load(const std::wstring& filename);

        LUZ_API Fbx();
        LUZ_API ~Fbx();

        Vertex* GetVertex(u32 i);

        u32 AddVertex();
        u32 AddVertex(float(&position)[3], float(&normal)[3], float(&uv)[2]);
        u32 AddVertex(double(&position)[4], double(&normal)[4], double(&uv)[2]);

        void AddIndex(u32 i);

        const std::vector<u32>& GetIndices() const { return m_indices; }

        template<class VertexType>
        void WriteVertices(std::vector<VertexType>& outVertices, std::function<void(VertexType&, const Fbx::Vertex&)> writer) const
        {
            outVertices.resize(m_vertices.size());

            for (int i = 0, count = (int)m_vertices.size(); i < count; ++i)
            {
                writer(outVertices[i], m_vertices[i]);
            }
        }

    private:
        std::vector<Triangle<Vertex>> m_triangles;
        std::vector<Vertex> m_vertices;
        std::vector<u32> m_indices;
        std::unordered_map<i32, u32> m_indicesByControlPointIndex;
        std::unordered_map<i32, JointBlend> m_blendByControlPointIndex;
    };
}

#endif // !FBXRESOURCE_H

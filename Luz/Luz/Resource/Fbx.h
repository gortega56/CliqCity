#pragma once
#ifndef FBXRESOURCE_H
#define FBXRESOURCE_H

#ifndef MESH_H
#include "Mesh.h"
#endif


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
            float Position[3];
            float Normal[3];
            float Tangent[3];
            float UV[2];

            Vertex() {}
            Vertex(float(&position)[3], float(&normal)[3], float(&uv)[2])
            {
                memcpy_s(Position, 12, position, 12);
                memcpy_s(Normal, 12, normal, 12);
                memcpy_s(UV, 8, uv, 8);
            }

            enum class Flag : u16
            {
                NONE        = 0,
                POSITION    = 1 << 0,
                NORMAL      = 1 << 1,
                UV          = 1 << 2,
                TANGENT     = 1 << 3,
                BLEND       = 1 << 4,
                COLOR       = 1 << 5
            };
        };

        struct Triangle
        {
            Vertex Vertices[3];
            Triangle() {}
            Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2)
            {
                Vertices[0] = v0;
                Vertices[1] = v1;
                Vertices[2] = v2;
            }
        };

        struct JointBlend
        {
            i32 Index;
            float Weight;

            JointBlend() {}
        };

        Vertex* GetVertex(u32 i);

        u32 AddVertex();
        u32 AddVertex(float(&position)[3], float(&normal)[3], float(&uv)[2]);
        u32 AddVertex(double(&position)[4], double(&normal)[4], double(&uv)[2]);

        void AddIndex(u32 i);

        void LUZ_API WriteVertexData(void* pData, size_t stride, size_t size, Vertex::Flag flags) const;

        template<class VertexType>
        void WriteVertices(std::vector<VertexType>& outVertices, std::function<void(VertexType&, const Fbx::Vertex&)> writer) const
        {
            outVertices.resize(m_vertices.size());

            for (int i = 0, count = (int)m_vertices.size(); i < count; ++i)
            {
                writer(outVertices[i], m_vertices[i]);
            }
        }

        const std::vector<u32>& GetIndices() const { return m_indices; }

    private:
        std::vector<Triangle> m_triangles;
        std::vector<Vertex> m_vertices;
        std::vector<u32> m_indices;
        std::unordered_map<i32, u32> m_indicesByControlPointIndex;
        std::unordered_map<i32, JointBlend> m_blendByControlPointIndex;
        Vertex::Flag m_vertexFlags;
    };

    inline Fbx::Vertex::Flag operator&(const Fbx::Vertex::Flag& lhs, const Fbx::Vertex::Flag& rhs)
    {
        return static_cast<Fbx::Vertex::Flag>(static_cast<u16>(lhs) & static_cast<u16>(rhs));
    }

    inline Fbx::Vertex::Flag operator|(const Fbx::Vertex::Flag& lhs, const Fbx::Vertex::Flag& rhs)
    {
        return static_cast<Fbx::Vertex::Flag>(static_cast<u16>(lhs) | static_cast<u16>(rhs));
    }

    inline Fbx::Vertex::Flag& operator|=(Fbx::Vertex::Flag& lhs, const Fbx::Vertex::Flag rhs)
    {
        return lhs = lhs | rhs;
    }

    inline bool operator==(const Fbx::Vertex::Flag& lhs, const u16& rhs)
    {
        return static_cast<u16>(lhs) == rhs;
    }

    inline bool operator!=(const Fbx::Vertex::Flag& lhs, const u16& rhs)
    {
        return !(lhs == rhs);
    }
}

#endif // !FBXRESOURCE_H

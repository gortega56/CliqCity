#pragma once
#ifndef MESH_H
#define MESH_H

#ifndef LUZEXPORT_H
#include "LuzExport.h"
#endif

#ifndef LINA_H
#include "lina.h"
#endif

#include <vector>
#include <map>

namespace Graphics
{
    __interface LUZ_API IMesh
    {
    public:
        virtual void* Vertices() const = 0;
        virtual void* Indices() const = 0;

        virtual u32 VertexStride() const = 0;
        virtual u32 IndexStride() const = 0;

        virtual u32 NumVertices() const = 0;
        virtual u32 NumIndices() const = 0;

        virtual u64 NumVertexBytes() const = 0;
        virtual u32 NumIndexBytes() const = 0;
    };

    template<class VertexType, class IndexType>
    class Mesh : public IMesh
    {
    public:
        Mesh(const VertexType* vertices, const u32 numVertices, const IndexType* indices, const u32 numIndices);
        Mesh();
        ~Mesh();

        Mesh(const Mesh<VertexType, IndexType>& o);
        Mesh(Mesh<VertexType, IndexType>&& o);

        Mesh& operator=(const Mesh<VertexType, IndexType>& o);
        Mesh& operator=(Mesh<VertexType, IndexType>&& o);

        void SetVertices(const VertexType* pVertices, const u32 numVertices);
        void SetIndices(const IndexType* pIndices, const u32 numIndices);

        void* Vertices() const override;
        void* Indices() const override;

        u32 VertexStride() const override;
        u32 IndexStride() const override;

        u32 NumVertices() const override;
        u32 NumIndices() const override;

        u64 NumVertexBytes() const override;
        u32 NumIndexBytes() const override;

        static void CreateTangents(VertexType* pVertices, const u32 numVertices, IndexType* pIndices, const u32 numIndices);

    private:
        std::vector<VertexType> m_vertices;
        std::vector<IndexType> m_indices;
    };

    template<class VertexType, class IndexType>
    Mesh<VertexType, IndexType>::Mesh(const VertexType* pVertices, const u32 numVertices, const IndexType* pIndices, const u32 numIndices)
        : m_vertices(pVertices, pVertices + numVertices)
        , m_indices(pIndices, pIndices + numIndices)
    {

    }


    template<class VertexType, class IndexType>
    Mesh<VertexType, IndexType>::Mesh() : Mesh(nullptr, 0, nullptr, 0)
    {

    }

    template<class VertexType, class IndexType>
    Mesh<VertexType, IndexType>::~Mesh()
    {
        m_vertices.clear();
        m_indices.clear();
    }

    template<class VertexType, class IndexType>
    Mesh<VertexType, IndexType>::Mesh(const Mesh<VertexType, IndexType>& o)
        : m_vertices(o.m_vertices)
        , m_indices(o.m_indices)
    {

    }

    template<class VertexType, class IndexType>
    Mesh<VertexType, IndexType>::Mesh(Mesh<VertexType, IndexType>&& o)
        : m_vertices(std::move(o.m_vertices))
        , m_indices(std::move(o.m_indices))
    {

    }

    template<class VertexType, class IndexType>
    Mesh<VertexType, IndexType>& Mesh<VertexType, IndexType>::operator=(const Mesh<VertexType, IndexType>& o)
    {
        m_vertices = other.m_vertices;
        m_indices = other.m_indices;
        return *this;
    }

    template<class VertexType, class IndexType>
    Mesh<VertexType, IndexType>& Mesh<VertexType, IndexType>::operator=(Mesh<VertexType, IndexType>&& o)
    {
        m_vertices = std::move(other.m_vertices);
        m_indices = std::move(other.m_indices);
        return *this;
    }

    template<class VertexType, class IndexType>
    inline void* Mesh<VertexType, IndexType>::Vertices() const 
    { 
        return (void*)m_vertices.data(); 
    }

    template<class VertexType, class IndexType>
    inline void* Mesh<VertexType, IndexType>::Indices() const
    { 
        return (void*)m_indices.data(); 
    }

    template<class VertexType, class IndexType>
    inline u32 Mesh<VertexType, IndexType>::VertexStride() const
    { 
        return sizeof(VertexType);
    }

    template<class VertexType, class IndexType>
    inline u32 Mesh<VertexType, IndexType>::IndexStride() const
    { 
        return sizeof(IndexType); 
    }

    template<class VertexType, class IndexType>
    inline u32 Mesh<VertexType, IndexType>::NumVertices() const
    { 
        return static_cast<u32>(m_vertices.size()); 
    }

    template<class VertexType, class IndexType>
    inline u32 Mesh<VertexType, IndexType>::NumIndices() const
    { 
        return static_cast<u32>(m_indices.size());
    }

    template<class VertexType, class IndexType>
    inline u64 Mesh<VertexType, IndexType>::NumVertexBytes() const
    { 
        return static_cast<u64>(sizeof(VertexType) * m_vertices.size());
    }

    template<class VertexType, class IndexType>
    inline u32 Mesh<VertexType, IndexType>::NumIndexBytes() const
    { 
        return static_cast<u32>(sizeof(IndexType) * m_indices.size());
    }

    template<class VertexType, class IndexType>
    inline void Mesh<VertexType, IndexType>::SetVertices(const VertexType* pVertices, const u32 numVertices)
    { 
        m_vertices.clear();
        m_vertices.resize(static_cast<size_t>(numVertices));
        memcpy_s(m_vertices.data(), sizeof(VertexType) * m_vertices.size(), pVertices, sizeof(VertexType) * numVertices);
    }

    template<class VertexType, class IndexType>
    inline void Mesh<VertexType, IndexType>::SetIndices(const IndexType* pIndices, const u32 numIndices)
    {
        m_indices.clear();
        m_indices.resize(static_cast<size_t>(numIndices));
        memcpy_s(m_indices.data(), sizeof(IndexType) * m_indices.size(), pIndices, sizeof(IndexType) * numIndices);
    }

    template<class VertexType, class IndexType>
    void Mesh<VertexType, IndexType>::CreateTangents(VertexType* pVertices, const u32 numVertices, IndexType* pIndices, const u32 numIndices)
    {
        //static_assert(Geometry::VertexTraits<VertexType>::Tangent);
        //static_assert(Geometry::VertexTraits<VertexType>::Position);
        //static_assert(Geometry::VertexTraits<VertexType>::Normal);
        //static_assert(Geometry::VertexTraits<VertexType>::UV);

        std::vector<float3> tangents(static_cast<size_t>(numVertices));
        std::vector<float3> bitangents(static_cast<size_t>(numVertices));
        memset(tangents.data(), 0, sizeof(float3) * tangents.size());
        memset(bitangents.data(), 0, sizeof(float3) * bitangents.size());

        for (u32 i = 3; i <= numIndices; i += 3)
        {
            IndexType i0 = pIndices[i - 3];
            IndexType i1 = pIndices[i - 2];
            IndexType i2 = pIndices[i - 1];

            VertexType& v0 = pVertices[i0];
            VertexType& v1 = pVertices[i1];
            VertexType& v2 = pVertices[i2];

            float x1 = v1.Position.x - v0.Position.x;
            float x2 = v2.Position.x - v0.Position.x;
            float y1 = v1.Position.y - v0.Position.y;
            float y2 = v2.Position.y - v0.Position.y;
            float z1 = v1.Position.z - v0.Position.z;
            float z2 = v2.Position.z - v0.Position.z;

            float s1 = v1.UV.x - v0.UV.x;
            float s2 = v2.UV.x - v0.UV.x;
            float t1 = v1.UV.y - v0.UV.y;
            float t2 = v2.UV.y - v0.UV.y;

            //float r = 1.0f / ((s1 * t2) - (s2 * t1));

            //float3 tangent = { (((t2 * x1) - (t1 * x2)) * r), (((t2 * y1) - (t1 * y2)) * r), (((t2 * z1) - (t1 * z2)) * r) };
            //float3 bitangent = { (((s2 * x1) - (s1 * x2)) * r), (((s2 * y1) - (s1 * y2)) * r), (((s2 * z1) - (s1 * z2)) * r) };

            float3 tangent = 
            { 
                t2 * x1 - t1 * x2, 
                t2 * y1 - t1 * y2, 
                t2 * z1 - t1 * z2 
            };

            float3 bitangent = 
            { 
                -s2 * x1 + s1 * x2,
                -s2 * y1 + s1 * y2,
                -s2 * z1 + s1 * z2
            };

            tangent.normalize();
            bitangent.normalize();

            tangents[i0] += tangent;
            tangents[i1] += tangent;
            tangents[i2] += tangent;

            bitangents[i0] += bitangent;
            bitangents[i1] += bitangent;
            bitangents[i2] += bitangent;
        }

        for (u32 i = 0; i < numIndices; i++)
        {
            IndexType index = pIndices[i];
            VertexType& vertex = pVertices[index];
            float3 tangent = tangents[index];
            float3 bitangent = bitangents[index];
            float3 normal = vertex.Normal;

            tangent -= normal * lina::dot(tangent, normal);
            tangent.normalize();

            float w = (lina::dot(lina::cross(normal, tangent), bitangent) > 0.0f) ? 1.0f : -1.0f;
            vertex.Tangent = float4(tangent, w);
        }
    }
}



#endif
#pragma once
#ifndef MESH_H
#define MESH_H

#ifndef LUZEXPORT_H
#include "LuzExport.h"
#endif

#ifndef GMATH_H
#include "gmath.h"
#endif

#include <vector>
#include <map>

__interface LUZ_API IMesh
{
public:
    virtual void* VertexData() const = 0;
    virtual void* IndexData() const = 0;

    virtual size_t VertexStride() const = 0;
    virtual size_t IndexStride() const = 0;

    virtual u32 NumVertices() const = 0;
    virtual u32 NumIndices() const = 0;

    virtual size_t VertexDataSize() const = 0;
    virtual size_t IndexDataSize() const = 0;
};



template<class Vertex, class Index>
class Mesh : public IMesh
{
    typedef gmath::float4 float4;
    typedef gmath::float3 float3;
public:
    Mesh()
    {

    }

    ~Mesh()
    {
        m_vertices.clear();
        m_indices.clear();
    }

    Mesh(const std::vector<Vertex>& vertices, const std::vector<Index>& indices) : m_vertices(vertices), m_indices(indices)
    {

    }

    Mesh(const Mesh<Vertex, Index>& other) : Mesh(other.m_vertices, other.m_indices)
    {

    }

    Mesh(Mesh<Vertex, Index>&& other) : m_vertices(std::move(other.m_vertices), std::move(other.m_indices))
    {

    }

    Mesh& operator=(const Mesh<Vertex, Index>& other)
    {
        m_vertices = other.m_vertices;
        m_indices = other.m_indices;
        return *this;
    }

    Mesh& operator=(Mesh<Vertex, Index>&& other)
    {
        m_vertices = std::move(other.m_vertices);
        m_indices = std::move(other.m_indices);
        return *this;
    }

    inline void* VertexData() const override { return (void*)m_vertices.data(); }
    inline void* IndexData() const override { return (void*)m_indices.data(); }

    inline size_t VertexStride() const override { return sizeof(Vertex); }
    inline size_t IndexStride() const override { return sizeof(Index); }

    inline u32 NumVertices() const override { return (u32)m_vertices.size(); }
    inline u32 NumIndices() const override { return (u32)m_indices.size(); }

    inline size_t VertexDataSize() const override { return sizeof(Vertex) * m_vertices.size(); }
    inline size_t IndexDataSize() const override { return sizeof(Index) * m_indices.size(); }

    inline std::vector<Vertex>& Vertices() { return m_vertices; }
    inline std::vector<Index>& Indices() { return m_indices; }

    inline void SetVertices(const std::vector<Vertex>& vertices) { m_vertices = vertices; }
    inline void SetIndices(const std::vector<Index>& indices) { m_indices = indices; }

    inline void SetVertices(std::vector<Vertex>&& vertices) { m_vertices = std::move(vertices); }
    inline void SetIndices(std::vector<Index>&& indices) { m_indices = std::move(indices); }

    void GenerateTangents()
    {
        std::vector<float3> tangents(m_vertices.size());
        std::vector<float3> bitangents(m_vertices.size());
        memset(tangents.data(), 0, sizeof(float3) * tangents.size());
        memset(bitangents.data(), 0, sizeof(float3) * bitangents.size());

        for (int i = 3, numIndices = (int)m_indices.size(); i <= numIndices; i += 3)
        {
            Index i0 = m_indices[i - 3];
            Index i1 = m_indices[i - 2];
            Index i2 = m_indices[i - 1];

            Vertex& v0 = m_vertices[i0];
            Vertex& v1 = m_vertices[i1];
            Vertex& v2 = m_vertices[i2];

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

            float r = 1.0f / ((s1 * t2) - (s2 * t1));

            float3 tangent = { (((t2 * x1) - (t1 * x2)) * r), (((t2 * y1) - (t1 * y2)) * r), (((t2 * z1) - (t1 * z2)) * r) };
            float3 bitangent = { (((s2 * x1) - (s1 * x2)) * r), (((s2 * y1) - (s1 * y2)) * r), (((s2 * z1) - (s1 * z2)) * r) };

            tangents[i0] += tangent;
            tangents[i1] += tangent;
            tangents[i2] += tangent;

            bitangents[i0] += bitangent;
            bitangents[i1] += bitangent;
            bitangents[i2] += bitangent;
        }

        for (int i = 0, numIndices = (int)m_indices.size(); i < numIndices; i++)
        {
            Index index = m_indices[i];
            Vertex& vertex = m_vertices[index];
            float3& tangent = tangents[index];
            float3& bitangent = bitangents[index];
            float3& normal = vertex.Normal;

            vertex.Tangent = gmath::normalize((tangent - normal * gmath::dot(normal, tangent)));
            vertex.Tangent.w = (gmath::dot(gmath::cross(normal, tangent), bitangent) > 0.0f) ? 1.0f : -1.0f;
        }
    }

private:
    std::vector<Vertex> m_vertices;
    std::vector<Index> m_indices;
};

#endif
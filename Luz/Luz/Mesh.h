#pragma once
#ifndef MESH_H
#define MESH_H

#ifndef LUZEXPORT_H
#include "LuzExport.h"
#endif

#include <vector>

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

private:
    std::vector<Vertex> m_vertices;
    std::vector<Index> m_indices;
};

#endif
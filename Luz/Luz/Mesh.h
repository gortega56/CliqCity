#pragma once
#ifndef MESH_H
#define MESH_H

#include <vector>

__interface IMesh
{
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

    Mesh(std::vector<Vertex>& vertices, std::vector<Index>& indices) : m_vertices(vertices), m_indices(indices)
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
        return Mesh(std::move(other));
    }

    inline void CopyVertices(std::vector<Vertex>& vertices)
    {
        m_vertices = vertices;
    }

    inline void MoveVertices(std::vector<Vertex>& vertices)
    {
        m_vertices = std::move(vertices);
    }

    inline void CopyIndices(std::vector<Index>& indices)
    {
        m_indices = indices;
    }

    inline void MoveIndices(std::vector<Index>& indices)
    {
        m_indices = std::move(indices);
    }

    void* VertexData() const override { return (void*)m_vertices.data(); }
    void* IndexData() const override { return (void*)m_indices.data(); }

    size_t VertexStride() const override { return sizeof(Vertex); }
    size_t IndexStride() const override { return sizeof(Index); }

    u32 NumVertices() const override { return (u32)m_vertices.size(); }
    u32 NumIndices() const override { return (u32)m_indices.size(); }

    size_t VertexDataSize() const override { return sizeof(Vertex) * m_vertices.size(); }
    size_t IndexDataSize() const override { return sizeof(Index) * m_indices.size(); }

private:
    std::vector<Vertex> m_vertices;
    std::vector<Index> m_indices;
};

#endif
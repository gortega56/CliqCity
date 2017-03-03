#pragma once
#ifndef MESH_H
#define MESH_H

#include <vector>

__interface IMesh
{
    virtual void* VertexData() = 0;
    virtual void* IndexData() = 0;

    virtual size_t VertexStride() = 0;
    virtual size_t IndexStride() = 0;

    virtual u32 NumVertices() = 0;
    virtual u32 NumIndices() = 0;

    virtual size_t VertexDataSize() = 0;
    virtual size_t IndexDataSize() = 0;
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

    Mesh(const Mesh<Vertex, Index>&& other) : m_vertices(std::move(other.m_vertices), std::move(other.m_indices))
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

    void* VertexData() override { return m_vertices.data(); }
    void* IndexData() override { return m_indices.data(); }

    size_t VertexStride() override { return sizeof(Vertex); }
    size_t IndexStride() override { return sizeof(Index); }

    u32 NumVertices() override { return (u32)m_vertices.size(); }
    u32 NumIndices() override { return (u32)m_indices.size(); }

    size_t VertexDataSize() override { return sizeof(Vertex) * m_vertices.size(); }
    size_t IndexDataSize() override { return sizeof(Index) * m_indices.size(); }

private:
    std::vector<Vertex> m_vertices;
    std::vector<Index> m_indices;
};

#endif
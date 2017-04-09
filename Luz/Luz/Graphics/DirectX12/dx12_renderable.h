#pragma once
#ifndef DX12RENDERABLE_H
#define DX12RENDERABLE_H

#ifndef GPURESOURCE_H
#include "GpuResource.h"
#endif

__interface IMesh;

namespace dx12
{
    class Renderer;

    class Renderable
    {
    public:
        Renderable();
        ~Renderable();

        D3D12_VERTEX_BUFFER_VIEW const* VertexBufferView() const;
        D3D12_INDEX_BUFFER_VIEW const* IndexBufferView() const;
        
        D3D12_PRIMITIVE_TOPOLOGY Topology() const { return m_topology; }
        u32 NumVertices() const { return m_vertexBuffer.NumElements(); }
        u32 NumIndices() const { return m_indexBuffer.NumElements(); }

        bool LoadMesh(Renderer* pRenderer, IMesh* pMesh);

    private:
        GpuBuffer m_vertexBuffer;
        GpuBuffer m_indexBuffer;

        D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
        D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

        D3D_PRIMITIVE_TOPOLOGY m_topology;
    };
}

#endif

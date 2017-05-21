#pragma once
#ifndef DX12RENDERABLE_H
#define DX12RENDERABLE_H

#ifndef DX12_GPURESOURCE_H
#include "GpuResource.h"
#endif

__interface IMesh;

namespace Dx12
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

        bool LoadMesh(std::shared_ptr<const Renderer> pRenderer, std::shared_ptr<const IMesh> pMesh);
        bool LoadMesh(std::shared_ptr<const Renderer> pRenderer, IMesh const* pMesh);

        void Prepare(GraphicsCommandContext* pCtx);
        void DrawIndexedInstanced(GraphicsCommandContext* pCtx, u32 instanceCount = 1, u32 startIndex = 0, i32 baseVertex = 0, u32 startInstance = 0);

    private:
        GpuBuffer m_vertexBuffer;
        GpuBuffer m_indexBuffer;

        D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
        D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

        D3D_PRIMITIVE_TOPOLOGY m_topology;
    };
}

#endif

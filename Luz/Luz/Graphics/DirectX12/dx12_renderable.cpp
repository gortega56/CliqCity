#include "stdafx.h"
#include "dx12_renderable.h"
#include "dx12_renderer.h"
#include "Mesh.h"
#include "CommandContext.h"

using namespace Dx12;

Renderable::Renderable() : 
    m_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST), 
    m_vertexBuffer(D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER),
    m_indexBuffer(D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_INDEX_BUFFER)
{
    ZeroMemory(&m_vertexBufferView, sizeof(D3D12_VERTEX_BUFFER_VIEW));
    ZeroMemory(&m_indexBufferView, sizeof(D3D12_INDEX_BUFFER_VIEW));
}

Renderable::~Renderable()
{
    
}

D3D12_VERTEX_BUFFER_VIEW const* Renderable::VertexBufferView() const
{
    return &m_vertexBufferView;
}

D3D12_INDEX_BUFFER_VIEW const* Renderable::IndexBufferView() const
{
    return &m_indexBufferView;
}

bool Renderable::LoadMesh(std::shared_ptr<const Renderer> pRenderer, std::shared_ptr<const IMesh> pMesh)
{
    return LoadMesh(pRenderer, pMesh.get());
}

bool Renderable::LoadMesh(std::shared_ptr<const Renderer> pRenderer, IMesh const* pMesh)
{
    auto pQueue = pRenderer->GetCommandQueue();

    m_vertexBuffer.SetBufferSize((u64)pMesh->VertexDataSize());
    m_vertexBuffer.SetElementSize((u32)pMesh->VertexStride());
    m_vertexBuffer.SetNumElements(pMesh->NumVertices());
    if (!m_vertexBuffer.Initialize(pRenderer, pMesh->VertexData()))
    {
        return false;
    }

    m_indexBuffer.SetBufferSize((u64)pMesh->IndexDataSize());
    m_indexBuffer.SetElementSize((u32)pMesh->IndexStride());
    m_indexBuffer.SetNumElements(pMesh->NumIndices());
    if (!m_indexBuffer.Initialize(pRenderer, pMesh->IndexData()))
    {
        return false;
    }

    m_vertexBufferView = m_vertexBuffer.VertexBufferView();
    m_indexBufferView = m_indexBuffer.IndexBufferView();

    return true;
}

void Renderable::Prepare(GraphicsCommandContext* pCtx)
{
    pCtx->CommandList()->IASetPrimitiveTopology(m_topology);
    pCtx->CommandList()->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    pCtx->CommandList()->IASetIndexBuffer(&m_indexBufferView);
}

void Renderable::DrawIndexedInstanced(GraphicsCommandContext* pCtx, u32 instanceCount, u32 startIndex, i32 baseVertex, u32 startInstance)
{
    pCtx->CommandList()->DrawIndexedInstanced(NumIndices(), instanceCount, startIndex, baseVertex, startInstance);
}
#include "stdafx.h"
#include "dx12_renderable.h"
#include "dx12_renderer.h"
#include "Mesh.h"
#include "CommandContext.h"

using namespace dx12;

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

bool Renderable::LoadMesh(Renderer* pRenderer, IMesh* pMesh)
{
    auto pQueue = pRenderer->GetCommandQueue();
    auto pCtx = pRenderer->GetFrameContext();

    if (!m_vertexBuffer.Initialize(pQueue, pCtx, pMesh->VertexDataSize(), (u32)pMesh->VertexStride(), pMesh->NumVertices(), pMesh->VertexData()))
    {
        return false;
    }

    if (!m_indexBuffer.Initialize(pQueue, pCtx, pMesh->IndexDataSize(), (u32)pMesh->IndexStride(), pMesh->NumIndices(), pMesh->IndexData()))
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
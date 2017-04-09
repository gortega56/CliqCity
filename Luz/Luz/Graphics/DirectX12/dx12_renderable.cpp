#include "stdafx.h"
#include "dx12_renderable.h"
#include "dx12_renderer.h"
#include "Mesh.h"

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
    auto pCtx = pRenderer->GetGraphicsContext();

    if (!m_vertexBuffer.Initialize(pQueue, pCtx, pMesh->VertexDataSize(), pMesh->VertexStride(), pMesh->NumVertices(), pMesh->VertexData()))
    {
        return false;
    }

    if (!m_indexBuffer.Initialize(pQueue, pCtx, pMesh->IndexDataSize(), pMesh->IndexStride(), pMesh->NumIndices(), pMesh->IndexData()))
    {
        return false;
    }

    m_vertexBufferView = m_vertexBuffer.VertexBufferView();
    m_indexBufferView = m_indexBuffer.IndexBufferView();

    return true;
}
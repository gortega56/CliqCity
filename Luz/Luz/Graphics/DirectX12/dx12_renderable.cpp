#include "stdafx.h"
#include "dx12_renderable.h"
#include "dx12_renderer.h"
#include "Mesh.h"

using namespace dx12;

Renderable::Renderable() : m_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{
    ZeroMemory(&m_vertexBufferView, sizeof(D3D12_VERTEX_BUFFER_VIEW));
    ZeroMemory(&m_indexBufferView, sizeof(D3D12_INDEX_BUFFER_VIEW));
}

Renderable::~Renderable()
{
    
}

D3D12_VERTEX_BUFFER_VIEW* Renderable::VertexBufferView()
{
    return &m_vertexBufferView;
}

D3D12_INDEX_BUFFER_VIEW* Renderable::IndexBufferView()
{
    return &m_indexBufferView;
}

bool Renderable::LoadMesh(Renderer* pRenderer, IMesh* pMesh)
{
    auto pDevice = pRenderer->m_device.DX();

    m_vertexBuffer.SetBufferSize(pMesh->VertexDataSize());
    m_vertexBuffer.SetElementSize(pMesh->VertexStride());

    if (!m_vertexBuffer.Initialize(pRenderer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, pMesh->VertexData()))
    {
        return false;
    }

    m_indexBuffer.SetBufferSize(pMesh->IndexDataSize());
    m_indexBuffer.SetElementSize(pMesh->IndexStride());

    if (!m_indexBuffer.Initialize(pRenderer, D3D12_RESOURCE_STATE_INDEX_BUFFER, pMesh->IndexData()))
    {
        return false;
    }

    m_vertexBufferView = m_vertexBuffer.VertexBufferView();
    m_indexBufferView = m_indexBuffer.IndexBufferView();

    return true;
}
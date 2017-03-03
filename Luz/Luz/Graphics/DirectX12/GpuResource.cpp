#include "stdafx.h"
#include "GpuResource.h"
#include "dx12_renderer.h"
#include "DescriptorHeap.h"

using namespace dx12;

#pragma region GpuResource

GpuResource::GpuResource(u32 numResources) : m_resources(numResources)
{

}

GpuResource::~GpuResource()
{
    Release();
}

void GpuResource::Release() 
{ 
    for (ID3D12Resource* resource : m_resources)
    {
        SAFE_RELEASE(resource); 
    }
}

#pragma endregion

#pragma region GpuBuffer

GpuBuffer::GpuBuffer(u32 numResources, u64 bufferSize, u32 elementSize) : GpuResource(numResources), m_bufferSize(bufferSize), m_elementSize(elementSize), m_numElements((u32)bufferSize / elementSize)
{

}

GpuBuffer::~GpuBuffer()
{

}

bool GpuBuffer::Initialize(Renderer* pRenderer, D3D12_RESOURCE_STATES state, void* data)
{
    auto pDevice = pRenderer->m_device.DX();

    if (!CreateDestinationBufferResource(pDevice, ResourceAddress(), m_bufferSize))
    {
        return false;
    }

    if (data)
    {
        ID3D12Resource* uploadHeap;
        if (!CreateUploadBufferResource(pDevice, &uploadHeap, m_bufferSize))
        {
            return false;
        }

        UpdateBufferResource(pRenderer->m_graphicsCommandCtx.CommandList(), Resource(), uploadHeap, state, data, (LONG_PTR)m_bufferSize, (LONG_PTR)m_bufferSize);

        pRenderer->m_graphicsCommandCtx.IncrementFenceValue();
        pRenderer->ExecuteGraphicsCommandContext();
    }

    return true;
}

D3D12_VERTEX_BUFFER_VIEW GpuBuffer::VertexBufferView()
{
    D3D12_VERTEX_BUFFER_VIEW vbv;
    vbv.BufferLocation = Resource()->GetGPUVirtualAddress();
    vbv.StrideInBytes = m_elementSize;
    vbv.SizeInBytes = (UINT)m_bufferSize;
    return vbv;
}

D3D12_INDEX_BUFFER_VIEW GpuBuffer::IndexBufferView()
{
    D3D12_INDEX_BUFFER_VIEW ibv;
    ibv.BufferLocation = Resource()->GetGPUVirtualAddress();
    ibv.Format = IndexFormat(m_elementSize);
    ibv.SizeInBytes = (UINT)m_bufferSize;
    return ibv;
}

#pragma endregion

#pragma region DynamicBuffer

DynamicBuffer::DynamicBuffer(u32 numResources, u64 bufferSize, u32 elementSize) : GpuBuffer(numResources, bufferSize, elementSize), m_gpuAddress(nullptr)
{

}

DynamicBuffer::~DynamicBuffer()
{

}

bool DynamicBuffer::Initialize(Renderer* pRenderer, void* data)
{
    auto pDevice = pRenderer->m_device.DX();

    for (auto& res : m_resources)
    {
        if (!CreateUploadBufferResource(pDevice, &res, m_bufferSize))
        {
            return false;
        }

        if (data)
        {
            Map(data);
        }

        Unmap();
    }

    

    return true;
}


void DynamicBuffer::ConstantBufferView(Renderer* pRenderer, DescriptorHeap* pCBVHeap, int i)
{
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = m_resources[i]->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = (m_bufferSize + 255) & ~255; // CB size is required to be 256-byte aligned.

    pRenderer->m_device.DX()->CreateConstantBufferView(&cbvDesc, pCBVHeap->Native()->GetCPUDescriptorHandleForHeapStart());
}

bool DynamicBuffer::Map(void* data)
{
    CD3DX12_RANGE readRange(0, 0);
    HRESULT hr = Resource()->Map(0, &readRange, reinterpret_cast<void**>(&m_gpuAddress));
    if (FAILED(hr))
    {
        return false;
    }

    memcpy(m_gpuAddress, data, (size_t)m_bufferSize);

    return true;
}

void DynamicBuffer::Unmap()
{
    CD3DX12_RANGE readRange(0, 0);
    Resource()->Unmap(0, &readRange);
}

#pragma endregion


PixelBuffer::PixelBuffer(u32 numResources, u32 width, u32 height) : GpuResource(numResources), m_width(numResources, width), m_height(numResources, height)
{

}

PixelBuffer::~PixelBuffer()
{

}

ColorBuffer::ColorBuffer(u32 numResources, u32 width, u32 height, float clearColor[4]) : PixelBuffer(numResources, width, height)
{
    SetColor(clearColor);
}

ColorBuffer::~ColorBuffer()
{

}

void ColorBuffer::SetColor(float color[4])
{
    for (auto c : m_colors)
    {
        c[0] = color[0];
        c[1] = color[1];
        c[2] = color[2];
        c[3] = color[3];
    }
}

void ColorBuffer::SetColor(float color[4], int i)
{
    m_colors[i][0] = color[0];
    m_colors[i][1] = color[1];
    m_colors[i][2] = color[2];
    m_colors[i][3] = color[3];
}

DepthBuffer::DepthBuffer(u32 width, u32 height) : PixelBuffer(4, width, height)
{

}

DepthBuffer::~DepthBuffer()
{

}

bool DepthBuffer::Initialize(Renderer* pRenderer, DescriptorHeap* pDescriptorHeap)
{
    ID3D12Device* pDevice = pRenderer->m_device.DX();

    if (!CreateDepthStencilResource(pDevice, &m_resources[0], m_width[0], m_height[0], 1, 0, 1, 0))
    {
        return false;
    }

    CreateDepthStencilViews(pDevice, pDescriptorHeap->Native(), &m_resources[0], 1);

    return true;
}

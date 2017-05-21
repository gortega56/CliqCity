#include "stdafx.h"
#include "GpuResource.h"
#include "CommandContext.h"
#include "Device.h"
#include "DescriptorHeap.h"
#include "CommandQueue.h"
#include "dx12_renderer.h"

using namespace Dx12;

#pragma region GpuResource

GpuResource::GpuResource(ID3D12Resource* pResource, D3D12_RESOURCE_STATES resourceState) : m_resource(pResource), m_resourceState(resourceState)
{

}


GpuResource::GpuResource(D3D12_RESOURCE_STATES resourceState) : GpuResource(nullptr, resourceState)
{

}

GpuResource::GpuResource(GpuResource&& other)
{
    m_resource = other.m_resource;
    m_resourceState = other.m_resourceState;
    
    other.m_resource = nullptr;
    other.m_resourceState = (D3D12_RESOURCE_STATES)-1;
}

GpuResource::~GpuResource()
{
    SAFE_RELEASE(m_resource);
}

#pragma endregion

#pragma region GpuBuffer

GpuBuffer::GpuBuffer() : m_bufferSize(0), m_elementSize(0), m_numElements(0)
{

}

GpuBuffer::~GpuBuffer()
{

}

bool GpuBuffer::Initialize(std::shared_ptr<const Renderer> pRenderer, void* data /*= nullptr*/)
{
    auto pDevice = pRenderer->GetDevice()->DX();

    if (!CreateDestinationBuffer(pDevice, &m_resource, m_bufferSize))
    {
        return false;
    }

    if (data)
    {
        ID3D12Resource* uploadBuffer;
        if (!CreateUploadBuffer(pDevice, &uploadBuffer, m_bufferSize))
        {
            return false;
        }

        auto& pCtx = pRenderer->GetContext();
        auto pCommandList = pCtx->CommandList();
        UpdateBufferResource(pCommandList, m_resource, uploadBuffer, data, (LONG_PTR)m_bufferSize, (LONG_PTR)m_bufferSize);
        TransitionResource(pCommandList, m_resource, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST, m_resourceState);

        auto& pCommandQueue = pRenderer->GetCommandQueue();
        pCommandQueue->Execute(pCtx, true);
        SAFE_RELEASE(uploadBuffer);
    }

    return true;
}

bool GpuBuffer::Initialize(std::shared_ptr<const Renderer> pRenderer, u64 bufferSize, u32 elementSize, u32 numElements, void* data /*= nullptr*/)
{
    m_bufferSize = bufferSize;
    m_elementSize = elementSize;
    m_numElements = numElements;
    return Initialize(pRenderer, data);
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

UploadBuffer::UploadBuffer() : GpuBuffer(D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON)
{

}

UploadBuffer::~UploadBuffer()
{

}

bool UploadBuffer::Initialize(std::shared_ptr<const Renderer> pRenderer, void* data /*= nullptr*/)
{
    auto pDevice = pRenderer->GetDevice()->DX();

    if (!CreateUploadBuffer(pDevice, &m_resource, m_bufferSize))
    {
        return false;
    }

    if (data)
    {
        Map(data);
        Unmap();
    }

    return true;
}


//void UploadBuffer::ConstantBufferView(Renderer* pRenderer, DescriptorHeap* pCBVHeap, int i)
//{
//    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
//    cbvDesc.BufferLocation = m_resources[i]->GetGPUVirtualAddress();
//    cbvDesc.SizeInBytes = (m_bufferSize + 255) & ~255; // CB size is required to be 256-byte aligned.
//
//    pRenderer->m_device.DX()->CreateConstantBufferView(&cbvDesc, pCBVHeap->Native()->GetCPUDescriptorHandleForHeapStart());
//}

bool UploadBuffer::Map(void* data)
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

void UploadBuffer::Unmap()
{
    CD3DX12_RANGE readRange(0, 0);
    Resource()->Unmap(0, &readRange);
}

#pragma endregion

PixelBuffer::PixelBuffer(ID3D12Resource* pResource, 
    D3D12_RESOURCE_STATES resourceState, 
    DXGI_FORMAT format, u32 width, u32 height) : 
    GpuResource(pResource, resourceState), 
    m_width(width), 
    m_height(height),
    m_mipLevels(1),
    m_arraySize(1),
    m_format(format)
{

}

PixelBuffer::PixelBuffer(D3D12_RESOURCE_STATES resourceState, DXGI_FORMAT format, u32 width, u32 height) : 
    PixelBuffer(nullptr, resourceState, format, width, height)
{

}

PixelBuffer::~PixelBuffer()
{

}

PixelBuffer::PixelBuffer(PixelBuffer&& other)
{
    m_resource = other.m_resource;
    m_resourceState = other.m_resourceState;
    m_format = other.m_format;
    m_width = other.m_width;
    m_height = other.m_height;

    other.m_resource = nullptr;
    other.m_resourceState = (D3D12_RESOURCE_STATES)-1;
    other.m_format = DXGI_FORMAT_UNKNOWN;
    other.m_width = 0;
    other.m_height = 0;
}

bool PixelBuffer::InitializeTexture2D(std::shared_ptr<const Renderer> pRenderer, void* data /*= nullptr*/)
{
    auto pDevice = pRenderer->GetDevice()->DX();
    if (!CreateDestinationTexture2D(pDevice, &m_resource, m_width, m_height, m_mipLevels, m_format))
    {
        return false;
    }

    if (data)
    {
        ID3D12Resource* uploadBuffer;
        if (!CreateUploadTexture2D(pDevice, &uploadBuffer, m_width, m_height, m_mipLevels, m_format))
        {
            return false;
        }

        auto& pCtx = pRenderer->GetContext();
        auto pCommandList = pCtx->CommandList();
        int bytesPerRow = GetDXGIFormatBitsPerPixel(m_format);
        UpdateBufferResource(pCommandList, m_resource, uploadBuffer, data, (LONG_PTR)bytesPerRow, (LONG_PTR)(bytesPerRow * m_height));
        TransitionResource(pCommandList, m_resource, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST, m_resourceState);

        auto& pCommandQueue = pRenderer->GetCommandQueue();
        pCommandQueue->Execute(pCtx, true);
        SAFE_RELEASE(uploadBuffer);
    }

    return true;
}

bool PixelBuffer::InitializeTexture2D(std::shared_ptr<const Renderer> pRenderer, 
    const u32 width, 
    const u32 height, 
    const DXGI_FORMAT format,
    const u16 mipLevels, 
    void* data /*= nullptr*/)
{
    m_dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    m_width = width;
    m_height = height;
    m_format = format;
    m_arraySize = 1;
    m_mipLevels = mipLevels;
    m_sampleCount = 1;
    m_sampleQuality = 0;
    m_viewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    return InitializeTexture2D(pRenderer, data);
}

ColorBuffer::ColorBuffer(ID3D12Resource* pResource,
    D3D12_RESOURCE_STATES resourceState,
    DXGI_FORMAT format, u32 width, u32 height, 
    const float* color) : 
    PixelBuffer(pResource, resourceState, format, width, height)
{
    SetColor(color);
}
ColorBuffer::ColorBuffer(D3D12_RESOURCE_STATES resourceState,
    DXGI_FORMAT format, u32 width, u32 height,
    const float* color) : 
    ColorBuffer(nullptr, resourceState, format, width, height)
{

}

ColorBuffer::~ColorBuffer()
{
    
}

ColorBuffer::ColorBuffer(ColorBuffer&& other)
{
    m_resource = other.m_resource;
    m_resourceState = other.m_resourceState;
    m_format = other.m_format;
    m_width = other.m_width;
    m_height = other.m_height;
    m_color[0] = other.m_color[0];
    m_color[1] = other.m_color[1];
    m_color[2] = other.m_color[2];
    m_color[3] = other.m_color[3];

    other.m_resource = nullptr;
    other.m_resourceState = (D3D12_RESOURCE_STATES)-1;
    other.m_format = DXGI_FORMAT_UNKNOWN;
    other.m_width = 0;
    other.m_height = 0;
    other.m_color[0] = 1;
    other.m_color[1] = 0;
    other.m_color[2] = 0;
    other.m_color[3] = 0;
}

void ColorBuffer::SetColor(const float* color)
{
    if (color)
    {
        SetColor(color[0], color[1], color[2], color[3]);
    }
}


void ColorBuffer::SetColor(float r, float g, float b, float a)
{
    m_color[0] = r;
    m_color[1] = g;
    m_color[2] = b;
    m_color[3] = a;
}

DepthBuffer::DepthBuffer(u32 width, u32 height) : PixelBuffer(nullptr)
{

}

DepthBuffer::~DepthBuffer()
{

}

bool DepthBuffer::Initialize(std::shared_ptr<const Device> pDevice, std::shared_ptr<DescriptorHeap> pHeap)
{
    if (!CreateDepthStencilResource(pDevice->DX(), &m_resource, m_width, m_height, 1, 0, 1, 0))
    {
        return false;
    }

    CreateDepthStencilViews(pDevice->DX(), pHeap->Native(), &m_resource, 1);

    return true;
}

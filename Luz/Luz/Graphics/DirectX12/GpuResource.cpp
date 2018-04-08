#include "stdafx.h"
#include "GpuResource.h"
#include "dx12_internal.h"
#include "CommandContext.h"
#include "Device.h"
#include "CommandQueue.h"
#include "Dx12Graphics.h"
#include "Resource\Texture.h"
#include "TextureImpl.h"

using namespace Dx12;

#pragma region GpuResource

GpuResource::GpuResource(Microsoft::WRL::ComPtr<ID3D12Resource> pResource, D3D12_RESOURCE_STATES resourceState) : m_resource(pResource), m_resourceState(resourceState)
{

}


GpuResource::GpuResource(D3D12_RESOURCE_STATES resourceState) : GpuResource(nullptr, resourceState)
{

}

GpuResource::~GpuResource()
{

}

GpuResource::GpuResource(GpuResource&& other) : m_resource(std::move(other.m_resource)), m_resourceState(other.m_resourceState)
{
    other.m_resource = nullptr;
    other.m_resourceState = (D3D12_RESOURCE_STATES)-1;
}

GpuResource& GpuResource::operator=(GpuResource&& other)
{
    m_resource = std::move(other.m_resource);
    m_resourceState = other.m_resourceState;

    other.m_resource = nullptr;
    other.m_resourceState = (D3D12_RESOURCE_STATES)-1;

    return *this;
}

#pragma endregion

#pragma region GpuBuffer

GpuBuffer::GpuBuffer() : m_bufferSize(0), m_elementSize(0), m_numElements(0)
{

}

GpuBuffer::~GpuBuffer()
{

}

bool GpuBuffer::Initialize(void* data /*= nullptr*/)
{
    auto pDevice = Device::SharedInstance()->DX();

    if (!CreateDestinationBuffer(pDevice, m_resource.ReleaseAndGetAddressOf(), m_bufferSize))
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

        auto pCtx = GraphicsCommandContext::Create();
        auto pCommandList = pCtx->CommandList();
        UpdateBufferResource(pCommandList, m_resource.Get(), uploadBuffer, data, (LONG_PTR)m_bufferSize, (LONG_PTR)m_bufferSize);
        TransitionResource(pCommandList, m_resource.Get(), D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST, m_resourceState);
        pCtx->Execute(true);
        SAFE_RELEASE(uploadBuffer);
    }

    return true;
}

bool GpuBuffer::Initialize(u64 bufferSize, u32 elementSize, u32 numElements, void* data /*= nullptr*/)
{
    m_bufferSize = bufferSize;
    m_elementSize = elementSize;
    m_numElements = numElements;
    return Initialize(data);
}

D3D12_VERTEX_BUFFER_VIEW GpuBuffer::VertexBufferView()
{
    D3D12_VERTEX_BUFFER_VIEW vbv;
    vbv.BufferLocation = m_resource->GetGPUVirtualAddress();
    vbv.StrideInBytes = m_elementSize;
    vbv.SizeInBytes = (UINT)m_bufferSize;
    return vbv;
}

D3D12_INDEX_BUFFER_VIEW GpuBuffer::IndexBufferView()
{
    D3D12_INDEX_BUFFER_VIEW ibv;
    ibv.BufferLocation = m_resource->GetGPUVirtualAddress();
    ibv.Format = IndexFormat(m_elementSize);
    ibv.SizeInBytes = (UINT)m_bufferSize;
    return ibv;
}

GpuBuffer::GpuBuffer(GpuBuffer&& other) : GpuResource(std::move(other))
{
    m_bufferSize = other.m_bufferSize;
    m_elementSize = other.m_elementSize;
    m_numElements = other.m_numElements;

    other.m_bufferSize = -1;
    other.m_elementSize = -1;
    other.m_numElements = -1;
}

GpuBuffer& GpuBuffer::operator=(GpuBuffer&& other)
{
    GpuResource::operator=(std::move(other));

    m_bufferSize = other.m_bufferSize;
    m_elementSize = other.m_elementSize;
    m_numElements = other.m_numElements;

    other.m_bufferSize = -1;
    other.m_elementSize = -1;
    other.m_numElements = -1;

    return *this;
}

#pragma endregion

#pragma region DynamicBuffer

UploadBuffer::UploadBuffer() : GpuBuffer(D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON)
{

}

UploadBuffer::~UploadBuffer()
{

}

bool UploadBuffer::Initialize(void* data /*= nullptr*/)
{
    auto pDevice = Device::SharedInstance()->DX();

    if (!CreateUploadBuffer(pDevice, m_resource.ReleaseAndGetAddressOf(), m_bufferSize))
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

bool UploadBuffer::Map(void* data) const
{
    u32* gpuAddress = nullptr;
    CD3DX12_RANGE readRange(0, 0);
    HRESULT hr = m_resource->Map(0, &readRange, reinterpret_cast<void**>(&gpuAddress));
    if (FAILED(hr))
    {
        return false;
    }

    memcpy(gpuAddress, data, (size_t)m_bufferSize);

    return true;
}

void UploadBuffer::Unmap() const
{
    CD3DX12_RANGE readRange(0, 0);
    Resource()->Unmap(0, &readRange);
}

#pragma endregion

#pragma region PixelBuffer

PixelBuffer::PixelBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> pResource, 
    D3D12_RESOURCE_STATES resourceState, 
    DXGI_FORMAT format, u64 width, u32 height) :
    GpuResource(pResource, resourceState), 
    m_width(width), 
    m_height(height),
    m_mipLevels(1),
    m_arraySize(1),
    m_format(format)
{

}

PixelBuffer::PixelBuffer(D3D12_RESOURCE_STATES resourceState, DXGI_FORMAT format, u64 width, u32 height) :
    PixelBuffer(nullptr, resourceState, format, width, height)
{

}

PixelBuffer::~PixelBuffer()
{

}

bool PixelBuffer::InitializeTexture2D(std::shared_ptr<const Resource::Texture> texture/* = nullptr*/)
{
    auto pTexImpl = texture->GetImpl<Dx12::TextureImpl>();
    if (!pTexImpl)
    {
        return false;
    }
 
    const DirectX::ScratchImage& image = *pTexImpl->m_image;
    DirectX::TexMetadata imageMetadata = image.GetMetadata();

    m_format = imageMetadata.format;
    m_viewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    m_mipLevels = (u16)imageMetadata.mipLevels;

    ID3D12Device* pDevice = Device::SharedInstance()->DX();

    HRESULT hr;
    hr = CreateTexture(pDevice, imageMetadata, m_resource.ReleaseAndGetAddressOf());
    if (FAILED(hr))
    {
        return false;
    }

    m_resource->SetName(pTexImpl->Filename().c_str());

    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    hr = PrepareUpload(pDevice, image.GetImages(), image.GetImageCount(), imageMetadata, subresources);
    if (FAILED(hr))
    {
        return false;
    }

    const u32 numSubresources = static_cast<u32>(subresources.size());
    const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_resource.Get(), 0, numSubresources);

    ID3D12Resource* uploadBuffer;
    if (!CreateUploadBuffer(pDevice, &uploadBuffer, uploadBufferSize))
    {
        return false;
    }

    auto pCtx = GraphicsCommandContext::Create();
    auto pCommandList = pCtx->CommandList();
    UpdateSubresources(pCommandList, m_resource.Get(), uploadBuffer, 0, 0, numSubresources, subresources.data());
    TransitionResource(pCommandList, m_resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, m_resourceState);

    pCtx->Execute(true);
    SAFE_RELEASE(uploadBuffer);

    return true;
}

bool PixelBuffer::InitializeTexture2D(void* data /*= nullptr*/)
{
    //if (data)
    //{
    //    auto pDevice = pRenderer->GetDevice()->DX();

    //    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    //    HRESULT hr = DirectX::LoadDDSTextureFromMemoryEx(pDevice, (u8*)data, m_size, 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_FLAGS::DDS_LOADER_MIP_AUTOGEN, &m_resource, subresources);
    //    if (FAILED(hr))
    //    {
    //        return false;
    //    }

    //    //const u64 uploadBufferSize = GetRequiredIntermediateSize(m_resource, 0, static_cast<u32>(subresources.size()));

    //    ID3D12Resource* uploadBuffer;
    //    if (!CreateUploadTexture2D(pDevice, &uploadBuffer, m_width, m_height, m_mipLevels, m_format))
    //    {
    //        return false;
    //    }

    //    auto& pCtx = pRenderer->GetContext();
    //    auto pCommandList = pCtx->CommandList();
    //    int bytesPerRow = GetDXGIFormatBitsPerPixel(m_format);
    //    UpdateBufferResource(pCommandList, m_resource, uploadBuffer, data, (LONG_PTR)bytesPerRow, (LONG_PTR)(bytesPerRow * m_height));
    //    TransitionResource(pCommandList, m_resource, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST, m_resourceState);

    //    auto& pCommandQueue = pRenderer->GetCommandQueue();
    //    pCommandQueue->Execute(pCtx, true);
    //    SAFE_RELEASE(uploadBuffer);
    //}

    return true;
}

bool PixelBuffer::InitializeTexture2D( 
    const u64 width,
    const u32 height, 
    const u32 size,
    const DXGI_FORMAT format,
    const u16 mipLevels, 
    void* data /*= nullptr*/)
{
    m_dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    m_width = width;
    m_height = height;
    m_size = size;
    m_format = format;
    m_arraySize = 1;
    m_mipLevels = mipLevels;
    m_sampleCount = 1;
    m_sampleQuality = 0;
    m_viewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    return InitializeTexture2D(data);
}

void PixelBuffer::CreateShaderResourceView(const DescriptorHandle& descriptorHandle, u32 offset /*= 0*/)
{
    m_srvHandle = descriptorHandle;

    D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    desc.Format = m_format;
    desc.ViewDimension = m_viewDimension;
    desc.Texture2D.MipLevels = m_mipLevels;

    Device::SharedInstance()->DX()->CreateShaderResourceView(m_resource.Get(), &desc, m_srvHandle.CpuHandle(offset));
}

void PixelBuffer::CreateShaderResourceView()
{
    CreateShaderResourceView(Dx12::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
}

void PixelBuffer::CreateRenderTargetView(const DescriptorHandle& descriptorHandle, u32 offset /*= 0*/)
{
    m_rtvHandle = descriptorHandle;

    Device::SharedInstance()->DX()->CreateRenderTargetView(m_resource.Get(), nullptr, m_rtvHandle.CpuHandle(offset));
}

void PixelBuffer::CreateRenderTargetView()
{
    CreateRenderTargetView(Dx12::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
}

PixelBuffer::PixelBuffer(PixelBuffer&& other) : 
    GpuResource(std::move(other))
{
    m_format = other.m_format;
    m_width = other.m_width;
    m_height = other.m_height;
    m_size = other.m_size;
    m_arraySize = other.m_arraySize;
    m_dimension = other.m_dimension;
    m_sampleQuality = other.m_sampleQuality;
    m_viewDimension = other.m_viewDimension;
    m_sampleCount = other.m_sampleCount;
    m_mipLevels = other.m_mipLevels;

    other.m_format = DXGI_FORMAT_UNKNOWN;
    other.m_width = -1;
    other.m_height = -1;
    other.m_size = -1;
    other.m_arraySize = -1;
    other.m_dimension = D3D12_RESOURCE_DIMENSION_UNKNOWN;
    other.m_viewDimension = D3D12_SRV_DIMENSION_UNKNOWN;
    other.m_sampleQuality = -1;
    other.m_sampleCount = -1; 
    other.m_mipLevels = -1;
}

PixelBuffer& PixelBuffer::operator=(PixelBuffer&& other)
{
    GpuResource::operator=(std::move(other));

    m_format = other.m_format;
    m_width = other.m_width;
    m_height = other.m_height;
    m_size = other.m_size;
    m_arraySize = other.m_arraySize;
    m_dimension = other.m_dimension;
    m_sampleQuality = other.m_sampleQuality;
    m_viewDimension = other.m_viewDimension;
    m_sampleCount = other.m_sampleCount;
    m_mipLevels = other.m_mipLevels;

    other.m_format = DXGI_FORMAT_UNKNOWN;
    other.m_width = -1;
    other.m_height = -1;
    other.m_size = -1;
    other.m_arraySize = -1;
    other.m_dimension = D3D12_RESOURCE_DIMENSION_UNKNOWN;
    other.m_viewDimension = D3D12_SRV_DIMENSION_UNKNOWN;
    other.m_sampleQuality = -1;
    other.m_sampleCount = -1;
    other.m_mipLevels = -1;

    return *this;
}

#pragma endregion

ColorBuffer::ColorBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> pResource,
    D3D12_RESOURCE_STATES resourceState,
    DXGI_FORMAT format, u64 width, u32 height,
    const float* color) : 
    PixelBuffer(pResource, resourceState, format, width, height)
{
    SetColor(color);
}
ColorBuffer::ColorBuffer(D3D12_RESOURCE_STATES resourceState,
    DXGI_FORMAT format, u64 width, u32 height,
    const float* color) : 
    ColorBuffer(nullptr, resourceState, format, width, height)
{

}

ColorBuffer::~ColorBuffer()
{
    
}

ColorBuffer::ColorBuffer(ColorBuffer&& other) : PixelBuffer(std::move(other))
{
    m_color[0] = other.m_color[0];
    m_color[1] = other.m_color[1];
    m_color[2] = other.m_color[2];
    m_color[3] = other.m_color[3];

    other.m_color[0] = 1;
    other.m_color[1] = 0;
    other.m_color[2] = 0;
    other.m_color[3] = 0;
}

ColorBuffer& ColorBuffer::operator=(ColorBuffer&& other)
{
    PixelBuffer::operator=(std::move(other));

    m_color[0] = other.m_color[0];
    m_color[1] = other.m_color[1];
    m_color[2] = other.m_color[2];
    m_color[3] = other.m_color[3];

    other.m_color[0] = 1;
    other.m_color[1] = 0;
    other.m_color[2] = 0;
    other.m_color[3] = 0;

    return *this;
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

DepthBuffer::DepthBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> pResource,
    D3D12_RESOURCE_STATES resourceState/* = D3D12_RESOURCE_STATE_COMMON*/,
    DXGI_FORMAT format/* = DXGI_FORMAT_D24_UNORM_S8_UINT*/,
    u64 width /*= 0*/,
    u32 height /*= 0*/,
    D3D12_CLEAR_FLAGS clearFlags /*= D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL*/,
    float clearDepth /*= 1.0f*/,
    u8 clearStencil /*= 0*/) :
    PixelBuffer(pResource, resourceState, format, width, height),
    m_clearFlags(clearFlags),
    m_clearDepth(clearDepth),
    m_clearStencil(clearStencil)
{

}

DepthBuffer::DepthBuffer(D3D12_RESOURCE_STATES resourceState/* = D3D12_RESOURCE_STATE_COMMON*/,
    DXGI_FORMAT format/* = DXGI_FORMAT_D24_UNORM_S8_UINT*/,
    u64 width /*= 0*/,
    u32 height /*= 0*/,
    D3D12_CLEAR_FLAGS clearFlags /*= D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL*/,
    float clearDepth /*= 1.0f*/,
    u8 clearStencil /*= 0*/) :
    DepthBuffer(nullptr, resourceState, format, width, height)
{

}

DepthBuffer::~DepthBuffer()
{

}

bool DepthBuffer::Initialize(u32 width, u32 height)
{
    m_width = width;
    m_height = height;

    auto pDevice = Device::SharedInstance()->DX();

    if (!CreateDepthStencilResource(pDevice, m_resource.ReleaseAndGetAddressOf(), m_width, m_height, 1, 0, 1, 0))
    {
        return false;
    }

    CreateDepthStencilView();

    return true;
}

DepthBuffer::DepthBuffer(DepthBuffer&& other) : PixelBuffer(std::move(other))
{
    m_clearDepth = other.m_clearDepth;
    m_clearFlags = other.m_clearFlags;
    m_clearStencil = other.m_clearStencil;

    other.m_clearDepth = std::nanf("");
    other.m_clearFlags = (D3D12_CLEAR_FLAGS)-1;
    other.m_clearStencil = -1;
}

DepthBuffer& DepthBuffer::operator=(DepthBuffer&& other)
{
    PixelBuffer::operator=(std::move(other));

    m_clearDepth = other.m_clearDepth;
    m_clearFlags = other.m_clearFlags;
    m_clearStencil = other.m_clearStencil;

    other.m_clearDepth = std::nanf("");
    other.m_clearFlags = (D3D12_CLEAR_FLAGS)-1;
    other.m_clearStencil = -1;

    return *this;
}

void DepthBuffer::CreateDepthStencilView(const DescriptorHandle& descriptorHandle, u32 offset /*= 0*/)
{
    m_dsvHandle = descriptorHandle;

    D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
    depthStencilDesc.Format = m_format;
    depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

    Device::SharedInstance()->DX()->CreateDepthStencilView(m_resource.Get(), &depthStencilDesc, m_dsvHandle.CpuHandle(offset));
}

void DepthBuffer::CreateDepthStencilView()
{
    CreateDepthStencilView(Dx12::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV));
}
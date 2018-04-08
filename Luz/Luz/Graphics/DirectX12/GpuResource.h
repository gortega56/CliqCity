#pragma once
#ifndef DX12_GPURESOURCE_H
#define DX12_GPURESOURCE_H

#ifndef __D3DX12_H__
#include "d3dx12.h"
#endif

#ifndef DX12_DESCRIPTORHEAP_H
#include "DescriptorHeap.h"
#endif

namespace Resource
{
    class Texture;
}

namespace Dx12
{
    class CommandQueue;
    class GraphicsCommandContext;
    class Device;

    class GpuResource
    {
    public:
        GpuResource(Microsoft::WRL::ComPtr<ID3D12Resource> pResource, D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COMMON);
        GpuResource(D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COMMON);
        ~GpuResource();

        inline Microsoft::WRL::ComPtr<ID3D12Resource> SharedResource() { return m_resource; }
        inline ID3D12Resource* Resource() const { return m_resource.Get(); }
        inline D3D12_RESOURCE_STATES ResourceState() const { return m_resourceState; }

        inline void SetResourceState(D3D12_RESOURCE_STATES state) { m_resourceState = state; }
        
        GpuResource(GpuResource&& other);
        GpuResource& operator=(GpuResource&& other);

    protected:
        Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
        D3D12_RESOURCE_STATES m_resourceState;

        NO_COPY(GpuResource)
    };

    class GpuBuffer : public GpuResource
    {
    public:
        using GpuResource::GpuResource;

        GpuBuffer();
        ~GpuBuffer();

        bool Initialize(u64 bufferSize, u32 elementSize, u32 numElements, void* data = nullptr);
        virtual bool Initialize(void* data = nullptr);

        inline void SetBufferSize(u64 bufferSize) { m_bufferSize = bufferSize; }
        inline void SetElementSize(u32 elementSize) { m_elementSize = elementSize; }
        inline void SetNumElements(u32 numElements) { m_numElements = numElements; }

        inline u64 BufferSize() const { return m_bufferSize; }
        inline u32 ElementSize() const { return m_elementSize; }
        inline u32 NumElements() const { return m_numElements; }

        D3D12_VERTEX_BUFFER_VIEW VertexBufferView();
        D3D12_INDEX_BUFFER_VIEW IndexBufferView();

        inline D3D12_GPU_VIRTUAL_ADDRESS RootConstantBufferView() const { return m_resource->GetGPUVirtualAddress(); }

        GpuBuffer(GpuBuffer&& other);
        GpuBuffer& operator=(GpuBuffer&& other);

    protected:
        u64 m_bufferSize;
        u32 m_elementSize;
        u32 m_numElements;

        NO_COPY(GpuBuffer)
    };

    class UploadBuffer : public GpuBuffer
    {
    public:
        UploadBuffer();
        ~UploadBuffer();

        bool Initialize(void* data = nullptr) override;

        template<class DataType>
        bool InitializeStructure(DataType* data);
        //void ConstantBufferView(Renderer* pRenderer, DescriptorHeap* pCBVHeap, int i = 0);

        bool Map(void* data) const;
        void Unmap() const;

        NO_COPY(UploadBuffer)
    };

    template<class DataType>
    bool UploadBuffer::InitializeStructure(DataType* data)
    {
        return GpuBuffer::Initialize(sizeof(DataType), sizeof(DataType), 1, data);
    }

    class PixelBuffer : public GpuResource
    {
    public:
        PixelBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> pResource, 
            D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COMMON, 
            DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, 
            u64 width = 0,
            u32 height = 0);
        PixelBuffer(D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COMMON, 
            DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, 
            u64 width = 0,
            u32 height = 0);
        ~PixelBuffer();

        bool InitializeTexture2D(std::shared_ptr<const Resource::Texture> texture = nullptr);
        bool InitializeTexture2D(void* data = nullptr);
        bool InitializeTexture2D(
            const u64 width, 
            const u32 height, 
            const u32 dataSize,
            const DXGI_FORMAT format, 
            const u16 mipLevels,
            void* data = nullptr);

        inline void SetDimension(D3D12_RESOURCE_DIMENSION dimension) { m_dimension = dimension; }
        inline void SetWidth(u64 width) { m_width = width; }
        inline void SetHeight(u32 height) { m_height = height; }
        inline void SetSize(u32 size) { m_size = size; }
        inline void SetArraySize(u16 arraySize) { m_arraySize = arraySize; }
        inline void SetMipLevels(u16 mipLevels) { m_mipLevels = mipLevels; }
        inline void SetFormat(DXGI_FORMAT format) { m_format = format; }
        inline void SetSampleCount(u32 sampleCount) { m_sampleCount = sampleCount; }
        inline void SetSampleQuality(u32 sampleQuality) { m_sampleQuality = sampleQuality; }
        inline void SetViewDimension(D3D12_SRV_DIMENSION viewDimension) { m_viewDimension = viewDimension; }

        inline D3D12_RESOURCE_DIMENSION Dimension() const { return m_dimension; }
        inline u64 Width() const { return m_width; }
        inline u32 Height() const { return m_height; }
        inline u32 Size() const { return m_size; }
        inline u16 ArraySize() const { return m_arraySize; }
        inline u16 MipLevels() const { return m_mipLevels; }
        inline DXGI_FORMAT Format() const { return m_format; }
        inline u32 SampleCount() const { return m_sampleCount; }
        inline u32 SampleQuality() const { return m_sampleQuality; }
        inline D3D12_SRV_DIMENSION ViewDimension() const { return m_viewDimension; }

        inline const DescriptorHandle ShaderResourceViewHandle() const { return m_srvHandle; }
        inline const DescriptorHandle RenderTargetViewHandle() const { return m_rtvHandle; }

        void CreateShaderResourceView(const DescriptorHandle& descriptorHandle, u32 offset = 0);
        void CreateShaderResourceView();

        void CreateRenderTargetView(const DescriptorHandle& descriptorHandle, u32 offset = 0);
        void CreateRenderTargetView();

        PixelBuffer(PixelBuffer&& other);
        PixelBuffer& operator=(PixelBuffer&& other);

    protected:
        D3D12_RESOURCE_DIMENSION m_dimension;
        u64 m_width;
        u32 m_height;
        u32 m_size;
        u16 m_arraySize;
        u16 m_mipLevels;
        DXGI_FORMAT m_format;
        u32 m_sampleCount;
        u32 m_sampleQuality;   
        D3D12_SRV_DIMENSION m_viewDimension;
        DescriptorHandle m_srvHandle;
        DescriptorHandle m_rtvHandle;

        NO_COPY(PixelBuffer)
    };

    class ColorBuffer : public PixelBuffer
    {
    public:
        ColorBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> pResource, 
            D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COMMON, 
            DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, 
            u64 width = 0,
            u32 height = 0, 
            const float* color = nullptr);
        ColorBuffer(D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COMMON,
            DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN,
            u64 width = 0,
            u32 height = 0,
            const float* color = nullptr);
        ~ColorBuffer();

        void SetColor(const float* color);
        void SetColor(float r, float g, float b, float a);
        float const* Color() const { return m_color; }

        ColorBuffer(ColorBuffer&& other);
        ColorBuffer& operator=(ColorBuffer&& other);

    protected:
        float m_color[4];

        NO_COPY(ColorBuffer)
    };

    class DepthBuffer : public PixelBuffer
    {
    public:
        DepthBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> pResource,
            D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COMMON,
            DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT,
            u64 width = 0,
            u32 height = 0,
            D3D12_CLEAR_FLAGS clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
            float depthClear = 1.0f,
            u8 stencilClear = 0);
        DepthBuffer(D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COMMON,
            DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT,
            u64 width = 0,
            u32 height = 0,
            D3D12_CLEAR_FLAGS clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
            float depthClear = 1.0f,
            u8 stencilClear = 0);
        ~DepthBuffer();

        bool Initialize(u32 width, u32 height);

        float ClearDepth() const { return m_clearDepth; }
        u8 ClearStencil() const { return m_clearStencil; }
        D3D12_CLEAR_FLAGS ClearFlags() const { return m_clearFlags; }

        void SetClearDepth(float f) { m_clearDepth = f; }
        void SetClearStencil(u8 s) { m_clearStencil = s; }
        void SetClearFlags(D3D12_CLEAR_FLAGS flags) { m_clearFlags |= flags; }

        inline DescriptorHandle DepthStencilViewHandle() const { return m_dsvHandle; }

        void CreateDepthStencilView(const DescriptorHandle& descriptorHandle, u32 offset = 0);
        void CreateDepthStencilView();

        DepthBuffer(DepthBuffer&& other);
        DepthBuffer& operator=(DepthBuffer&& other);

    private:
        DescriptorHandle m_dsvHandle;
        D3D12_CLEAR_FLAGS m_clearFlags;
        float m_clearDepth;
        u8 m_clearStencil;

        NO_COPY(DepthBuffer)
    };
}

#endif
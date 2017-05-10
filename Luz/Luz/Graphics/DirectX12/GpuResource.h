#pragma once
#ifndef DX12_GPURESOURCE_H
#define DX12_GPURESOURCE_H

#ifndef DX12_INTERNAL_H
#include "dx12_internal.h"
#endif

namespace Dx12
{
    class Renderer;
    class DescriptorHeap;
    class CommandQueue;
    class GraphicsCommandContext;
    class Device;

    class GpuResource
    {
    public:
        GpuResource(ID3D12Resource* pResource, D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COMMON);
        GpuResource(D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COMMON);
        ~GpuResource();

        ID3D12Resource* Resource() const { return m_resource; }
        D3D12_RESOURCE_STATES ResourceState() const { return m_resourceState; }

        GpuResource(GpuResource&& other);
    protected:
        ID3D12Resource* m_resource;
        D3D12_RESOURCE_STATES m_resourceState;

        GpuResource(const GpuResource& other) = delete;
    };

    class GpuBuffer : public GpuResource
    {
    public:
        using GpuResource::GpuResource;

        GpuBuffer();
        ~GpuBuffer();

        bool Initialize(std::shared_ptr<const CommandQueue> pQueue, 
            std::shared_ptr<GraphicsCommandContext> pDevice, 
            const u64 bufferSize, u32 elementSize, u32 numElements, 
            void* data = nullptr);

        u64 BufferSize() const { return m_bufferSize; }
        u32 ElementSize() const { return m_elementSize; }
        u32 NumElements() const { return m_numElements; }

        D3D12_VERTEX_BUFFER_VIEW VertexBufferView();
        D3D12_INDEX_BUFFER_VIEW IndexBufferView();

        D3D12_GPU_VIRTUAL_ADDRESS RootConstantBufferView() const { return m_resource->GetGPUVirtualAddress(); }

    protected:
        u64 m_bufferSize;
        u32 m_elementSize;
        u32 m_numElements;
    };

    class UploadBuffer : public GpuBuffer
    {
    public:
        UploadBuffer();
        ~UploadBuffer();

        bool Initialize(std::shared_ptr<GraphicsCommandContext> pCtx, const u64 bufferSize, u32 elementSize, u32 numElements, void* data);

        template<class DataType>
        bool Initialize(std::shared_ptr<GraphicsCommandContext> pCtx, DataType* data);
        //void ConstantBufferView(Renderer* pRenderer, DescriptorHeap* pCBVHeap, int i = 0);

        bool Map(void* data);
        void Unmap();

    private:
        UINT8* m_gpuAddress;
    };

    template<class DataType>
    bool UploadBuffer::Initialize(std::shared_ptr<GraphicsCommandContext> pCtx, DataType* data)
    {
        return Initialize(pCtx, sizeof(DataType), sizeof(DataType), 1, data);
    }

    class PixelBuffer : public GpuResource
    {
    public:
        PixelBuffer(ID3D12Resource* pResource, 
            D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COMMON, 
            DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, 
            u32 width = 0, 
            u32 height = 0);
        PixelBuffer(D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COMMON, 
            DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, 
            u32 width = 0, 
            u32 height = 0);
        ~PixelBuffer();

        bool Initialize(std::shared_ptr<const CommandQueue> pQueue, std::shared_ptr<GraphicsCommandContext> pDevice, const u32 width, const u32 height, const DXGI_FORMAT format, void* data);

        void SetWidth(u32 width) { m_width = width; }
        void SetHeight(u32 height) { m_height = height; }
        void SetFormat(DXGI_FORMAT format) { m_format = format; }

        u32 Width() const { return m_width; }
        u32 Height() const { return m_height; }
        DXGI_FORMAT Format() const { return m_format; }

        PixelBuffer(PixelBuffer&& other);

    protected:
        u32 m_width;
        u32 m_height;
        u16 m_mipLevels;
        u16 m_arraySize;
        DXGI_FORMAT m_format;
    };

    class ColorBuffer : public PixelBuffer
    {
    public:
        ColorBuffer(ID3D12Resource* pResource, 
            D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COMMON, 
            DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, 
            u32 width = 0, 
            u32 height = 0, 
            const float* color = nullptr);
        ColorBuffer(D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COMMON,
            DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN,
            u32 width = 0,
            u32 height = 0,
            const float* color = nullptr);
        ~ColorBuffer();

        void SetColor(const float* color);
        void SetColor(float r, float g, float b, float a);
        float const* Color() const { return m_color; }

        ColorBuffer(ColorBuffer&& other);

    protected:
        float m_color[4];
    };

    class DepthBuffer : public PixelBuffer
    {
    public:
        DepthBuffer(u32 width = 0, u32 height = 0);
        ~DepthBuffer();

        bool Initialize(std::shared_ptr<const Device> pDevice, std::shared_ptr<DescriptorHeap> pHeap);

        float ClearDepth() const { return m_clearDepth; }
        u8 ClearStencil() const { return m_clearStencil; }
        D3D12_CLEAR_FLAGS ClearFlags() const { return m_clearFlags; }

        void SetClearDepth(float f) { m_clearDepth = f; }
        void SetClearStencil(u8 s) { m_clearStencil = s; }
        void SetClearFlags(D3D12_CLEAR_FLAGS flags) { m_clearFlags = flags; }

    private:
        float m_clearDepth;
        u8 m_clearStencil;
        D3D12_CLEAR_FLAGS m_clearFlags;
    };
}

#endif
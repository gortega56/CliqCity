#pragma once
#ifndef GPURESOURCE_H
#define GPURESOURCE_H

#ifndef DX12_INTERNAL_H
#include "dx12_internal.h"
#endif

namespace dx12
{
    class Renderer;
    class DescriptorHeap;

    class GpuResource
    {
    public:
        GpuResource(u32 numResources = 1);
        ~GpuResource();

        void Release();

        ID3D12Resource* Resource(int i = 0) { return m_resources[i]; }
        ID3D12Resource** Resources() { return m_resources.data(); }
        ID3D12Resource** ResourceAddress(int i = 0) { return &m_resources[i]; }
        u32 NumResources() { return (u32)m_resources.size(); }

    protected:
        std::vector<ID3D12Resource*> m_resources;
    };

    class GpuBuffer : public GpuResource
    {
    public:
        GpuBuffer(u32 numResources = 1, u64 bufferSize = 0, u32 elementSize = 0);
        ~GpuBuffer();

        bool Initialize(Renderer* pRenderer, D3D12_RESOURCE_STATES state, void* data = nullptr);

        void SetBufferSize(u64 bufferSize) { m_bufferSize = bufferSize; }
        void SetElementSize(u32 elementSize) { m_elementSize = elementSize; }

        u64 BufferSize() { return m_bufferSize; }
        u32 ElementSize() { return m_elementSize; }
        u32 NumElements() { return m_numElements; }

        D3D12_VERTEX_BUFFER_VIEW VertexBufferView();
        D3D12_INDEX_BUFFER_VIEW IndexBufferView();

    protected:
        u64 m_bufferSize;
        u32 m_elementSize;
        u32 m_numElements;
    };

    class DynamicBuffer : public GpuBuffer
    {
    public:
        DynamicBuffer(u32 numResources = 1, u64 bufferSize = 0, u32 elementSize = 0);
        ~DynamicBuffer();

        bool Initialize(Renderer* pRenderer, void* data = nullptr);

        void ConstantBufferView(Renderer* pRenderer, DescriptorHeap* pCBVHeap, int i = 0);

        bool Map(void* data);
        void Unmap();

    private:
        UINT8* m_gpuAddress;
    };

    class PixelBuffer : public GpuResource
    {
    public:
        PixelBuffer(u32 numResources = 1, u32 width = 0, u32 height = 0);
        ~PixelBuffer();

        void SetWidth(u32 width) { m_width.assign(m_resources.size(), width); }
        void SetHeight(u32 height) { m_height.assign(m_resources.size(), height); }

        void SetWidth(u32 width, int i) { m_width[i] = width; }
        void SetHeight(u32 height, int i) { m_height[i] = height; }

        u32 Width(int i = 0) { return m_width[i]; }
        u32 Height(int i = 0) { return m_height[i]; }

        void SetFormat(DXGI_FORMAT format) { m_formats.assign(m_resources.size(), format); }
        void SetFormat(DXGI_FORMAT format, int i) { m_formats[i] = format; }
        DXGI_FORMAT Format(int i = 0) { return m_formats[i]; }

    protected:
        std::vector<u32> m_width;
        std::vector<u32> m_height;
        std::vector<DXGI_FORMAT> m_formats;
    };

    class ColorBuffer : public PixelBuffer
    {
    public:
        ColorBuffer(u32 numResources = 1, u32 width = 0, u32 height = 0, float clearColor[4] = { 0 });
        ~ColorBuffer();

        void SetColor(float color[4]);
        void SetColor(float color[4], int i);
        float* Color(int i = 0) { return m_colors[i]; }

    protected:
        std::vector<float[4]> m_colors;
    };

    class DepthBuffer : public PixelBuffer
    {
    public:
        DepthBuffer(u32 width = 0, u32 height = 0);
        ~DepthBuffer();

        bool Initialize(Renderer* pRenderer, DescriptorHeap* pDescriptorHeap);

        float ClearDepth() { return m_clearDepth; }
        u8 ClearStencil() { return m_clearStencil; }
        D3D12_CLEAR_FLAGS ClearFlags() { return m_clearFlags; }

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
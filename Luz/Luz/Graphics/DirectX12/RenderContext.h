#pragma once
#ifndef DX12RENDERCONTEXT_H
#define DX12RENDERCONTEXT_H

#ifndef GPURESOURCE_H
#include "GpuResource.h"
#endif

#ifndef DESCRIPTORHEAP_H
#include "DescriptorHeap.h"
#endif

namespace dx12
{
    class Device;
    class SwapChain;

    class RenderContext
    {
    public:
        RenderContext();
        ~RenderContext();

        bool Initialize(std::shared_ptr<const Device> pDevice, std::shared_ptr<SwapChain>, i32 width, i32 height);
        
        inline ColorBuffer const* RTV(int i) const { return &m_rtvs[i]; }
        inline DepthBuffer const* DSV() const { return &m_dsv; }
        inline i32 NumRTVs() const { return (i32)m_rtvs.size(); }

        inline DescriptorHeap* RtvHeap() { return m_rtvDescriptorHeap.get(); }
        inline DescriptorHeap* DsvHeap() { return m_dsvDescriptorHeap.get(); }

        inline u32 Width() const { return m_width; }
        inline u32 Height() const { return m_height; }
        inline float AspectRatio() const { return (float)m_width / (float)m_height; }

        void SetColor(const float* color);
        void SetClearDepth(float f);
        void SetClearStencil(u8 s);
        void SetClearFlags(D3D12_CLEAR_FLAGS flags);

    private:
        u32 m_width;
        u32 m_height;

        std::vector<ColorBuffer> m_rtvs;
        DepthBuffer m_dsv;

        std::shared_ptr<DescriptorHeap> m_rtvDescriptorHeap;
        std::shared_ptr<DescriptorHeap> m_dsvDescriptorHeap;
    };
}

#endif // !DX12RENDERCONTEXT_H


#pragma once
#ifndef DX12_RENDERCONTEXT_H
#define DX12_RENDERCONTEXT_H

#ifndef DX12_GPURESOURCE_H
#include "GpuResource.h"
#endif

namespace Dx12
{
    class Device;
    class SwapChain;
    //class ColorBuffer;
    //class DepthBuffer;
    class DescriptorHeap;

    class IRenderContext
    {

    };

    class RenderContext
    {
    public:
        RenderContext();
        ~RenderContext();

        bool Initialize(std::shared_ptr<const Device> pDevice, std::shared_ptr<SwapChain>, i32 width, i32 height);
        
        //inline ColorBuffer const* RTV(int i) const { return m_rtvs[i].get(); }
        //inline DepthBuffer const* DSV() const { return m_dsv.get(); }
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
        void SetClearDepthFlag();
        void SetClearStencilFlag();

    private:
        u32 m_width;
        u32 m_height;

        //std::vector<std::shared_ptr<ColorBuffer>> m_rtvs;
        //std::shared_ptr<DepthBuffer> m_dsv;

        std::vector<ColorBuffer> m_rtvs;
        DepthBuffer m_dsv;

        std::shared_ptr<DescriptorHeap> m_rtvDescriptorHeap;
        std::shared_ptr<DescriptorHeap> m_dsvDescriptorHeap;
    };
}

#endif // !DX12_RENDERCONTEXT_H


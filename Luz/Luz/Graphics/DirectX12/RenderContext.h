#pragma once
#ifndef DX12_RENDERCONTEXT_H
#define DX12_RENDERCONTEXT_H

#ifndef DX12_VIEWPORT_H
#include "Viewport.h"
#endif

#ifndef DX12_GPURESOURCE_H
#include "GpuResource.h"
#endif

#ifndef DX12_DESCRIPTORHEAP_H
#include "DescriptorHeap.h"
#endif

namespace Dx12
{
    class Device;
    class SwapChain;

    class RenderContext
    {
    public:
        RenderContext();
        ~RenderContext();

        //bool Initialize(std::shared_ptr<const Device> pDevice, std::shared_ptr<SwapChain>, u32 width, u32 height);

        inline u32 NumRenderTargetViews() const { return (u32)m_colorBuffers.size(); }

        inline u32 Width() const { return m_width; }
        inline u32 Height() const { return m_height; }
        inline float AspectRatio() const { return (float)m_width / (float)m_height; }

        void SetColor(const float* color);
        void SetClearDepth(float f);
        void SetClearStencil(u8 s);
        void SetClearDepthFlag();
        void SetClearStencilFlag();

        DescriptorHandle RenderTargetView(int i) const;
        DescriptorHandle DepthStencilView() const;

    protected:
        u32 m_width;
        u32 m_height;

        std::vector<ColorBuffer> m_colorBuffers;
        DepthBuffer m_depthBuffer;

        friend class GraphicsCommandContext;
    };

    class SwapChainContext : public RenderContext
    {
    public:
        SwapChainContext();
        ~SwapChainContext();
        
        bool Initialize(std::shared_ptr<SwapChain> pSwapChain);

    protected:
        friend class GraphicsCommandContext;
        
        std::shared_ptr<SwapChain> m_swapChain;
        Viewport m_viewport;

        NO_COPY(SwapChainContext)
        NO_MOVE(SwapChainContext)
    };
}

#endif // !DX12_RENDERCONTEXT_H


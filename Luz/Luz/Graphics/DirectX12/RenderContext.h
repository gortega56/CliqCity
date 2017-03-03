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
    class RenderContext
    {
    public:
        RenderContext(u32 numRtvs = 1);
        ~RenderContext();

        bool InitializeWithSwapChain(Renderer* pRenderer, int width, int height);
        
        void Release();

        ColorBuffer* RTV() { return &m_rtvs; }
        DepthBuffer* DSV() { return &m_dsv; }

        DescriptorHeap* RtvHeap() { return &m_rtvDescriptorHeap; }
        DescriptorHeap* DsvHeap() { return &m_dsvDescriptorHeap; }

    private:
        ColorBuffer m_rtvs;
        DepthBuffer m_dsv;

        DescriptorHeap m_rtvDescriptorHeap;
        DescriptorHeap m_dsvDescriptorHeap;
    };
}

#endif // !DX12RENDERCONTEXT_H


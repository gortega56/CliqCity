#pragma once
#ifndef DX12_SWAPCHAIN_H
#define DX12_SWAPCHAIN_H

#ifndef __D3DX12_H__
#include "d3dx12.h"
#endif

struct IDXGISwapChain;
struct IDXGISwapChain1;
struct IDXGISwapChain2;
struct IDXGISwapChain3;

namespace Dx12
{
    class Device;
    class CommandQueue;
    class DescriptorHeap;

    class SwapChain
    {
    public:
        SwapChain(DXGI_FORMAT format, 
            DXGI_USAGE usage, 
            DXGI_SWAP_EFFECT swapEffect);
        ~SwapChain();

        bool Initialize(std::shared_ptr<const Device> pDevice, std::shared_ptr<const CommandQueue> pCmdQueue, HWND hwnd, u32 numFrameBuffers, u32 width, u32 height, bool fullScreen);
        bool Present();

        inline IDXGISwapChain* GetSwapChain() const { return m_swapChain; }
        inline IDXGISwapChain1* GetSwapChain1() const { return m_swapChain1; }
        inline IDXGISwapChain2* GetSwapChain2() const { return m_swapChain2; }
        inline IDXGISwapChain3* GetSwapChain3() const { return m_swapChain3; }
    
        inline DXGI_SWAP_EFFECT SwapEffect() const { return m_swapEffect; }
        inline DXGI_FORMAT Format() const { return m_format; }
        inline DXGI_USAGE Usage() const { return m_usage; }
        inline HWND Handle() const { return m_hwnd; }

        inline u32 Width() const { return m_width; }
        inline u32 Height() const { return m_height; }
        
        inline bool FullScreen() const { return m_fullScreen; }

        inline ID3D12Resource* FrameBuffer(int i) const { return m_frameBuffers[i].Get(); }
        inline u32 NumFrameBuffers() const { return (u32)m_frameBuffers.size(); }

        inline u32 GetCurrentBackBufferIndex() const { return m_frameIndex; }

    protected:
        friend class CommandQueue;
        friend class GraphicsCommandContext;

        void Finalize(ID3D12GraphicsCommandList* pGraphicsCommandList) const;

    private:
        IDXGISwapChain* m_swapChain;
        IDXGISwapChain1* m_swapChain1;
        IDXGISwapChain2* m_swapChain2;
        IDXGISwapChain3* m_swapChain3;

        DXGI_SWAP_EFFECT m_swapEffect;
        DXGI_FORMAT m_format;
        DXGI_USAGE m_usage;
        HWND m_hwnd;

        u32 m_width;
        u32 m_height;
        u32 m_frameIndex;

        bool m_fullScreen;

        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_frameBuffers;

        NO_COPY(SwapChain)
    };
}

#endif
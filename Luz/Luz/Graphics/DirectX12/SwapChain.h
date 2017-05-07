#pragma once
#ifndef DX12_SWAPCHAIN_H
#define DX12_SWAPCHAIN_H

#ifndef DX12_INTERNAL_H
#include "dx12_internal.h"
#endif

#ifndef DX12_FENCE_H
#include "Fence.h"
#endif

namespace dx12
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
        bool InitializeFrameBuffers(std::shared_ptr<const Device> pDevice, std::shared_ptr<DescriptorHeap> pHeap);
        bool Present();

        inline IDXGISwapChain* GetSwapChain() const { return m_swapChain; }
        inline IDXGISwapChain1* GetSwapChain1() const { return m_swapChain1; }
        inline IDXGISwapChain2* GetSwapChain2() const { return m_swapChain2; }
        inline IDXGISwapChain3* GetSwapChain3() const { return m_swapChain3; }
    
        inline DXGI_FORMAT Format() const { return m_format; }
        inline DXGI_USAGE Usage() const { return m_usage; }
        inline DXGI_SWAP_EFFECT SwapEffect() const { return m_swapEffect; }
        inline HWND Handle() const { return m_hwnd; }
        inline u32 Width() const { return m_width; }
        inline u32 Height() const { return m_height; }
        inline bool FullScreen() const { return m_fullScreen; }

        inline ID3D12Resource* FrameBuffer(int i) const { return m_frameBuffers[i]; }
        inline ID3D12Resource** FrameBuffers() { return m_frameBuffers.data(); }
        inline u32 NumFrameBuffers() const { return (u32)m_frameBuffers.size(); }

        inline u32 GetCurrentBackBufferIndex() const { return m_frameIndex; }

    protected:
        friend class CommandQueue;
        friend class GraphicsCommandContext;

        Fence* GetFence() { return &m_fences[m_frameIndex]; }
        void Finalize(ID3D12GraphicsCommandList* pGraphicsCommandList) const;

    private:
        IDXGISwapChain* m_swapChain;
        IDXGISwapChain1* m_swapChain1;
        IDXGISwapChain2* m_swapChain2;
        IDXGISwapChain3* m_swapChain3;

        DXGI_FORMAT m_format;
        DXGI_USAGE m_usage;
        DXGI_SWAP_EFFECT m_swapEffect;
        HWND m_hwnd;

        u32 m_width;
        u32 m_height;
        u32 m_frameIndex;

        bool m_fullScreen;

        std::vector<ID3D12Resource*> m_frameBuffers;
        std::vector<Fence> m_fences;

        SwapChain(const SwapChain& other) = delete;
        SwapChain& operator=(const SwapChain& other) = delete;
    };
}

#endif
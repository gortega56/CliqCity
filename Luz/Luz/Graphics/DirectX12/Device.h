#pragma once
#ifndef DEVICE_H
#define DEVICE_H

#ifndef DX12_INTERNAL_H
#include "dx12_internal.h"
#endif

namespace dx12
{
    class Device
    {
    public:
        Device();
        ~Device();

        bool Initialize();
        void Shutdown();

        inline IDXGIFactory* Factory() { return m_factory; }
        inline IDXGIFactory1* Factory1() { return m_factory1; }
        inline IDXGIFactory2* Factory2() { return m_factory2; }
        inline IDXGIFactory3* Factory3() { return m_factory3; }
        inline IDXGIFactory4* Factory4() { return m_factory4; }

        inline ID3D12Device* DX() { return m_device; }
        inline ID3D12Device1* DX1() { return m_device1; }

        inline IDXGISwapChain* SwapChain() { return m_swapChain; }
        inline IDXGISwapChain1* SwapChain1() { return m_swapChain1; }
        inline IDXGISwapChain2* SwapChain2() { return m_swapChain2; }
        inline IDXGISwapChain3* SwapChain3() { return m_swapChain3; }

        inline IDXGISwapChain** SwapChainAddress() { return &m_swapChain; }
        inline IDXGISwapChain1** SwapChain1Address() { return &m_swapChain1; }
        inline IDXGISwapChain2** SwapChain2Address() { return &m_swapChain2; }
        inline IDXGISwapChain3** SwapChain3Address() { return &m_swapChain3; }

    private:
        IDXGIFactory* m_factory;
        IDXGIFactory1* m_factory1;
        IDXGIFactory2* m_factory2;
        IDXGIFactory3* m_factory3;
        IDXGIFactory4* m_factory4;

        ID3D12Device* m_device;
        ID3D12Device1* m_device1;

        IDXGISwapChain* m_swapChain;
        IDXGISwapChain1* m_swapChain1;
        IDXGISwapChain2* m_swapChain2;
        IDXGISwapChain3* m_swapChain3;
    };
}

#endif
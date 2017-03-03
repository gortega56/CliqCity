#include "stdafx.h"
#include "RenderContext.h"
#include "dx12_renderer.h"

using namespace dx12;

RenderContext::RenderContext(u32 numRtvs) : m_rtvs(numRtvs), m_dsv(1), m_rtvDescriptorHeap(numRtvs), m_dsvDescriptorHeap(1)
{
    
}

bool RenderContext::InitializeWithSwapChain(Renderer* pRenderer, int width, int height)
{
    IDXGISwapChain3* pSwapChain = pRenderer->m_device.SwapChain3();

    UINT frameIndex = pSwapChain->GetCurrentBackBufferIndex();

    if (!m_rtvDescriptorHeap.InitializeRTV(pRenderer, L"Swap Chain RTV Descriptor Heap"))
    {
        return false;
    }

    // Book keeping. The underlying resources handled by the swap chain
    m_rtvs.SetWidth(width);
    m_rtvs.SetHeight(height);

    ID3D12Device* pDevice = pRenderer->m_device.DX();

    // Create swap chain rtv resources
    if (!CreateSwapChainRenderTargetViews(pDevice, m_rtvDescriptorHeap.Native(), pSwapChain, m_rtvs.Resources(), m_rtvs.NumResources()))
    {
        return false;
    }

    if (!m_dsvDescriptorHeap.InitializeDSV(pRenderer, L"Main DSV Descriptor Heap"))
    {
        return false;
    }

    m_dsv.SetWidth(width);
    m_dsv.SetHeight(height);

    if (!m_dsv.Initialize(pRenderer, &m_dsvDescriptorHeap))
    {
        return false;
    }

    return true;
}

RenderContext::~RenderContext()
{
    Release();
}

void RenderContext::Release()
{
    m_rtvs.Release();
    m_dsv.Release();
    m_rtvDescriptorHeap.Release();
    m_dsvDescriptorHeap.Release();
}
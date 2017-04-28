#include "stdafx.h"
#include "SwapChain.h"
#include "Device.h"
#include "CommandQueue.h"
#include "DescriptorHeap.h"

using namespace dx12;

SwapChain::SwapChain(DXGI_FORMAT format,
    DXGI_USAGE usage,
    DXGI_SWAP_EFFECT swapEffect) :
    m_swapChain(nullptr),
    m_swapChain1(nullptr),
    m_swapChain2(nullptr),
    m_swapChain3(nullptr),
    m_format(format),
    m_usage(usage),
    m_swapEffect(swapEffect),
    m_hwnd(nullptr),
    m_width(0),
    m_height(0),
    m_frameIndex(0),
    m_fullScreen(false)
{

}

SwapChain::~SwapChain()
{
    // get swapchain out of full screen before exiting
    BOOL fs = false;
    if (m_swapChain3->GetFullscreenState(&fs, NULL))
        m_swapChain3->SetFullscreenState(false, NULL);

    SAFE_RELEASE(m_swapChain);
    SAFE_RELEASE(m_swapChain1);
    SAFE_RELEASE(m_swapChain2);
    SAFE_RELEASE(m_swapChain3);

    for (auto& res : m_frameBuffers)
    {
        SAFE_RELEASE(res);
    }
}

bool SwapChain::Initialize(std::shared_ptr<const Device> pDevice, std::shared_ptr<const CommandQueue> pCmdQueue, HWND hwnd, u32 numFrameBuffers, u32 width, u32 height, bool fullScreen)
{
    m_hwnd = hwnd;
    m_width = width;
    m_height = height;
    m_fullScreen = fullScreen;

    m_frameBuffers.resize(numFrameBuffers);

    if (!InitD3D12SwapChain(pDevice->Factory4(), pCmdQueue->GraphiceQueue(), &m_swapChain3, m_hwnd, m_width, m_height, m_format, m_usage, m_swapEffect, numFrameBuffers, m_fullScreen))
    {
        return false;
    }

    for (u32 i = 0; i < numFrameBuffers; ++i)
    {
        m_fences.emplace_back(Fence());
        if (!m_fences.back().Initialize(pDevice))
        {
            return false;
        }
    }

    return true;
}

bool SwapChain::InitializeFrameBuffers(std::shared_ptr<const Device> pDevice, std::shared_ptr<DescriptorHeap> pHeap)
{
    // Create swap chain rtv resources
    if (!CreateSwapChainRenderTargetViews(pDevice->DX(), pHeap->Native(), m_swapChain3, m_frameBuffers.data(), (UINT)m_frameBuffers.size()))
    {
        return false;
    }

    return true;
}

bool SwapChain::WaitForPreviousFrame()
{
    bool running = m_fences[m_frameIndex].Wait();
    m_frameIndex = m_swapChain3->GetCurrentBackBufferIndex();

    return running;
}

void SwapChain::Finalize(ID3D12GraphicsCommandList* pGraphicsCommandList) const
{
    TransitionResource(pGraphicsCommandList, m_frameBuffers[m_frameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
}
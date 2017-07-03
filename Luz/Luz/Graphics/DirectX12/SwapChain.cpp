#include "stdafx.h"
#include "SwapChain.h"
#include "dx12_internal.h"
#include "Device.h"
#include "CommandQueue.h"
#include "DescriptorHeap.h"

using namespace Dx12;

SwapChain::SwapChain(DXGI_FORMAT format,
    DXGI_USAGE usage,
    DXGI_SWAP_EFFECT swapEffect) :
    m_swapChain(nullptr),
    m_swapChain1(nullptr),
    m_swapChain2(nullptr),
    m_swapChain3(nullptr),
    m_swapEffect(swapEffect),
    m_format(format),
    m_usage(usage),
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
}

bool SwapChain::Initialize(std::shared_ptr<const Device> pDevice, std::shared_ptr<const CommandQueue> pCmdQueue, HWND hwnd, u32 numFrameBuffers, u32 width, u32 height, bool fullScreen)
{
    m_width = width;
    m_height = height;
    m_hwnd = hwnd;
    m_fullScreen = fullScreen;

    m_frameBuffers.resize(numFrameBuffers);

    // Resources are created here but internal to the swap chain

    if (!InitD3D12SwapChain(pDevice->Factory4(), pCmdQueue->GetQueue(), &m_swapChain3, m_hwnd, m_width, m_height, m_format, m_usage, m_swapEffect, numFrameBuffers, m_fullScreen))
    {
        return false;
    }

    // Attach resources to our vector of ComPtrs

    std::vector<ID3D12Resource*> resources(m_frameBuffers.size());
    if (!GetSwapChainBuffers(m_swapChain3, resources.data(), numFrameBuffers))
    {
        return false;
    }

    for (int i = 0; i < (int)m_frameBuffers.size(); ++i)
    {
        m_frameBuffers[i].Attach(resources[i]);
    }

    return true;
}

bool SwapChain::Present()
{
    HRESULT hr = m_swapChain3->Present(0, 0);
    if (FAILED(hr))
    {
        return false;
    }

    m_frameIndex = m_swapChain3->GetCurrentBackBufferIndex();

    return true;
}

void SwapChain::Finalize(ID3D12GraphicsCommandList* pGraphicsCommandList) const
{
    TransitionResource(pGraphicsCommandList, m_frameBuffers[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
}
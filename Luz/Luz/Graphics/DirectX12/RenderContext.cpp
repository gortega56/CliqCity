#include "stdafx.h"
#include "RenderContext.h"
#include "Device.h"
#include "CommandContext.h"
#include "DescriptorHeap.h"
#include "SwapChain.h"
#include "GpuResource.h"

using namespace Dx12;

RenderContext::RenderContext() : m_depthBuffer(nullptr)
{
    
}

RenderContext::~RenderContext()
{

}

void RenderContext::SetColor(const float* color)
{
    for (auto& cb : m_colorBuffers)
    {
        cb.SetColor(color);
    }
}

void RenderContext::SetClearDepth(float f) 
{ 
    m_depthBuffer.SetClearDepth(f); 
}

void RenderContext::SetClearStencil(u8 s) 
{ 
    m_depthBuffer.SetClearStencil(s);
}

void RenderContext::SetClearDepthFlag()
{
    m_depthBuffer.SetClearFlags(D3D12_CLEAR_FLAG_DEPTH);
}

void RenderContext::SetClearStencilFlag()
{
    m_depthBuffer.SetClearFlags(D3D12_CLEAR_FLAG_STENCIL);
}

DescriptorHandle RenderContext::RenderTargetView(int i) const
{
    return m_colorBuffers[i].RenderTargetViewHandle();
}

DescriptorHandle RenderContext::DepthStencilView() const
{
    return m_depthBuffer.DepthStencilViewHandle();
}

SwapChainContext::SwapChainContext() : m_swapChain(nullptr)
{

}

SwapChainContext::~SwapChainContext()
{

}

bool SwapChainContext::Initialize(std::shared_ptr<SwapChain> pSwapChain, std::shared_ptr<CommandQueue> pMainQueue)
{   
    m_mainQueue = pMainQueue;
    m_swapChain = pSwapChain;
    m_width = pSwapChain->Width();
    m_height = pSwapChain->Height();

    m_viewport.SetViewportRect(0.0f, 0.0f, (float)m_width, (float)m_height);
    m_viewport.SetScissorRect(0, 0, m_width, m_height);

    for (int i = 0, count = m_swapChain->NumFrameBuffers(); i < count; ++i)
    {
        m_colorBuffers.emplace_back(m_swapChain->FrameBuffer(i), D3D12_RESOURCE_STATE_RENDER_TARGET, m_swapChain->Format(), m_width, m_height);
    }

    for (auto& cb : m_colorBuffers)
    {
        cb.CreateRenderTargetView();
    }

    if (!m_depthBuffer.Initialize(m_width, m_height))
    {
        return false;
    }

    return true;
}
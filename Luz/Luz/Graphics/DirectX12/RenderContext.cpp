#include "stdafx.h"
#include "RenderContext.h"
#include "Device.h"
#include "CommandContext.h"
#include "DescriptorHeap.h"
#include "SwapChain.h"
#include "GpuResource.h"

using namespace Dx12;

RenderContext::RenderContext() : m_dsv(/*std::make_shared<DepthBuffer>(1)*/1), m_rtvDescriptorHeap(nullptr), m_dsvDescriptorHeap(nullptr)
{
    
}

bool RenderContext::Initialize(std::shared_ptr<const Device> pDevice, std::shared_ptr<SwapChain> pSwapChain, i32 width, i32 height)
{
    m_width = width;
    m_height = height;

    m_rtvDescriptorHeap = std::make_shared<DescriptorHeap>(pSwapChain->NumFrameBuffers());
    if (!m_rtvDescriptorHeap->InitializeRTV(pDevice, L"Swap Chain RTV Descriptor Heap"))
    {
        return false;
    }

    // Create swap chain rtv resources
    if (!pSwapChain->InitializeFrameBuffers(pDevice, m_rtvDescriptorHeap))
    {
        return false;
    }

    for (int i = 0, count = pSwapChain->NumFrameBuffers(); i < count; ++i)
    {
        auto frameBuffer = pSwapChain->FrameBuffer(i);
        //m_rtvs.push_back(std::make_shared<ColorBuffer>(frameBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, pSwapChain->Format(), m_width, m_height));
        m_rtvs.emplace_back(ColorBuffer(frameBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, pSwapChain->Format(), m_width, m_height));
    }

    m_dsvDescriptorHeap = std::make_shared<DescriptorHeap>(1);
    if (!m_dsvDescriptorHeap->InitializeDSV(pDevice, L"Main DSV Descriptor Heap"))
    {
        return false;
    }

    //m_dsv->SetFormat(DXGI_FORMAT_D24_UNORM_S8_UINT);
    //m_dsv->SetWidth(width);
    //m_dsv->SetHeight(height);

    m_dsv.SetFormat(DXGI_FORMAT_D24_UNORM_S8_UINT);
    m_dsv.SetWidth(width);
    m_dsv.SetHeight(height);

    if (!m_dsv.Initialize(pDevice, m_dsvDescriptorHeap))
    {
        return false;
    }

    return true;
}

void RenderContext::SetColor(const float* color)
{
    for (auto& cb : m_rtvs)
    {
        cb.SetColor(color);
    }
}

void RenderContext::SetClearDepth(float f) 
{ 
    m_dsv.SetClearDepth(f); 
}

void RenderContext::SetClearStencil(u8 s) 
{ 
    m_dsv.SetClearStencil(s);
}

void RenderContext::SetClearDepthFlag()
{
    m_dsv.SetClearFlags(D3D12_CLEAR_FLAG_DEPTH);
}

void RenderContext::SetClearStencilFlag()
{
    m_dsv.SetClearFlags(D3D12_CLEAR_FLAG_STENCIL);
}

RenderContext::~RenderContext()
{

}
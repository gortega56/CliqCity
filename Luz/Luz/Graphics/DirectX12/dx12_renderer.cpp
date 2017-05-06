#include "stdafx.h"
#include "dx12_renderer.h"
#include "dx12_renderable.h"
#include "OSWin.h"
#include "CommandQueue.h"
#include "SwapChain.h"

using namespace dx12;

Renderer::Renderer(OSWin* pOS, u32 numThreads, u32 numFrameBuffers) : 
    m_hwnd(pOS->RootWindowHandle()),
    m_device(std::make_shared<Device>()),
    m_commandQueue(std::make_shared<CommandQueue>()),
    m_renderContext(std::make_shared<RenderContext>()),
    m_swapChain(nullptr),
    m_numThreads(numThreads),
    m_numFrameBuffers(numFrameBuffers),
    m_running(true)
{


}

Renderer::~Renderer()
{

}

bool Renderer::Initialize(int width, int height, bool fullscreen)
{
//#ifdef _DEBUG
//    HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&m_debug));
//    if (SUCCEEDED(hr))
//    {
//        m_debug->EnableDebugLayer();
//    }
//#endif

    if (!m_device->Initialize())
    {
        return false;
    }

    //ID3D12InfoQueue* pInfoQueue = nullptr;
    //if (SUCCEEDED(m_device->DX()->QueryInterface(IID_PPV_ARGS(&pInfoQueue))))
    //{
    //    // Suppress whole categories of messages
    //    //D3D12_MESSAGE_CATEGORY Categories[] = {};

    //    // Suppress messages based on their severity level
    //    D3D12_MESSAGE_SEVERITY Severities[] =
    //    {
    //        D3D12_MESSAGE_SEVERITY_CORRUPTION ,
    //        D3D12_MESSAGE_SEVERITY_ERROR,
    //        D3D12_MESSAGE_SEVERITY_WARNING,
    //        D3D12_MESSAGE_SEVERITY_INFO ,
    //        D3D12_MESSAGE_SEVERITY_MESSAGE
    //    };

    //    //// Suppress individual messages by their ID
    //    //D3D12_MESSAGE_ID DenyIds[] =
    //    //{
    //    //    // This occurs when there are uninitialized descriptors in a descriptor table, even when a
    //    //    // shader does not access the missing descriptors.  I find this is common when switching
    //    //    // shader permutations and not wanting to change much code to reorder resources.
    //    //    D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,

    //    //    // Triggered when a shader does not export all color components of a render target, such as
    //    //    // when only writing RGB to an R10G10B10A2 buffer, ignoring alpha.
    //    //    D3D12_MESSAGE_ID_CREATEGRAPHICSPIPELINESTATE_PS_OUTPUT_RT_OUTPUT_MISMATCH,

    //    //    // This occurs when a descriptor table is unbound even when a shader does not access the missing
    //    //    // descriptors.  This is common with a root signature shared between disparate shaders that
    //    //    // don't all need the same types of resources.
    //    //    D3D12_MESSAGE_ID_COMMAND_LIST_DESCRIPTOR_TABLE_NOT_SET,

    //    //    // RESOURCE_BARRIER_DUPLICATE_SUBRESOURCE_TRANSITIONS
    //    //    (D3D12_MESSAGE_ID)1008,
    //    //};

    //    D3D12_INFO_QUEUE_FILTER NewFilter = {};
    //    //NewFilter.DenyList.NumCategories = _countof(Categories);
    //    //NewFilter.DenyList.pCategoryList = Categories;
    //    NewFilter.DenyList.NumSeverities = _countof(Severities);
    //    NewFilter.DenyList.pSeverityList = Severities;
    //    NewFilter.DenyList.NumIDs = 0;// _countof(DenyIds);
    //    NewFilter.DenyList.pIDList = nullptr;// DenyIds;

    //    pInfoQueue->PushStorageFilter(&NewFilter);
    //    pInfoQueue->Release();
    //}

    if (!m_commandQueue->Initialize(m_device))
    {
        return false;
    }

    m_swapChain = std::make_shared<SwapChain>(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_USAGE_RENDER_TARGET_OUTPUT, DXGI_SWAP_EFFECT_FLIP_DISCARD);
    if (!m_swapChain->Initialize(m_device, m_commandQueue, m_hwnd, m_numFrameBuffers, width, height, fullscreen))
    {
        return false;
    }
    
    for (u32 i = 0; i < m_numFrameBuffers; ++i)
    {
        m_graphicsCommandContexts.emplace_back(std::make_shared<GraphicsCommandContext>());
        if (!m_graphicsCommandContexts.back()->Initialize(m_device, m_swapChain))
        {
            return false;
        }
    }

    if (!m_renderContext->Initialize(m_device, m_swapChain, width, height))
    {
        return false;
    }

    float color[4] = { 0.44f, 0.86f, 0.91f, 1.0f };

    m_renderContext->SetColor(color);
    m_renderContext->SetClearDepth(1.0f);
    m_renderContext->SetClearStencil(0);
    m_renderContext->SetClearFlags(D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL);

    m_viewport.SetViewportRect(0, 0, (float)width, (float)height);
    m_viewport.SetScissorRect(0, 0, width, height);


#if _DEBUG

#endif

    return true;
}

bool Renderer::Shutdown()
{
    // wait for the gpu to finish all frames
    CommandContext::WaitForAll(m_graphicsCommandContexts);

    return true;
}

void Renderer::SetRenderContext(std::shared_ptr<GraphicsCommandContext>& pCtx)
{
    pCtx->SetRenderContext(m_renderContext.get());
}

void Renderer::ClearRenderContext(std::shared_ptr<GraphicsCommandContext>& pCtx)
{
    pCtx->ClearRenderContext(m_renderContext.get());
}

void Renderer::SetViewport(std::shared_ptr<GraphicsCommandContext>& pCtx)
{
    pCtx->SetViewport(&m_viewport);
}

void Renderer::Present(std::shared_ptr<GraphicsCommandContext> pCtx)
{
    // Transition back buffer to present
    pCtx->FinalizeSwapChain();

    // Close / Execute / Signal
    m_running = m_commandQueue->Execute(pCtx);
    
    // Present
    HRESULT hr = m_swapChain->GetSwapChain3()->Present(0, 0);
    if (FAILED(hr))
    {
        m_running = false;
    }

    m_commandQueue->Signal(m_swapChain);
}

void Renderer::ExecuteGraphicsCommandContext(std::shared_ptr<GraphicsCommandContext> pGraphicsCommandCtx)
{
    m_running = m_commandQueue->Execute(pGraphicsCommandCtx);
}

void Renderer::WaitForPreviousFrame()
{
    m_swapChain->WaitForPreviousFrame();
}

std::shared_ptr<GraphicsCommandContext> Renderer::GetFrameContext()
{
    //u32 frameIndex = m_swapChain->GetCurrentBackBufferIndex();
    //return m_graphicsCommandContexts[frameIndex];
    return CommandContext::GetNextAvailable(m_graphicsCommandContexts);

}
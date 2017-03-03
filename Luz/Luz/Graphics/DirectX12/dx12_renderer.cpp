#include "stdafx.h"
#include "dx12_renderer.h"
#include "dx12_renderable.h"
#include "OSWin.h"

using namespace dx12;

Renderer::Renderer(OSWin* pOS, u32 numThreads, u32 numFrameBuffers) : 
    m_hwnd(pOS->RootWindowHandle()),
    m_device(),
    m_graphicsCommandCtx(numFrameBuffers),
    m_renderContext(numFrameBuffers),
    m_running(true)
{


}

Renderer::~Renderer()
{

}

bool Renderer::Initialize(int width, int height, bool fullscreen)
{
    if (!m_device.Initialize())
    {
        return false;
    }

    ID3D12Device* pDevice = m_device.DX();

    if (!CreateCommandQueue(pDevice, &m_commandQueue, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_QUEUE_FLAG_NONE, D3D12_COMMAND_QUEUE_PRIORITY_NORMAL))
    {
        return false;
    }

    IDXGIFactory4* pFactory = m_device.Factory4();

    // TODO: Move this into dx12::Device

    if (!InitD3D12SwapChain(pFactory, m_commandQueue, m_device.SwapChain3Address(), m_hwnd, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_USAGE_RENDER_TARGET_OUTPUT, DXGI_SWAP_EFFECT_FLIP_DISCARD, m_graphicsCommandCtx.NumFrameBuffers(), fullscreen))
    {
        return false;
    }

    if (!m_renderContext.InitializeWithSwapChain(this, width, height))
    {
        return false;
    }

    float color[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
    m_renderContext.RTV()->SetColor(color);
    m_renderContext.DSV()->SetClearDepth(1.0f);
    m_renderContext.DSV()->SetClearStencil(0);
    m_renderContext.DSV()->SetClearFlags(D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL);

    
    if (!m_graphicsCommandCtx.Initialize(this))
    {
        return false;
    }

    return true;
}

bool Renderer::Shutdown()
{
    // wait for the gpu to finish all frames
    m_graphicsCommandCtx.Release(this);

    IDXGISwapChain3* pSwapChain = m_device.SwapChain3();

    // get swapchain out of full screen before exiting
    BOOL fs = false;
    if (pSwapChain->GetFullscreenState(&fs, NULL))
        pSwapChain->SetFullscreenState(false, NULL);

    m_device.Shutdown();
    
    m_renderContext.Release();

    return true;
}

void Renderer::SetGraphicsPipeline(GraphicsPipeline* pGraphicsPipeline)
{
    m_graphicsCommandCtx.CommandList()->SetPipelineState(pGraphicsPipeline->PSO());
}

void Renderer::SetRootSignature(RootSignature* pRootSignature)
{
    m_graphicsCommandCtx.CommandList()->SetGraphicsRootSignature(pRootSignature->Signature());
}

void Renderer::SetRenderContext(RenderContext* pRenderContext)
{
    ColorBuffer* pBuffer = pRenderContext->RTV();

    u32 frameIndex = m_graphicsCommandCtx.FrameIndex();

    // transition the rtv at the given frameIndex from present state to rtv state so we can draw to it
    m_graphicsCommandCtx.CommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pBuffer->Resource(frameIndex), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

    // get handle the rtv that we want to bind to the output merger
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = pRenderContext->RtvHeap()->CpuHandle(frameIndex);
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = pRenderContext->DsvHeap()->CpuHandle();

    // Assumes contiguous rtvs if multiple
    m_graphicsCommandCtx.CommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
}

void Renderer::SetRenderContext()
{
    SetRenderContext(&m_renderContext);
}

void Renderer::ClearRenderContext(RenderContext* pRenderContext)
{
    u32 frameIndex = m_graphicsCommandCtx.FrameIndex();

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = pRenderContext->RtvHeap()->CpuHandle(frameIndex);
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = pRenderContext->DsvHeap()->CpuHandle();

    ColorBuffer* pRtv = pRenderContext->RTV();
    DepthBuffer* pDsv = pRenderContext->DSV();

    m_graphicsCommandCtx.CommandList()->ClearRenderTargetView(rtvHandle, pRtv->Color(frameIndex), 0, nullptr);
    m_graphicsCommandCtx.CommandList()->ClearDepthStencilView(dsvHandle, pDsv->ClearFlags(), pDsv->ClearDepth(), pDsv->ClearStencil(), 0, nullptr);
}

void Renderer::ClearRenderContext()
{
    ClearRenderContext(&m_renderContext);
}

void Renderer::SetViewport()
{
    m_graphicsCommandCtx.CommandList()->RSSetViewports(1, &m_viewport);
    m_graphicsCommandCtx.CommandList()->RSSetScissorRects(1, &m_scissorRect);
}

void Renderer::UpdatePipeline(GraphicsPipeline* pGraphicsPipeline)
{
    if (!m_graphicsCommandCtx.WaitForFence())
    {
        m_running = false;
    }

    m_graphicsCommandCtx.SetFrameIndex(m_device.SwapChain3()->GetCurrentBackBufferIndex());

    m_graphicsCommandCtx.Reset(pGraphicsPipeline);
}

void Renderer::Prepare(Renderable* pRenderable)
{
    m_graphicsCommandCtx.CommandList()->IASetPrimitiveTopology(pRenderable->Topology());
    m_graphicsCommandCtx.CommandList()->IASetVertexBuffers(0, 1, pRenderable->VertexBufferView());
    m_graphicsCommandCtx.CommandList()->IASetIndexBuffer(pRenderable->IndexBufferView());
}


void Renderer::DrawIndexedInstanced(Renderable* pRenderable, u32 instanceCount, u32 startIndex, i32 baseVertex, u32 startInstance)
{
    m_graphicsCommandCtx.CommandList()->DrawIndexedInstanced(pRenderable->NumIndices(), instanceCount, startIndex, baseVertex, startInstance);
}

void Renderer::Present()
{
    if (!m_graphicsCommandCtx.Close())
    {
        m_running = false;
    }

    ExecuteGraphicsCommandContext();

    HRESULT hr = m_device.SwapChain3()->Present(0, 0);
    if (FAILED(hr))
    {
        m_running = false;
    }
}

void Renderer::ExecuteGraphicsCommandContext(GraphicsCommandContext* pGraphicsCommandCtx)
{
    m_running = pGraphicsCommandCtx->Execute(this);
}

void Renderer::ExecuteGraphicsCommandContext()
{
    m_running = m_graphicsCommandCtx.Execute(this);
}
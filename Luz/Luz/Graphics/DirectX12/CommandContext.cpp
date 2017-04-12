#include "stdafx.h"
#include "CommandContext.h"
#include "PipelineState.h"
#include "GpuResource.h"
#include "SwapChain.h"
#include "Device.h"
#include "RenderContext.h"
#include "Viewport.h"

using namespace dx12;

#pragma region CommandContext

CommandContext::CommandContext(u32 numAllocators) :
    m_allocatorIndex(0),
    m_commandAllocators(numAllocators, nullptr),
    m_device(nullptr),
    m_swapChain(nullptr)
{

}

CommandContext::~CommandContext()
{
    for (auto& allocator : m_commandAllocators)
    {
        SAFE_RELEASE(allocator);
    }
}

bool CommandContext::WaitForNextAllocator()
{
    bool running = WaitForFence(m_allocatorIndex);

    m_allocatorIndex++;
    if (m_allocatorIndex > m_commandAllocators.size() - 1)
    {
        m_allocatorIndex = 0;
    }

    return running;
}
#pragma endregion CommandContext

#pragma region GraphicsCommandContext

GraphicsCommandContext::GraphicsCommandContext(u32 numAllocators) : CommandContext(numAllocators), m_commandList(nullptr)
{
    
}

GraphicsCommandContext::~GraphicsCommandContext()
{
    SAFE_RELEASE(m_commandList);
}

void GraphicsCommandContext::WaitForAll()
{
    int idx;
    for (u32 i = 0, count = NumAllocators(); i < count; ++i)
    {
        idx = i;
        WaitForFence(idx);
    }
}

bool GraphicsCommandContext::Initialize(std::shared_ptr<const Device> pDevice, std::shared_ptr<const SwapChain> pSwapChain)
{
    m_device = pDevice;
    m_swapChain = pSwapChain;

    ID3D12Device* pDevice1 = m_device->DX();

    if (!CreateGraphicsCommandAllocators(pDevice1, m_commandAllocators.data(), (UINT)m_commandAllocators.size()))
    {
        return false;
    }

    if (!CreateGraphicsCommandList(pDevice1, m_commandAllocators[0], nullptr, &m_commandList))
    {
        return false;
    }

    FenceContext::Initialize(pDevice, (u32)m_commandAllocators.size());

    //for (int i = 0, num = pCommandList->NumAllocators(); i < num; ++i)
    //{
    //    hr = pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCommandList->m_commandAllocators[i]));
    //    if (FAILED(hr))
    //    {
    //        return false;
    //    }
    //}

    //// Create command list using first allocator
    //hr = pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandList->m_commandAllocators[0], nullptr, IID_PPV_ARGS(&pCommandList->m_commandList));
    //if (FAILED(hr))
    //{
    //    return false;
    //}

    //// Create fence
    //for (int i = 0, num = pCommandList->NumAllocators(); i < num; ++i)
    //{
    //    hr = pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pCommandList->m_fences[i]));
    //    if (FAILED(hr))
    //    {
    //        return false;
    //    }

    //    pCommandList->m_fenceValues[i] = 0;
    //}

    //pCommandList->m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    //if (pCommandList->m_fenceEvent == nullptr)
    //{
    //    return false;
    //}

    return true;
}

bool GraphicsCommandContext::Reset(GraphicsPipeline* pGraphicsPipeline)
{
    bool running = true;

    running = WaitForNextAllocator();

    HRESULT hr = m_commandAllocators[m_allocatorIndex]->Reset();
    if (FAILED(hr))
    {
        running = false;
    }

    hr = m_commandList->Reset(m_commandAllocators[m_allocatorIndex], (pGraphicsPipeline) ? pGraphicsPipeline->PSO() : nullptr);
    if (FAILED(hr))
    {
        running = false;
    }

    return running;
}

void GraphicsCommandContext::Set(GraphicsPipeline* pGraphicsPipeline)
{
    m_commandList->SetPipelineState(pGraphicsPipeline->PSO());
}

void GraphicsCommandContext::SetRootSignature(RootSignature* pRootSignature)
{
    m_commandList->SetGraphicsRootSignature(pRootSignature->Signature());
}

void GraphicsCommandContext::SetRenderContext(RenderContext* pRenderContext)
{
    u32 frameIndex = m_swapChain->GetCurrentBackBufferIndex();
    const ColorBuffer* pBuffer = pRenderContext->RTV(frameIndex);

    // transition the rtv at the given frameIndex from present state to rtv state so we can draw to it
    TransitionResource(m_commandList, pBuffer->Resource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

    // get handle the rtv that we want to bind to the output merger
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = pRenderContext->RtvHeap()->CpuHandle(frameIndex);
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = pRenderContext->DsvHeap()->CpuHandle();

    // Assumes contiguous rtvs if multiple
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
}

void GraphicsCommandContext::ClearRenderContext(RenderContext* pRenderContext)
{
    u32 frameIndex = m_swapChain->GetCurrentBackBufferIndex();
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = pRenderContext->RtvHeap()->CpuHandle(frameIndex);
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = pRenderContext->DsvHeap()->CpuHandle();

    const ColorBuffer* pRtv = pRenderContext->RTV(frameIndex);
    const DepthBuffer* pDsv = pRenderContext->DSV();

    m_commandList->ClearRenderTargetView(rtvHandle, pRtv->Color(), 0, nullptr);
    m_commandList->ClearDepthStencilView(dsvHandle, pDsv->ClearFlags(), pDsv->ClearDepth(), pDsv->ClearStencil(), 0, nullptr);
}

void GraphicsCommandContext::SetViewport(Viewport* pViewport)
{
    m_commandList->RSSetViewports(1, &pViewport->ViewportRect());
    m_commandList->RSSetScissorRects(1, &pViewport->ScissorRect());
}

bool GraphicsCommandContext::Close()
{
    HRESULT hr = m_commandList->Close();
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

void GraphicsCommandContext::FinalizeSwapChain()
{
    m_swapChain->Finalize(m_commandList);
}

void GraphicsCommandContext::SetGraphicsRootConstantBufferView(UploadBuffer* pBuffer, u32 paramIndex /*= 0*/)
{
    m_commandList->SetGraphicsRootConstantBufferView(paramIndex, pBuffer->RootConstantBufferView());
}
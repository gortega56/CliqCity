#include "stdafx.h"
#include "CommandContext.h"
#include "PipelineState.h"
#include "GpuResource.h"
#include "SwapChain.h"
#include "Device.h"
#include "RenderContext.h"
#include "Viewport.h"
#include "DescriptorHeap.h"
#include "dx12_internal.h"

using namespace Dx12;

#pragma region CommandContext

CommandContext::CommandContext() :
    m_commandAllocator(nullptr),
    m_device(nullptr),
    m_swapChain(nullptr)
{

}

CommandContext::~CommandContext()
{
    SAFE_RELEASE(m_commandAllocator);
}

#pragma endregion CommandContext

#pragma region GraphicsCommandContext

GraphicsCommandContext::GraphicsCommandContext() : CommandContext(), m_commandList(nullptr)
{
    
}

GraphicsCommandContext::~GraphicsCommandContext()
{
    SAFE_RELEASE(m_commandList);
}

bool GraphicsCommandContext::Initialize(std::shared_ptr<const Device> pDevice, std::shared_ptr<const SwapChain> pSwapChain)
{
    m_device = pDevice;
    m_swapChain = pSwapChain;

    ID3D12Device* pDevice1 = m_device->DX();

    if (!CreateGraphicsCommandAllocators(pDevice1, &m_commandAllocator, 1))
    {
        return false;
    }

    if (!CreateGraphicsCommandList(pDevice1, m_commandAllocator, nullptr, &m_commandList))
    {
        return false;
    }

    if (!m_fence.Initialize(pDevice))
    {
        return false;
    }

    return true;
}

bool GraphicsCommandContext::Reset(GraphicsPipeline* pGraphicsPipeline)
{
    bool running = true;

    HRESULT hr = m_commandAllocator->Reset();
    if (FAILED(hr))
    {
        running = false;
    }

    hr = m_commandList->Reset(m_commandAllocator, (pGraphicsPipeline) ? pGraphicsPipeline->PSO() : nullptr);
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

//void GraphicsCommandContext::SetDescriptorHeaps(std::vector<DescriptorHeap* const> descriptorHeaps)
//{
//    std::vector<ID3D12DescriptorHeap*> pHeaps;
//    pHeaps.reserve(descriptorHeaps.size());
//
//    std::transform(descriptorHeaps.begin(), descriptorHeaps.end(), pHeaps.begin(), [](const DescriptorHeap* const pHeap) { return pHeap->Native(); });
//    m_commandList->SetDescriptorHeaps((u32)pHeaps.size(), pHeaps.data());
//}

void GraphicsCommandContext::SetDescriptorHeap(const DescriptorHeap* pDescriptorHeap)
{
    ID3D12DescriptorHeap* descriptorHeaps[] = { pDescriptorHeap->Native() };
    m_commandList->SetDescriptorHeaps(1, descriptorHeaps);
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

void GraphicsCommandContext::SetGraphicsRootDescriptorTable(DescriptorHeap* pHeap, u32 paramIndex/* = 0*/)
{
    m_commandList->SetGraphicsRootDescriptorTable(paramIndex, pHeap->Native()->GetGPUDescriptorHandleForHeapStart());
}
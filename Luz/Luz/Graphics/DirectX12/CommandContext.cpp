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
#include "CommandQueue.h"

using namespace Dx12;

#pragma region CommandAllocator

CommandAllocator::CommandAllocator() : m_allocator(nullptr)
{

}

bool CommandAllocator::Initialize(D3D12_COMMAND_LIST_TYPE type)
{
    auto pDevice = Device::SharedInstance();

    if (!CreateCommandAllocators(pDevice->DX(), m_allocator.ReleaseAndGetAddressOf(), 1, type))
    {
        return false;
    }

    if (!m_fence.Initialize(pDevice))
    {
        return false;
    }

    return true;
}

#pragma endregion

#pragma region CommandAllocatorPool

static const D3D12_COMMAND_LIST_TYPE g_commandListTypes[] =
{
    D3D12_COMMAND_LIST_TYPE_DIRECT,
    D3D12_COMMAND_LIST_TYPE_BUNDLE,
    D3D12_COMMAND_LIST_TYPE_COMPUTE,
    D3D12_COMMAND_LIST_TYPE_COPY,
};

static const int g_numCommandListTypes = 4;
static CommandAllocatorPool g_allocatorPools[g_numCommandListTypes];
static std::mutex g_allocatorPoolMutex;

CommandAllocatorPool::CommandAllocatorPool()
{

}

bool CommandAllocatorPool::Initialize()
{
    std::lock_guard<std::mutex> lock(g_allocatorPoolMutex);

    for (int typeIndex = 0; typeIndex < g_numCommandListTypes; ++typeIndex)
    {
        D3D12_COMMAND_LIST_TYPE listType = g_commandListTypes[typeIndex];
        for (u32 i = 0; i < sm_maxAllocatorsPerType; ++i)
        {
            if (!g_allocatorPools[listType].m_allocators[i].Initialize(listType))
            {
                return false;
            }
        }
    }

    return true;
}

CommandAllocator* CommandAllocatorPool::Allocate(D3D12_COMMAND_LIST_TYPE type)
{
    std::lock_guard<std::mutex> lock(g_allocatorPoolMutex);

    auto& allocatorPool = g_allocatorPools[type];

    UINT64 min = UINT64_MAX;
    int idx = 0;

    CommandAllocator* pCommandAllocator = nullptr;

    // Just look for any ctx that we aren't waiting for
    for (int i = 0, count = (int)sm_maxAllocatorsPerType; i < count; ++i)
    {
        CommandAllocator* iter = &allocatorPool.m_allocators[i];
    //    std::cout << i << ":\tLast Complete: " << iter->GetFence().Completed() << " Signal: " << iter->GetFence().Signal() << " IsWaiting: " << iter->GetFence().IsWaiting() << std::endl;

        if (iter->GetFence().IsIdle())
        {
            pCommandAllocator = iter;
            break;
        }
        else if (iter->GetFence().IsWaiting())
        {
            UINT64 sig = iter->GetFence().Signal();
            if (sig < min)
            {
                min = sig;
                idx = i;
            }
        }
    }

    if (!pCommandAllocator)
    {
        pCommandAllocator = &allocatorPool.m_allocators[idx];

        // We know this guy WAS waiting... but we gotta check again...
        if (pCommandAllocator->GetFence().IsWaiting())
        {
            std::cout << "Last Complete: " << pCommandAllocator->GetFence().Completed() << " Signal: " << pCommandAllocator->GetFence().Signal() << std::endl;
            pCommandAllocator->GetFence().Wait();
        }

        HRESULT hr = pCommandAllocator->Allocator()->Reset();
        LUZASSERT(SUCCEEDED(hr));
    }

    // Increment so we know it's in use
    pCommandAllocator->GetFence().IncrementSignal();
    
    return pCommandAllocator;
}

void CommandAllocatorPool::WaitAll()
{
    for (int typeIndex = 0; typeIndex < g_numCommandListTypes; ++typeIndex)
    {
        D3D12_COMMAND_LIST_TYPE listType = g_commandListTypes[typeIndex];
        for (u32 i = 0; i < sm_maxAllocatorsPerType; ++i)
        {
            g_allocatorPools[listType].m_allocators[i].GetFence().Wait();
        }
    }
}

#pragma endregion


#pragma region CommandContext

CommandContext::CommandContext() :
    m_commandAllocator(nullptr),
    m_commandQueue(nullptr)
{

}

CommandContext::~CommandContext()
{

}

#pragma endregion

#pragma region GraphicsCommandContext

static std::shared_ptr<CommandQueue> g_graphicsCommandQueue = nullptr;

std::shared_ptr<CommandQueue> GraphicsCommandContext::GlobalQueue()
{
    if (!g_graphicsCommandQueue)
    {
        g_graphicsCommandQueue = CommandQueue::CreateGraphicsQueue();
    }

    return g_graphicsCommandQueue;
}

static std::shared_ptr<SwapChain> g_swapChain = nullptr;

void GraphicsCommandContext::SetGlobalSwapChain(std::shared_ptr<SwapChain> pSwapChain)
{
    if (!g_swapChain)
    {
        g_swapChain = pSwapChain;
    }
}

std::shared_ptr<GraphicsCommandContext> GraphicsCommandContext::Create()
{
    auto ctx = std::make_shared<GraphicsCommandContext>();
    bool success = ctx->Initialize();
    LUZASSERT(success);
    return ctx;
}

GraphicsCommandContext::GraphicsCommandContext() : CommandContext(), m_commandList(nullptr)
{
    
}

GraphicsCommandContext::~GraphicsCommandContext()
{

}

bool GraphicsCommandContext::Initialize()
{
    m_commandQueue = GlobalQueue();
    m_swapChain = g_swapChain;

    m_commandAllocator = CommandAllocatorPool::Allocate(D3D12_COMMAND_LIST_TYPE_DIRECT);

    if (!CreateGraphicsCommandList(Device::SharedInstance()->DX(), m_commandAllocator->Allocator(), nullptr, m_commandList.ReleaseAndGetAddressOf()))
    {
        return false;
    }

    return true;
}

bool GraphicsCommandContext::Reset(GraphicsPipeline* pGraphicsPipeline)
{
    bool running = true;

    // TODO: temp do this better.... init and reset both allocate... but we should know something more here...
    if (!m_commandAllocator)
    {
        m_commandAllocator = CommandAllocatorPool::Allocate(D3D12_COMMAND_LIST_TYPE_DIRECT);
        LUZASSERT(m_commandAllocator->GetFence().InUse());
    }

    m_commandList->SetPipelineState((pGraphicsPipeline) ? pGraphicsPipeline->PSO() : nullptr);

    //HRESULT hr = m_commandList->Reset(m_commandAllocator->Allocator(), (pGraphicsPipeline) ? pGraphicsPipeline->PSO() : nullptr);
    /*if (FAILED(hr))
    {
        running = false;
    }*/

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

void GraphicsCommandContext::SetDescriptorHeaps(const DescriptorHeap* pDescriptorHeaps, u32 numHeaps)
{
    //std::vector<ID3D12DescriptorHeap*> descriptorHeaps;
    //descriptorHeaps.reserve(numHeaps);
    //std::transform(descriptorHeaps.begin(), descriptorHeaps.end(), pDescriptorHeaps, [](const DescriptorHeap& const pHeap) { return pHeap.Native(); });
    //m_commandList->SetDescriptorHeaps(numHeaps, descriptorHeaps.data());
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
    //u32 frameIndex = m_swapChain->GetCurrentBackBufferIndex();
    //const ColorBuffer* pBuffer = pRenderContext->RTV(frameIndex);

    //// transition the rtv at the given frameIndex from present state to rtv state so we can draw to it
    //TransitionResource(m_commandList, pBuffer->Resource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

    //// get handle the rtv that we want to bind to the output merger
    //CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = pRenderContext->RtvHeap()->CpuHandle(frameIndex);
    ////auto rtvHandle = pBuffer->RenderTargetViewHandle().CpuHandle();

    //CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = pRenderContext->DsvHeap()->CpuHandle();

    //// Assumes contiguous rtvs if multiple
    //m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
}

void GraphicsCommandContext::ClearRenderContext(RenderContext* pRenderContext)
{
    //u32 frameIndex = m_swapChain->GetCurrentBackBufferIndex();
    //CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = pRenderContext->RtvHeap()->CpuHandle(frameIndex);
    //CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = pRenderContext->DsvHeap()->CpuHandle();

    //const ColorBuffer* pRtv = pRenderContext->RTV(frameIndex);
    //const DepthBuffer* pDsv = pRenderContext->DSV();

    //m_commandList->ClearRenderTargetView(rtvHandle, pRtv->Color(), 0, nullptr);
    //m_commandList->ClearDepthStencilView(dsvHandle, pDsv->ClearFlags(), pDsv->ClearDepth(), pDsv->ClearStencil(), 0, nullptr);
}

void GraphicsCommandContext::SetRenderContext(SwapChainContext* pRenderContext)
{
    u32 frameIndex = pRenderContext->m_swapChain->GetCurrentBackBufferIndex();
    const ColorBuffer& colorBuffer = pRenderContext->m_colorBuffers[frameIndex];

    // transition the rtv at the given frameIndex from present state to rtv state so we can draw to it
    TransitionResource(m_commandList.Get(), colorBuffer.Resource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

    // get handle the rtv that we want to bind to the output merger
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = colorBuffer.RenderTargetViewHandle().CpuHandle();
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = pRenderContext->DepthStencilView().CpuHandle();

    // Assumes contiguous rtvs if multiple
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
}

void GraphicsCommandContext::ClearRenderContext(SwapChainContext* pRenderContext)
{
    u32 frameIndex = pRenderContext->m_swapChain->GetCurrentBackBufferIndex();
    const ColorBuffer& colorBuffer = pRenderContext->m_colorBuffers[frameIndex];
    const DepthBuffer& depthBuffer = pRenderContext->m_depthBuffer;

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = colorBuffer.RenderTargetViewHandle().CpuHandle();
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthBuffer.DepthStencilViewHandle().CpuHandle();

    m_commandList->ClearRenderTargetView(rtvHandle, colorBuffer.Color(), 0, nullptr);
    m_commandList->ClearDepthStencilView(dsvHandle, depthBuffer.ClearFlags(), depthBuffer.ClearDepth(), depthBuffer.ClearStencil(), 0, nullptr);
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
    m_swapChain->Finalize(m_commandList.Get());
}

void GraphicsCommandContext::SetGraphicsRootConstantBufferView(const UploadBuffer* pBuffer, u32 paramIndex /*= 0*/)
{
    m_commandList->SetGraphicsRootConstantBufferView(paramIndex, pBuffer->RootConstantBufferView());
}

void GraphicsCommandContext::SetGraphicsRootDescriptorTable(const DescriptorHandle& handle, u32 paramIndex /*= 0*/)
{
    m_commandList->SetGraphicsRootDescriptorTable(paramIndex, handle.GpuHandle());
}

void GraphicsCommandContext::Execute(bool wait /*= false*/)
{
    auto pCmdList = m_commandList.Get();

    // Close the command list
    HRESULT hr = m_commandList->Close();
    if (FAILED(hr))
    {
        __debugbreak();
    }

    // Create array of command lists
    ID3D12CommandList* ppCommandLists[] = { pCmdList };

    // execute command lists
    auto pQueue = m_commandQueue->GetQueue();
    pQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Signal (fence was incremented on the allocator when we first created the context)
    auto& fence = m_commandAllocator->GetFence();

    fence.IncrementSignal();
    //std::cout << "Last Complete: " << fence.Completed() << " Signal: " << fence.Signal() << std::endl;

    hr = pQueue->Signal(fence.Ptr(), fence.Signal());

    if (FAILED(hr))
    {
        __debugbreak();
    }

    if (wait)
    {
        fence.Wait();
    }
}

void GraphicsCommandContext::Present(bool wait /*= false*/)
{
    // Transition back buffer to present
    m_swapChain->Finalize(m_commandList.Get());

    // Close / Execute / Signal
    Execute(wait);

    // Present
    m_swapChain->Present();
}

#pragma endregion
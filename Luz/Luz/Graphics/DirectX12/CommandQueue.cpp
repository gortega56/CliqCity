#include "stdafx.h"
#include "CommandQueue.h"
#include "Device.h"
#include "CommandContext.h"
#include "SwapChain.h"

using namespace dx12;

CommandQueue::CommandQueue() : m_graphicsCommandQueue(nullptr), m_computeCommandQueue(nullptr)
{

}

CommandQueue::~CommandQueue()
{
    SAFE_RELEASE(m_graphicsCommandQueue);
    SAFE_RELEASE(m_computeCommandQueue);
}

bool CommandQueue::Initialize(std::shared_ptr<const Device> pDevice)
{
    if (!CreateCommandQueue(pDevice->DX(), &m_graphicsCommandQueue, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_QUEUE_FLAG_NONE, D3D12_COMMAND_QUEUE_PRIORITY_NORMAL))
    {
        return false;
    }

    if (!CreateCommandQueue(pDevice->DX(), &m_computeCommandQueue, D3D12_COMMAND_LIST_TYPE_COMPUTE, D3D12_COMMAND_QUEUE_FLAG_NONE, D3D12_COMMAND_QUEUE_PRIORITY_NORMAL))
    {
        return false;
    }

    return true;
}

bool CommandQueue::Signal(std::shared_ptr<SwapChain> pSwapChain, bool wait /*= false*/) const
{
    HRESULT hr = m_graphicsCommandQueue->Signal(pSwapChain->GetFence(), pSwapChain->GetFenceValue());
    if (FAILED(hr))
    {
        return false;
    }

    if (wait)
    {
        pSwapChain->WaitForPreviousFrame();
    }

    return true;
}


bool CommandQueue::Signal(std::shared_ptr<GraphicsCommandContext> pCtx, bool wait /*= false*/) const
{
    // add signal to command queue so the fence value gets updated
    HRESULT hr = m_graphicsCommandQueue->Signal(pCtx->GetFence()->Ptr(), pCtx->GetFence()->Signal());
    if (FAILED(hr))
    {
        return false;
    }

    if (wait)
    {
        pCtx->GetFence()->Wait();
        pCtx->Reset();
    }

    return true;
}

bool CommandQueue::Execute(std::shared_ptr<GraphicsCommandContext> pCtx, bool wait /*= false*/) const
{
    if (!pCtx->Close())
    {
        return false;
    }

    // Create array of command lists
    ID3D12CommandList* ppCommandLists[] = { pCtx->m_commandList };

    // execute command lists
    m_graphicsCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    return Signal(pCtx, wait);
}
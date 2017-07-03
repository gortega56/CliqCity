#include "stdafx.h"
#include "dx12_internal.h"
#include "CommandQueue.h"
#include "Device.h"
#include "CommandContext.h"
#include "SwapChain.h"

using namespace Dx12;

#pragma region CommandQueue

std::shared_ptr<CommandQueue> CommandQueue::CreateGraphicsQueue()
{
    auto queue = std::make_shared<CommandQueue>();
    bool success = queue->Initialize(D3D12_COMMAND_LIST_TYPE_DIRECT);
    LUZASSERT(success);
    return queue;
}

std::shared_ptr<CommandQueue> CommandQueue::CreateComputeQueue()
{
    auto queue = std::make_shared<CommandQueue>();
    bool success = queue->Initialize(D3D12_COMMAND_LIST_TYPE_COMPUTE);
    LUZASSERT(success);
    return queue;
}

CommandQueue::CommandQueue() : m_commandQueue(nullptr)
{

}

CommandQueue::~CommandQueue()
{
    SAFE_RELEASE(m_commandQueue);
}

bool CommandQueue::Initialize(D3D12_COMMAND_LIST_TYPE type, D3D12_COMMAND_QUEUE_FLAGS flags /*= D3D12_COMMAND_QUEUE_FLAG_NONE*/, D3D12_COMMAND_QUEUE_PRIORITY priority /*= D3D12_COMMAND_QUEUE_PRIORITY_NORMAL*/)
{
    if (!CreateCommandQueue(Device::SharedInstance()->DX(), &m_commandQueue, type, flags, priority))
    {
        return false;
    }

    return true;
}

//bool CommandQueue::Signal(std::shared_ptr<GraphicsCommandContext> pCtx, bool wait /*= false*/) const
//{
//    // add signal to command queue so the fence value gets updated
//    auto pFence = pCtx->GetFence();
//    pFence->IncrementSignal();
//
//    HRESULT hr = m_commandQueue->Signal(pFence->Ptr(), pFence->Signal());
//    if (FAILED(hr))
//    {
//        return false;
//    }
//
//    if (wait)
//    {
//        pCtx->GetFence()->Wait();
//        pCtx->Reset();
//    }
//
//    return true;
//}

//bool CommandQueue::Execute(std::shared_ptr<GraphicsCommandContext> pCtx, bool wait /*= false*/) const
//{
//    if (!pCtx->Close())
//    {
//        return false;
//    }
//
//    // Create array of command lists
//    ID3D12CommandList* ppCommandLists[] = { pCtx->m_commandList };
//
//    // execute command lists
//    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
//
//    return Signal(pCtx, wait);
//}

#pragma endregion

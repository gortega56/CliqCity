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

#pragma endregion

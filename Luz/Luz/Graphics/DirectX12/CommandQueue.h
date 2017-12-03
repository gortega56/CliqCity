#pragma once
#ifndef DX12_COMMANDQUEUE_H
#define DX12_COMMANDQUEUE_H

#ifndef __D3DX12_H__
#include "d3dx12.h"
#endif

namespace Dx12
{
    class Device;
    class SwapChain;
    class CommandContext;
    class GraphicsCommandContext;

    class CommandQueue
    {
    public:
        /* 
        /   MSDN: Any thread may submit a command list to any command queue at any time, 
        /   and the runtime will automatically serialize submission of the command 
        /   list in the command queue while preserving the submission order.
        /
        /   For right now just making global compute and graphics queues.
        */

        static std::shared_ptr<CommandQueue> CreateGraphicsQueue();
        static std::shared_ptr<CommandQueue> CreateComputeQueue();

        CommandQueue();
        ~CommandQueue();

        ID3D12CommandQueue* GetQueue() const { return m_commandQueue; }

        bool Initialize(D3D12_COMMAND_LIST_TYPE type, 
            D3D12_COMMAND_QUEUE_FLAGS flags = D3D12_COMMAND_QUEUE_FLAG_NONE, 
            D3D12_COMMAND_QUEUE_PRIORITY priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL);

    private:
        ID3D12CommandQueue* m_commandQueue;

        CommandQueue(const CommandQueue& other) = delete;
    };
}

#endif
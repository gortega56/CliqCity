#pragma once
#ifndef DX12_COMMANDQUEUE_H
#define DX12_COMMANDQUEUE_H

#ifndef DX12_INTERNAL_H
#include "dx12_internal.h"
#endif



namespace dx12
{
    class Device;
    class SwapChain;
    class CommandContext;
    class GraphicsCommandContext;

    class CommandQueue
    {
    public:
        CommandQueue();
        ~CommandQueue();

        ID3D12CommandQueue* GraphiceQueue() const { return m_graphicsCommandQueue; }
        ID3D12CommandQueue* ComputeQueue() const { return m_computeCommandQueue; }

        bool Initialize(std::shared_ptr<const Device> pDevice);

        bool Signal(std::shared_ptr<SwapChain> pSwapChain, bool wait = false) const;
        bool Signal(std::shared_ptr<GraphicsCommandContext> pCtx, bool wait = false) const;
        bool Execute(std::shared_ptr<GraphicsCommandContext> pCtx, bool wait = false) const;

    private:
        ID3D12CommandQueue* m_graphicsCommandQueue;
        ID3D12CommandQueue* m_computeCommandQueue;

        CommandQueue(const CommandQueue& other) = delete;
    };
}

#endif
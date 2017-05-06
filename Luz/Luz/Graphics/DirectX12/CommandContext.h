#pragma once
#ifndef DX12COMMANDCONTEXT_H
#define DX12COMMANDCONTEXT_H

#ifndef DX12_INTERNAL_H
#include "dx12_internal.h"
#endif

#ifndef DX12_FENCE_H
#include "Fence.h"
#endif

#include <vector>

namespace dx12
{
    class Renderer;
    class Pipeline;
    class GraphicsPipeline;
    class RootSignature;
    class RenderContext;
    class UploadBuffer;
    class Device;
    class Viewport;
    class SwapChain;

    class CommandContext
    {
    public:
        CommandContext();
        ~CommandContext();

        ID3D12CommandAllocator* CommandAllocator() const { return m_commandAllocator; }

        Fence* GetFence() { return &m_fence; }
        std::shared_ptr<const Device> GetDevice() const { return m_device; }
        std::shared_ptr<const SwapChain> GetSwapChain() const { return m_swapChain; }

        template<class CONTEXT>
        static std::shared_ptr<CONTEXT> GetNextAvailable(std::vector<std::shared_ptr<CONTEXT>>& contexts);
    
        template<class CONTEXT>
        static void WaitForAll(std::vector<std::shared_ptr<CONTEXT>>& contexts);

    protected:
        friend class CommandQueue;

        Fence m_fence;
        
        ID3D12CommandAllocator* m_commandAllocator;

        std::shared_ptr<const Device> m_device;
        std::shared_ptr<const SwapChain> m_swapChain;
    };

    class GraphicsCommandContext : public CommandContext
    {
    public:
        GraphicsCommandContext();
        ~GraphicsCommandContext();

        bool Initialize(std::shared_ptr<const Device> pDevice, std::shared_ptr<const SwapChain> pSwapChain);

        ID3D12GraphicsCommandList* CommandList() const { return m_commandList; }

        bool Reset(GraphicsPipeline* pGraphicsPipeline = nullptr);
        void Set(GraphicsPipeline* pGraphicsPipeline);

        void SetRootSignature(RootSignature* pRootSignature);

        void SetRenderContext(RenderContext* pRenderContext);
        void ClearRenderContext(RenderContext* pRenderContext);

        void SetViewport(Viewport* pViewport);

        void SetGraphicsRootConstantBufferView(UploadBuffer* pBuffer, u32 paramIndex = 0);
        
        bool Close();

        void FinalizeSwapChain();
        
    protected:
        friend class CommandQueue;
        friend class GpuBuffer;
        friend class Renderer;

    private:
        ID3D12GraphicsCommandList* m_commandList;
    };

    template<class CONTEXT>
    std::shared_ptr<CONTEXT> CommandContext::GetNextAvailable(std::vector<std::shared_ptr<CONTEXT>>& contexts)
    {
        // Round robin
        //static u32 numContexts = (u32)contexts.size();
        //static u32 idx = 0;

        //if (idx >= numContexts)
        //{
        //    idx = 0;
        //}

        //return contexts[idx++];

        // Just look for any ctx that we aren't waiting for
        for (auto& ctx : contexts)
        {
            if (!ctx->GetFence()->IsWaiting())
            {
                return ctx;
            }
        }

        // look for the one with the lowest signal;
        UINT64 min = UINT64_MAX;
        int idx = 0;
        for (int i = 0, count = (int)contexts.size(); i < count; ++i)
        {
            UINT64 sig = contexts[i]->GetFence()->Signal();
            if (sig < min)
            {
                min = sig;
                idx = i;
            }
        }

        contexts[idx]->GetFence()->Wait();

        return contexts[idx];
    }

    template<class CONTEXT>
    void CommandContext::WaitForAll(std::vector<std::shared_ptr<CONTEXT>>& contexts)
    {
        for (auto& ctx : contexts)
        {
            ctx->GetFence()->Wait();
        }
    }

}

#endif
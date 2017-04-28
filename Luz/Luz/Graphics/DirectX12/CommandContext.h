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

        UINT64 m_lastFenceValue;
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
        // Context with the lowest fence value should be the one closest to finishing
        //std::sort(contexts.begin(), contexts.end(), [](const std::shared_ptr<CONTEXT>& lhs, const std::shared_ptr<CONTEXT>& rhs)
        //{
        //    return (lhs->GetFence()->Value() > rhs->GetFence()->Value());
        //});

        static u32 numContexts = (u32)contexts.size();
        static u32 idx = 0;

        if (idx >= numContexts)
        {
            idx = 0;
        }

        return contexts[idx++];

        //for (u32 i = 0; i < (u32)contexts.size(); ++i)
        //{
        //    Fence* pFence = contexts[i]->GetFence();
        //    if (pFence->IsAvailable())
        //    {
        //       // std::cout << "GfxCtx: " << i << std::endl;

        //        return contexts[i];
        //    }
        //}

        //u32 idx = 0; u64 min = UINT64_MAX;
        //for (u32 i = 0; i < (u32)contexts.size(); ++i)
        //{
        //    Fence* pFence = contexts[i]->GetFence();
        //    u64 val = pFence->Signal();
        //    if (val < min)
        //    {
        //        min = val;
        //        idx = i;
        //    }
        //}

        //std::cout << "WAIT GfxCtx: " << idx << std::endl;
        //contexts[idx]->GetFence()->Wait();

        //return contexts[idx];
    }

    template<class CONTEXT>
    void CommandContext::WaitForAll(std::vector<std::shared_ptr<CONTEXT>>& contexts)
    {
        // Context with the lowest fence value should be the one closest to finishing

        for (auto& ctx : contexts)
        {
            ctx->GetFence()->Wait();
        }
    }

}

#endif
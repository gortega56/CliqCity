#pragma once
#ifndef DX12_COMMANDCONTEXT_H
#define DX12_COMMANDCONTEXT_H

#ifndef DX12_FENCE_H
#include "Fence.h"
#endif

#ifndef __DX12__
#include "d3dx12.h"
#endif

//struct ID3D12CommandQueue;
//struct ID3D12CommandAllocator;
//struct ID3D12CommandList;
//struct ID3D12GraphicsCommandList;

namespace Dx12
{
    class Renderer;
    class Pipeline;
    class GraphicsPipeline;
    class RootSignature;
    class DescriptorHeap;
    class DescriptorHandle;
    class RenderContext;
    class UploadBuffer;
    class Device;
    class Viewport;
    class SwapChain;
    class SwapChainContext;

    class CommandAllocator
    {
    public:
        CommandAllocator();
        ~CommandAllocator() = default;

        bool Initialize(D3D12_COMMAND_LIST_TYPE type);

        inline Microsoft::WRL::ComPtr<ID3D12CommandAllocator> SharedPtr() const { return m_allocator; }
        inline ID3D12CommandAllocator* Allocator() const{ return m_allocator.Get(); }
        
        inline Fence& GetFence() { return m_fence; }

    private:
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_allocator;
        Fence m_fence;
    };

    class CommandAllocatorPool
    {
    public:
        static const u32 sm_maxAllocatorsPerType = 16;
        
        static bool Initialize();
        static CommandAllocator* Allocate(D3D12_COMMAND_LIST_TYPE type);
        static void WaitAll();

        CommandAllocatorPool();
        ~CommandAllocatorPool() = default;
    private:

        u32 m_remainingAllocators;
        CommandAllocator m_allocators[sm_maxAllocatorsPerType];
    };

    class CommandContext
    {
    public:
        CommandContext();
        ~CommandContext();

        //template<class CONTEXT>
        //static int GetNextAvailable(const std::vector<std::shared_ptr<CONTEXT>>& contexts);
    
        //template<class CONTEXT>
        //static void WaitForAll(std::vector<std::shared_ptr<CONTEXT>>& contexts);

    protected:
        friend class CommandQueue;
        
        CommandAllocator* m_commandAllocator;
        std::shared_ptr<CommandQueue> m_commandQueue;
    };

    class GraphicsCommandContext : public CommandContext
    {
    public:
        static std::shared_ptr<CommandQueue> GlobalQueue();
        static void SetGlobalSwapChain(std::shared_ptr<SwapChain> pSwapChain);

        static std::shared_ptr<GraphicsCommandContext> Create();

        GraphicsCommandContext();
        ~GraphicsCommandContext();

        bool Initialize();

        ID3D12GraphicsCommandList* CommandList() const { return m_commandList.Get(); }

        bool Reset(GraphicsPipeline* pGraphicsPipeline = nullptr);
        void Set(GraphicsPipeline* pGraphicsPipeline);

        void SetRootSignature(RootSignature* pRootSignature);

        void SetDescriptorHeaps(const DescriptorHeap* pDescriptorHeaps, u32 numHeaps);

        void SetDescriptorHeap(const DescriptorHeap* pDescriptorHeap);
        void SetDescriptorHeap(std::shared_ptr<const DescriptorHeap> pDescriptorHeap) { return SetDescriptorHeap(pDescriptorHeap.get()); }


        void SetRenderContext(RenderContext* pRenderContext);
        void ClearRenderContext(RenderContext* pRenderContext);

        void SetRenderContext(SwapChainContext* pRenderContext);
        void ClearRenderContext(SwapChainContext* pRenderContext);

        void SetViewport(Viewport* pViewport);

        void SetGraphicsRootConstantBufferView(const UploadBuffer* pBuffer, u32 paramIndex = 0);
        
        void SetGraphicsRootDescriptorTable(const DescriptorHandle& handle, u32 paramIndex = 0);

        bool Close();

        void FinalizeSwapChain();

        void Execute(bool wait = false);
        void Present(bool wait = false);
        
    protected:
        friend class CommandQueue;
        friend class GpuBuffer;
        friend class Renderer;

    private:
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
        std::shared_ptr<SwapChain> m_swapChain;
    };

    //template<class CONTEXT>
    //int CommandContext::GetNextAvailable(const std::vector<std::shared_ptr<CONTEXT>>& contexts)
    //{
    //    UINT64 min = UINT64_MAX;
    //    int idx = 0;

    //    // Just look for any ctx that we aren't waiting for
    //    for (int i = 0, count = (int)contexts.size(); i < count; ++i)
    //    {
    //        if (!contexts[i]->GetFence()->IsWaiting())
    //        {
    //            return i;
    //        }
    //        else
    //        {
    //            UINT64 sig = contexts[i]->GetFence()->Signal();
    //            if (sig < min)
    //            {
    //                min = sig;
    //                idx = i;
    //            }
    //        }
    //    }

    //    contexts[idx]->GetFence()->Wait();

    //    return idx;
    //}

    //template<class CONTEXT>
    //void CommandContext::WaitForAll(std::vector<std::shared_ptr<CONTEXT>>& contexts)
    //{
    //    for (auto& ctx : contexts)
    //    {
    //        ctx->GetFence()->Wait();
    //    }
    //}

}

#endif
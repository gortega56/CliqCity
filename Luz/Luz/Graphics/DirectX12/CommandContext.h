#pragma once
#ifndef DX12_COMMANDCONTEXT_H
#define DX12_COMMANDCONTEXT_H

#ifndef DX12_FENCE_H
#include "Fence.h"
#endif

#ifndef __DX12__
#include "d3dx12.h"
#endif

namespace Dx12
{
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
        static void Destroy();
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

        static void Initialize();

    protected:
        friend class CommandQueue;
        
        CommandAllocator* m_commandAllocator;
        std::shared_ptr<CommandQueue> m_commandQueue;
    };

    class LUZ_API GraphicsCommandContext : public CommandContext
    {
    public:
        static std::shared_ptr<GraphicsCommandContext> Create(std::shared_ptr<CommandQueue> pCommandQueue = nullptr);

        GraphicsCommandContext();
        ~GraphicsCommandContext();

        bool Initialize(std::shared_ptr<CommandQueue> pCommandQueue);

        ID3D12GraphicsCommandList* CommandList() const { return m_commandList.Get(); }

        bool Reset(GraphicsPipeline* pGraphicsPipeline = nullptr);
        void Set(GraphicsPipeline* pGraphicsPipeline);

        void SetRootSignature(RootSignature* pRootSignature);

        void SetDescriptorHeaps(const DescriptorHeap* pDescriptorHeaps, u32 numHeaps);

        void SetDescriptorHeap(const DescriptorHeap* pDescriptorHeap);
        void SetDescriptorHeap(std::shared_ptr<const DescriptorHeap> pDescriptorHeap) { return SetDescriptorHeap(pDescriptorHeap.get()); }

        void SetRenderContext();
        void ClearRenderContext();

        void SetRenderContext(SwapChainContext* pRenderContext);
        void ClearRenderContext(SwapChainContext* pRenderContext);
        
        void SetRenderContext(RenderContext* pRenderContext);
        void ClearRenderContext(RenderContext* pRenderContext);

        void SetViewport();
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

    private:
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
        std::shared_ptr<SwapChain> m_swapChain;
    };
}

#endif
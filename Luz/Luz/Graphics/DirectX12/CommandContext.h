#pragma once
#ifndef DX12COMMANDCONTEXT_H
#define DX12COMMANDCONTEXT_H

#ifndef DX12_INTERNAL_H
#include "dx12_internal.h"
#endif

#ifndef DX12_FENCECONTEXT_H
#include "FenceContext.h"
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

    class CommandContext : public FenceContext
    {
    public:
        CommandContext(u32 numAllocators);
        CommandContext() : CommandContext(1) {}
        ~CommandContext();

        ID3D12CommandAllocator* CommandAllocator(i32 i) const { return m_commandAllocators[i]; }
        u32 AllocatorIndex() const { return m_allocatorIndex; }
        u32 NumAllocators() const { return (u32)m_commandAllocators.size(); }

        bool WaitForNextAllocator();

        std::shared_ptr<const Device> GetDevice() const { return m_device; }
        std::shared_ptr<const SwapChain> GetSwapChain() const { return m_swapChain; }

    protected:
        friend class CommandQueue;

        u32 m_allocatorIndex;
        std::vector<ID3D12CommandAllocator*> m_commandAllocators;

        std::shared_ptr<const Device> m_device;
        std::shared_ptr<const SwapChain> m_swapChain;
    };

    class GraphicsCommandContext : public CommandContext
    {
    public:
        GraphicsCommandContext(u32 numAllocators);
        GraphicsCommandContext() : GraphicsCommandContext(1) {}
        ~GraphicsCommandContext();

        bool Initialize(std::shared_ptr<const Device> pDevice, std::shared_ptr<const SwapChain> pSwapChain);

        bool Reset(GraphicsPipeline* pGraphicsPipeline = nullptr);
        void Set(GraphicsPipeline* pGraphicsPipeline);

        void SetRootSignature(RootSignature* pRootSignature);

        void SetRenderContext(RenderContext* pRenderContext);
        void ClearRenderContext(RenderContext* pRenderContext);

        void SetViewport(Viewport* pViewport);

        void SetGraphicsRootConstantBufferView(UploadBuffer* pBuffer, u32 paramIndex = 0);
        
        bool Close();

        void FinalizeSwapChain();

        void WaitForAll();
        
    protected:
        friend class CommandQueue;
        friend class GpuBuffer;
        friend class Renderer;

        ID3D12Fence* Fence() const { return FenceContext::Fence(m_allocatorIndex); }
        UINT64 FenceValue() const { return FenceContext::FenceValue(m_allocatorIndex); }

        ID3D12GraphicsCommandList* CommandList() const { return m_commandList; }

    private:
        ID3D12GraphicsCommandList* m_commandList;
    };
}

#endif
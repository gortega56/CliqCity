#pragma once
#ifndef DX12COMMANDCONTEXT_H
#define DX12COMMANDCONTEXT_H

#ifndef DX12_INTERNAL_H
#include "dx12_internal.h"
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
        CommandContext(u32 numAllocators);
        CommandContext() : CommandContext(1) {}
        ~CommandContext();

        ID3D12CommandAllocator* CommandAllocator(int i) { return m_commandAllocators[i]; }

        ID3D12Fence* Fence(int i) { return m_fences[i]; }
        UINT64* FenceValueAddress(int i) { return &m_fenceValues[i]; }
        UINT64 FenceValue(int i) { return m_fenceValues[i]; }

        void IncrementFenceValue(int i) { m_fenceValues[i]++; }

        bool WaitForFence(int i);

        std::shared_ptr<const Device> GetDevice() const { return m_device; }
        std::shared_ptr<const SwapChain> GetSwapChain() const { return m_swapChain; }

    protected:
        friend class CommandQueue;

        std::vector<ID3D12CommandAllocator*> m_commandAllocators;
        std::vector<ID3D12Fence*> m_fences;
        std::vector<UINT64> m_fenceValues;
        HANDLE m_fenceEvent;

        std::shared_ptr<const Device> m_device;
        std::shared_ptr<const SwapChain> m_swapChain;
    };

    class GraphicsCommandContext : public CommandContext
    {
    public:
        GraphicsCommandContext(u32 numAllocators, u32 numFrameBuffers);
        GraphicsCommandContext() : GraphicsCommandContext(1, 3) {}
        ~GraphicsCommandContext();

        bool Initialize(std::shared_ptr<const Device> pDevice, std::shared_ptr<const SwapChain> pSwapChain);

        bool Reset(GraphicsPipeline* pGraphicsPipeline = nullptr);
        void Set(GraphicsPipeline* pGraphicsPipeline);

        void SetRootSignature(RootSignature* pRootSignature);

        void SetRenderContext(RenderContext* pRenderContext);
        void ClearRenderContext(RenderContext* pRenderContext);

        void SetViewport(Viewport* pViewport);

        bool Close();

        void PrepareBackBuffer();

        void WaitForAll();

        ID3D12CommandAllocator* CommandAllocator() { return m_commandAllocators[m_frameIndex]; }

        ID3D12Fence* Fence() { return m_fences[m_frameIndex]; }
        UINT64* FenceValueAddress() { return &m_fenceValues[m_frameIndex]; }
        UINT64 FenceValue() { return m_fenceValues[m_frameIndex]; }
        
        ID3D12GraphicsCommandList* CommandList() const { return m_commandList; }

        void IncrementFenceValue() { m_fenceValues[m_frameIndex]++; }
        bool WaitForPreviousFrame();

        u32 NumFrameBuffers() { return m_numFrameBuffers; }
        u32 FrameIndex() { return m_frameIndex; }
        void SetFrameIndex(u32 i) { m_frameIndex = i; }

        void SetGraphicsRootConstantBufferView(UploadBuffer* pBuffer, u32 paramIndex = 0);

    protected:
        friend class CommandQueue;

        ID3D12GraphicsCommandList* m_commandList;

    private:
        u32 m_frameIndex;
        u32 m_numFrameBuffers;
    };
}

#endif
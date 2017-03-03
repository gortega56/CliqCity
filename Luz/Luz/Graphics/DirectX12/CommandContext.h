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

    class CommandContext
    {
    public:
        CommandContext(u32 numThreads);
        CommandContext() : CommandContext(1) {}
        ~CommandContext();

        u32 NumThreads() { return m_numThreads; }
        ID3D12CommandAllocator* CommandAllocator(int i) { return m_commandAllocators[i]; }

        ID3D12Fence* Fence(int i) { return m_fences[i]; }
        UINT64* FenceValueAddress(int i) { return &m_fenceValues[i]; }
        UINT64 FenceValue(int i) { return m_fenceValues[i]; }

        void IncrementFenceValue(int i) { m_fenceValues[i]++; }

        bool WaitForFence(int i);

    protected:
        std::vector<ID3D12CommandAllocator*> m_commandAllocators;
        std::vector<ID3D12Fence*> m_fences;
        std::vector<UINT64> m_fenceValues;
        HANDLE m_fenceEvent;
        u32 m_numThreads;
    };

    class GraphicsCommandContext : public CommandContext
    {
    public:
        GraphicsCommandContext(u32 numFrameBuffers);
        GraphicsCommandContext() : GraphicsCommandContext(1) {}
        ~GraphicsCommandContext();

        bool Initialize(Renderer* pRenderer);
        void Release();

        ID3D12GraphicsCommandList* CommandList() { return m_commandList; }
        bool Reset(GraphicsPipeline* pGraphicsPipeline);
        bool Execute(Renderer* pRenderer);
        bool Close();

        ID3D12CommandAllocator* CommandAllocator() { return m_commandAllocators[m_frameIndex]; }

        ID3D12Fence* Fence() { return m_fences[m_frameIndex]; }
        UINT64* FenceValueAddress() { return &m_fenceValues[m_frameIndex]; }
        UINT64 FenceValue() { return m_fenceValues[m_frameIndex]; }

        void IncrementFenceValue() { m_fenceValues[m_frameIndex]++; }
        bool WaitForFence() { return CommandContext::WaitForFence(m_frameIndex); }

        u32 NumFrameBuffers() { return m_numFrameBuffers; }

        u32 FrameIndex() { return m_frameIndex; }
        void SetFrameIndex(u32 i) { m_frameIndex = i; }

        void Release(Renderer* pRenderer);

    private:
        ID3D12GraphicsCommandList* m_commandList;
        u32 m_frameIndex;
        u32 m_numFrameBuffers;
    };
}

#endif
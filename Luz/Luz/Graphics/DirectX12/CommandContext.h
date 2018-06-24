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
    class CommandAllocator
    {
    public:
        CommandAllocator();
        ~CommandAllocator() = default;

        bool Initialize(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE type);

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
        
        static bool Initialize(ID3D12Device* pDevice);
        static void Destroy();
        static CommandAllocator* Allocate(D3D12_COMMAND_LIST_TYPE type);
        static void WaitAll();

        CommandAllocatorPool();
        ~CommandAllocatorPool() = default;
    private:

        u32 m_remainingAllocators;
        CommandAllocator m_allocators[sm_maxAllocatorsPerType];
    };
}

#endif
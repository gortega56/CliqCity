#pragma once
#ifndef DX12DESCRIPTORALLOCATOR_H
#define DX12DESCRIPTORALLOCATOR_H

#include "d3dx12.h"

#ifndef GRAPHICSTYPES_H
#include "GraphicsTypes.h"
#endif

namespace Graphics
{
    struct Descriptor
    {
        DescriptorHandle Handle;
        D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle;
    };

    struct DescriptorHeap
    {
        u16 Handle;
        ID3D12DescriptorHeap* pHeap;
    };

    struct DescriptorAllocator
    {
        static const u32 sm_maxDescriptorHeaps = 256;

        static const u32 sm_maxDescriptors = 256;

        D3D12_DESCRIPTOR_HEAP_TYPE m_type;

        D3D12_DESCRIPTOR_HEAP_FLAGS m_flags;

        u32 m_descriptorHandleIncrementSize;

        u32 m_remainingHandles;

        DescriptorHeap* m_pCurrentDescriptorHeap;

        std::vector<DescriptorHeap> m_descriptorHeaps;

        std::mutex m_descriptorHeapMutex;

        Descriptor Allocate(const u32 numDescriptors);

        inline ID3D12DescriptorHeap* GetHeap(const u16 handle)
        {
            std::lock_guard<std::mutex> lock(m_descriptorHeapMutex);
            return m_descriptorHeaps[GetHeapHandle(handle)].pHeap;
        }

        inline u16 GetHeapHandle(const u16 handle) { return (handle >> 8) & 0x00FF; }

        inline u16 GetDescriptorHandle(const u16 handle) { return handle & 0x00FF; }
    };

    extern DescriptorAllocator s_descriptorAllocatorCollection[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

    UINT GetDescriptorHandleIncrementSize(const D3D12_DESCRIPTOR_HEAP_TYPE eType);
}

#endif
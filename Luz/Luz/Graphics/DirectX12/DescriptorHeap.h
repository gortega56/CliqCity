#pragma once
#ifndef DESCRIPTORHEAP_H
#define DESCRIPTORHEAP_H

#ifndef DX12_INTERNAL_H
#include "dx12_internal.h"
#endif

namespace dx12
{
    class Renderer;

    class DescriptorHeap
    {
    public:
        DescriptorHeap(u32 numDescriptors = 1);
        ~DescriptorHeap();

        void Release();

        ID3D12DescriptorHeap* Native() { return m_descriptorHeap; }
        ID3D12DescriptorHeap** Address() { return &m_descriptorHeap; }
        u32 Size() { return m_descriptorHeapSize; }
        u32 NumDescriptors() { return m_numDescriptors; }

        CD3DX12_CPU_DESCRIPTOR_HANDLE CpuHandle(int i = 0);

        bool InitializeRTV(Renderer* pRenderer, std::wstring name);
        bool InitializeDSV(Renderer* pRenderer, std::wstring name);
        bool InitializeMixed(Renderer* pRenderer, std::wstring name);
        bool InitializeSampler(Renderer* pRenderer, std::wstring name);

    private:
        ID3D12DescriptorHeap* m_descriptorHeap;
        u32 m_descriptorHeapSize;
        u32 m_numDescriptors;
    };
}

#endif
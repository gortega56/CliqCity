#pragma once
#ifndef DX12_DESCRIPTORHEAP_H
#define DX12_DESCRIPTORHEAP_H

#ifndef __D3DX12_H__
#include "d3dx12.h"
#endif

namespace Dx12
{
    class Device;

    struct DescriptorHeapParams
    {
        D3D12_DESCRIPTOR_HEAP_TYPE Type;
        D3D12_DESCRIPTOR_HEAP_FLAGS Flags;
    };

    static const DescriptorHeapParams RtvHeapParams = { D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE };
    static const DescriptorHeapParams DsvHeapParams = { D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE };
    static const DescriptorHeapParams CbvSrvUavHeapParams = { D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE };
    static const DescriptorHeapParams SamplerHeapParams = { D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE };

    class DescriptorHeap
    {
    public:
        DescriptorHeap(u32 numDescriptors = 1);
        ~DescriptorHeap();

        DescriptorHeap(DescriptorHeap&& other);
        DescriptorHeap& operator=(DescriptorHeap&& other);

        ID3D12DescriptorHeap* Native() const { return m_descriptorHeap; }
        ID3D12DescriptorHeap** Address() { return &m_descriptorHeap; }
        u32 Size() const { return m_descriptorHeapSize; }
        u32 NumDescriptors() { return m_numDescriptors; }

        CD3DX12_CPU_DESCRIPTOR_HANDLE CpuHandle(int i = 0);
        CD3DX12_GPU_DESCRIPTOR_HANDLE GpuHandle(int i = 0);

        bool Initialize(std::shared_ptr<const Device> pDevice, DescriptorHeapParams const* pParams, std::wstring name);

        bool InitializeRTV(std::shared_ptr<const Device> pDevice, std::wstring name = L"");
        bool InitializeDSV(std::shared_ptr<const Device> pDevice, std::wstring name = L"");
        bool InitializeMixed(std::shared_ptr<const Device> pDevice, std::wstring name = L"");
        bool InitializeSampler(std::shared_ptr<const Device> pDevice, std::wstring name = L"");

    private:
        ID3D12DescriptorHeap* m_descriptorHeap;
        u32 m_descriptorHeapSize;
        u32 m_numDescriptors;
        D3D12_DESCRIPTOR_HEAP_TYPE m_type;

        DescriptorHeap(const DescriptorHeap& other) = delete;
        DescriptorHeap& operator=(const DescriptorHeap& other) = delete;
    };

    class DescriptorHandle
    {
    public:
        DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle, D3D12_DESCRIPTOR_HEAP_TYPE type, u32 descriptorIndex, u32 descriptorOffset);
        DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_DESCRIPTOR_HEAP_TYPE type, u32 descriptorIndex, u32 descriptorOffset);
        DescriptorHandle();
        ~DescriptorHandle() = default;

        D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle(u32 offset = 0) const;
        D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle(u32 offset = 0) const;
        D3D12_DESCRIPTOR_HEAP_TYPE Type() const;
        u32 Index() const;
        u32 Offset() const;

        DescriptorHandle(const DescriptorHandle& other) = default;
        DescriptorHandle(DescriptorHandle&& other) = default;
        DescriptorHandle& operator=(const DescriptorHandle& other) = default;
        DescriptorHandle& operator=(DescriptorHandle&& other) = default;

        bool operator==(const DescriptorHandle& other) const;

    private:
        D3D12_CPU_DESCRIPTOR_HANDLE m_cpuHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE m_gpuHandle;
        D3D12_DESCRIPTOR_HEAP_TYPE m_type;
        u32 m_descriptorIndex;
        u32 m_descriptorOffset;
    };

    class DescriptorHeapAllocator
    {
    public:
        static void Initialize();
        static void Destroy();

        DescriptorHeapAllocator() : m_currentHeap(nullptr), m_remainingHandles(sm_handlesPerHeap) {}
        ~DescriptorHeapAllocator() = default;

        const DescriptorHeap* GetHeap(int i) const { return &m_descriptorHeaps[i]; }
        
        DescriptorHeap* Current() const { return m_currentHeap; }
        u32 RemainingHandles() const { return m_remainingHandles; }
    
        DescriptorHandle Allocate(D3D12_DESCRIPTOR_HEAP_TYPE type, u32 count = 1);

    private:
        static const int sm_handlesPerHeap = 256;

        DescriptorHeap* m_currentHeap;
        u32 m_remainingHandles;
        std::mutex m_allocMutex;
        std::vector<DescriptorHeap> m_descriptorHeaps;
    };

    class DescriptorHandleCollection
    {
    public:
        DescriptorHandleCollection();
        ~DescriptorHandleCollection() = default;

        bool TrySet(const DescriptorHandle& handle, u16* pIndex);
        const DescriptorHandle Get(i32 i) const;
        void Remove(i32 i);

    private:
        static const u32 sm_stateBits = 8;
        static const u32 sm_maxHandles = 1024;
        static const u32 sm_maxHandleStates = sm_maxHandles / sm_stateBits;

        DescriptorHandle m_handles[sm_maxHandles];
        u8 m_handleStates[sm_maxHandleStates];
    };

    DescriptorHandle AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type, u32 count = 1);
    const DescriptorHeap* GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, u32 index);
    void GetDescriptorHeaps(std::vector<DescriptorHandle>& handles, std::vector<ID3D12DescriptorHeap*>& out);
}

#endif
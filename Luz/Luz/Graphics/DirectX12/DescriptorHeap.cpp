#include "stdafx.h"
#include "DescriptorHeap.h"
#include "dx12_internal.h"

namespace Dx12
{
    static u8 g_memory[sizeof(DescriptorHeapAllocator) * D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
    static DescriptorHeapAllocator* g_allocators = nullptr;
    static ID3D12Device* s_pDevice = nullptr;

    void DescriptorHeapAllocator::Initialize(ID3D12Device* pDevice)
    {
        LUZASSERT(!s_pDevice);
        s_pDevice = pDevice;

        LUZASSERT(!g_allocators);
        for (int i = 0, count = (int)D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; i < count; ++i)
        {
            new(&g_memory[sizeof(DescriptorHeapAllocator) * i]) DescriptorHeapAllocator();
        }

        g_allocators = reinterpret_cast<DescriptorHeapAllocator*>(g_memory);
    }

    void DescriptorHeapAllocator::Destroy()
    {
        LUZASSERT(g_allocators);
        for (int i = 0, count = (int)D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; i < count; ++i)
        {
            g_allocators[i].~DescriptorHeapAllocator();
        }

        g_allocators = nullptr;
        ZeroMemory(g_memory, sizeof(DescriptorHeapAllocator) * D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES);
    }

    DescriptorHeap::DescriptorHeap(u32 numDescriptors) : m_descriptorHeap(nullptr), m_descriptorHeapSize(0), m_numDescriptors(numDescriptors)
    {

    }

    DescriptorHeap::~DescriptorHeap()
    {
        SAFE_RELEASE(m_descriptorHeap);
    }

    DescriptorHeap::DescriptorHeap(DescriptorHeap&& other) :
        m_descriptorHeap(other.m_descriptorHeap),
        m_descriptorHeapSize(other.m_descriptorHeapSize),
        m_numDescriptors(other.m_numDescriptors),
        m_type(other.m_type)
    {
        other.m_descriptorHeap = nullptr;
        other.m_descriptorHeapSize = 0;
        other.m_numDescriptors = 0;
        other.m_type = (D3D12_DESCRIPTOR_HEAP_TYPE)-1;
    }

    DescriptorHeap& DescriptorHeap::operator=(DescriptorHeap&& other)
    {
        m_descriptorHeap = other.m_descriptorHeap;
        m_descriptorHeapSize = other.m_descriptorHeapSize;
        m_numDescriptors = other.m_numDescriptors;
        m_type = other.m_type;

        other.m_descriptorHeap = nullptr;
        other.m_descriptorHeapSize = 0;
        other.m_numDescriptors = 0;
        other.m_type = (D3D12_DESCRIPTOR_HEAP_TYPE)-1;
        return *this;
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::CpuHandle(int i) const
    {
        return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart(), i, m_descriptorHeapSize);
    }

    CD3DX12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GpuHandle(int i) const
    {
        return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), i, m_descriptorHeapSize);
    }

    bool DescriptorHeap::Initialize(DescriptorHeapParams const* pParams, std::wstring name)
    {
        if (!CreateDescriptorHeap(s_pDevice, &m_descriptorHeap, name.c_str(), pParams->Type, pParams->Flags, m_numDescriptors))
        {
            return false;
        }

        m_descriptorHeapSize = s_pDevice->GetDescriptorHandleIncrementSize(pParams->Type);

        return true;
    }

    bool DescriptorHeap::InitializeRTV(std::wstring name)
    {
        return Initialize(&RtvHeapParams, name);
    }

    bool DescriptorHeap::InitializeDSV(std::wstring name)
    {
        return Initialize(&DsvHeapParams, name);

    }

    bool DescriptorHeap::InitializeMixed(std::wstring name)
    {
        return Initialize(&CbvSrvUavHeapParams, name);

    }

    bool DescriptorHeap::InitializeSampler(std::wstring name)
    {
        return Initialize(&SamplerHeapParams, name);
    }

    DescriptorHandle::DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle, D3D12_DESCRIPTOR_HEAP_TYPE type, u32 descriptorIndex, u32 descriptorOffset) :
        m_cpuHandle(cpuHandle), m_gpuHandle(gpuHandle), m_type(type), m_descriptorIndex(descriptorIndex), m_descriptorOffset(descriptorOffset)
    {

    }

    DescriptorHandle::DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_DESCRIPTOR_HEAP_TYPE type, u32 descriptorIndex, u32 descriptorOffset) :
        m_cpuHandle(cpuHandle), m_type(type), m_descriptorIndex(descriptorIndex), m_descriptorOffset(descriptorOffset)
    {
        m_gpuHandle.ptr = 0;
    }

    DescriptorHandle::DescriptorHandle()
    {
        m_cpuHandle.ptr = 0;
        m_gpuHandle.ptr = 0;
        m_type = (D3D12_DESCRIPTOR_HEAP_TYPE)-1;
        m_descriptorIndex = -1;
        m_descriptorOffset = -1;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle::CpuHandle(u32 offset /*= 0*/) const
    {
        LUZASSERT(g_allocators[m_type].GetHeap(m_descriptorIndex));
        return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_cpuHandle, offset, g_allocators[m_type].GetHeap(m_descriptorIndex)->Size());
    }

    D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHandle::GpuHandle(u32 offset /*= 0*/) const
    {
        LUZASSERT(g_allocators[m_type].GetHeap(m_descriptorIndex));
        return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_gpuHandle, offset, g_allocators[m_type].GetHeap(m_descriptorIndex)->Size());
    }

    D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHandle::Type() const
    {
        return m_type;
    }

    u32 DescriptorHandle::Index() const
    {
        return m_descriptorIndex;
    }

    u32 DescriptorHandle::Offset() const
    {
        return m_descriptorOffset;
    }

    bool DescriptorHandle::operator==(const DescriptorHandle& other) const
    {
        return m_type == other.m_type && m_descriptorIndex == other.m_descriptorIndex && m_descriptorOffset == other.m_descriptorOffset;
    }

    DescriptorHandle DescriptorHeapAllocator::Allocate(D3D12_DESCRIPTOR_HEAP_TYPE type, u32 count /*= 1*/)
    {
        if (m_currentHeap == nullptr || m_remainingHandles < count)
        {
            {
                std::lock_guard<std::mutex> guard(m_allocMutex);
                m_descriptorHeaps.emplace_back(sm_handlesPerHeap);
                m_currentHeap = &m_descriptorHeaps.back();
            }

            switch (type)
            {
            case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
                m_currentHeap->InitializeMixed(L"CBV UAV SRV Heap" + (u32)(m_descriptorHeaps.size() - 1));
                break;
            case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
                m_currentHeap->InitializeDSV(L"DSV Heap" + (u32)(m_descriptorHeaps.size() - 1));
                break;
            case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
                m_currentHeap->InitializeRTV(L"RTV Heap" + (u32)(m_descriptorHeaps.size() - 1));
                break;
            case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
                m_currentHeap->InitializeSampler(L"Sampler Heap" + (u32)(m_descriptorHeaps.size() - 1));
                break;
            }

            m_remainingHandles = sm_handlesPerHeap;
        }

        u32 offset = sm_handlesPerHeap - m_remainingHandles;
        DescriptorHandle handle(m_currentHeap->CpuHandle(offset), m_currentHeap->GpuHandle(offset), type, (u32)(m_descriptorHeaps.size() - 1), offset);
        m_remainingHandles -= count;
        return handle;
    }

    DescriptorHandle AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type, u32 count /*= 1*/)
    {
        DescriptorHandle handle = g_allocators[type].Allocate(type, count);
        return handle;
    }

    const DescriptorHeap* GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, u32 index)
    {
        return g_allocators[type].GetHeap(index);
    }

    void GetDescriptorHeaps(std::vector<DescriptorHandle>& handles, std::vector<ID3D12DescriptorHeap*>& out)
    {
        std::vector<u32> uniqueIndicesByType[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
        for (auto& handle : handles)
        {
            auto& uniqueIndices = uniqueIndicesByType[handle.Type()];
            if (std::find_if(uniqueIndices.begin(), uniqueIndices.end(), [&handle](const u32& index) { return handle.Index() == index; }) == uniqueIndices.end())
            {
                uniqueIndices.push_back(handle.Index());

                auto pHeap = GetDescriptorHeap(handle.Type(), handle.Index());
                out.push_back(pHeap->Native());
            }
        }
    }

    ID3D12DescriptorHeap* GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
    {
        for (u32 i = 0, count = g_allocators[type].NumHeaps(); i < count; ++i)
        {
            auto pHeap = g_allocators[type].GetHeap(i);
            D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle = pHeap->CpuHandle();
            D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle = pHeap->GpuHandle();
            if (cpuDescriptorHandle.ptr == cpuHandle.ptr && gpuDescriptorHandle.ptr == gpuHandle.ptr)
            {
                return pHeap->Native();
            }
        }

        return nullptr;
    }
}



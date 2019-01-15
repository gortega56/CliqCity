#include "stdafx.h"
#include "Dx12DescriptorAllocator.h"
#include "Dx12GraphicsTypes.h"

namespace Graphics
{
    DescriptorAllocator s_descriptorAllocatorCollection[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

    Descriptor DescriptorAllocator::Allocate(const u32 numDescriptors)
    {
        if (m_pCurrentDescriptorHeap == nullptr || m_remainingHandles < numDescriptors)
        {
            u16 nHeaps;
            {
                std::lock_guard<std::mutex> guard(m_descriptorHeapMutex);
                m_pCurrentDescriptorHeap = &m_descriptorHeaps.emplace_back();
                nHeaps = static_cast<u16>(m_descriptorHeaps.size());
            }

            D3D12_DESCRIPTOR_HEAP_DESC desc;
            ZeroMemory(&desc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
            desc.Type = m_type;
            desc.Flags = m_flags;
            desc.NumDescriptors = static_cast<UINT>(sm_maxDescriptors);

            HRESULT hr = s_device.pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pCurrentDescriptorHeap->pHeap));
            LUZASSERT(SUCCEEDED(hr));

            std::wostringstream ss;
            ss << m_type << " Descriptor Heap " << (nHeaps - 1);

            m_pCurrentDescriptorHeap->pHeap->SetName(ss.str().c_str());
            m_pCurrentDescriptorHeap->Handle = nHeaps - 1;

            m_remainingHandles = sm_maxDescriptors;
        }

        u16 descriptorOffset = sm_maxDescriptors - m_remainingHandles;

        m_remainingHandles -= numDescriptors;

        Descriptor descriptor;
        descriptor.Handle = static_cast<DescriptorHandle>((m_pCurrentDescriptorHeap->Handle << 8) & descriptorOffset);
        descriptor.CpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pCurrentDescriptorHeap->pHeap->GetCPUDescriptorHandleForHeapStart(), descriptorOffset, m_descriptorHandleIncrementSize);
        descriptor.GpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_pCurrentDescriptorHeap->pHeap->GetGPUDescriptorHandleForHeapStart(), descriptorOffset, m_descriptorHandleIncrementSize);
        return descriptor;
    }

    UINT GetDescriptorHandleIncrementSize(const D3D12_DESCRIPTOR_HEAP_TYPE eType)
    {
        return s_descriptorAllocatorCollection[eType].m_descriptorHandleIncrementSize;
    }
}
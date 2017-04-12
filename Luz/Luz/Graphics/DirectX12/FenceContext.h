#pragma once
#ifndef DX12_FENCECONTEXT_H
#define DX12_FENCECONTEXT_H

#ifndef DX12_INTERNAL_H
#include "dx12_internal.h"
#endif

namespace dx12
{
    class Device;

    class FenceContext
    {
    public:
        FenceContext();
        ~FenceContext();

        bool Initialize(std::shared_ptr<const Device> pDevice, const u32 numFences);
        bool WaitForFence(i32 i);

        ID3D12Fence* Fence(i32 i) const { return m_fences[i]; }
        UINT64 FenceValue(i32 i) const { return m_fenceValues[i]; }
        HANDLE FenceEvent() const { return m_fenceEvent; }
        u32 NumFences() const { return (u32)m_fences.size(); }

    protected:
        std::vector<ID3D12Fence*> m_fences;
        std::vector<UINT64> m_fenceValues;
        HANDLE m_fenceEvent;
    };
}

#endif

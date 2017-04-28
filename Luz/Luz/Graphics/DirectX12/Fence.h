#pragma once
#ifndef DX12_FENCE_H
#define DX12_FENCE_H

#ifndef DX12_INTERNAL_H
#include "dx12_internal.h"
#endif

namespace dx12
{
    class Device;

    class Fence
    {
    public:
        Fence();
        ~Fence();

        bool Initialize(std::shared_ptr<const Device> pDevice);
        bool Wait();
        bool IsAvailable() const
        {
            return m_completed == m_fence->GetCompletedValue();
        }

        ID3D12Fence* Ptr() const { return m_fence; }
        UINT64 Signal() const { return m_signal; }

        UINT64* SignalPtr() { return &m_signal; }

        HANDLE Event() const { return m_event; }
    protected:
        friend class CommandQueue;

        UINT64 m_signal;
        UINT64 m_completed;
        HANDLE m_event;
        ID3D12Fence* m_fence;
    };

    //class FenceContext
    //{
    //public:
    //    FenceContext();
    //    ~FenceContext();

    //    bool Initialize(std::shared_ptr<const Device> pDevice, const u32 numFences);
    //    bool WaitForFence(i32 i);

    //    ID3D12Fence* Fence(i32 i) const { return m_fences[i]; }
    //    UINT64 FenceValue(i32 i) const { return m_fenceValues[i]; }
    //    HANDLE FenceEvent() const { return m_fenceEvent; }
    //    u32 NumFences() const { return (u32)m_fences.size(); }

    //protected:
    //    std::vector<ID3D12Fence*> m_fences;
    //    std::vector<UINT64> m_fenceValues;
    //    HANDLE m_fenceEvent;
    //};
}

#endif

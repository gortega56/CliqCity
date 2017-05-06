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

        bool IsWaiting() const { return m_completed < m_signal; }
     
        ID3D12Fence* Ptr() const { return m_fence; }
        UINT64 Signal() const { return m_signal; }
        UINT64* SignalPtr() { return &m_signal; }
        HANDLE Event() const { return m_event; }

    protected:
        friend class CommandQueue;

        UINT64 m_signal;
        UINT64 m_completed;
        ID3D12Fence* m_fence;
        HANDLE m_event;

        void IncrementSignal() { m_signal++; }
    };
}

#endif

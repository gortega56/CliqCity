#pragma once
#ifndef DX12_FENCE_H
#define DX12_FENCE_H

struct ID3D12Fence;

namespace Dx12
{
    class Device;

    class Fence
    {
    public:
        Fence();
        ~Fence();

        bool Initialize(std::shared_ptr<const Device> pDevice);
        bool Wait();

        bool IsIdle() const;
        bool InUse() const;
        bool IsWaiting() const;
     
        ID3D12Fence* Ptr() const { return m_fence; }
        UINT64 Completed() const { return m_completed; }
        UINT64 Signal() const { return m_signal; }
        UINT64* SignalPtr() { return &m_signal; }
        HANDLE Event() const { return m_event; }

        void IncrementSignal();

    protected:
        friend class CommandQueue;

        UINT64 m_signal;
        UINT64 m_completed;
        ID3D12Fence* m_fence;
        HANDLE m_event;
    };
}

#endif

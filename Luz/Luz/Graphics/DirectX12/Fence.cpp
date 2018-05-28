#include "stdafx.h"
#include "Fence.h"
#include "dx12_internal.h"
#include "Device.h"

using namespace Dx12;

Fence::Fence() : 
    m_signal(0),
    m_completed(0),
    m_fence(nullptr),
    m_event(nullptr)
{

}

Fence::~Fence()
{
    SAFE_RELEASE(m_fence);
  //  CloseHandle(m_event);
}

bool Fence::Initialize(std::shared_ptr<const Device> pDevice)
{
    UINT64 signal = 0;
    if (!CreateFences(pDevice->DX(), &m_fence, &signal, D3D12_FENCE_FLAG_NONE, 1))
    {
        return false;
    }

    m_signal.store(signal);

    m_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_event == nullptr)
    {
        return false;
    }

    return true;
}

bool Fence::Wait()
{
    bool running = false;

    // if the current fence value is still less than "fenceValue", then we know the GPU has not finished executing
    // the command queue since it has not reached the "commandQueue->Signal(fence, fenceValue)" command
    UINT64 gpuCompleted = m_fence->GetCompletedValue();
    UINT64 signal = m_signal.load();
    LUZASSERT(gpuCompleted <= signal);
    if (gpuCompleted < signal)
    {
        HRESULT hr = m_fence->SetEventOnCompletion(signal, m_event);
        if (SUCCEEDED(hr))
        {
            // We will wait until the fence has triggered the event that it's current value has reached "fenceValue". once it's value
            // has reached "fenceValue", we know the command queue has finished executing
            WaitForSingleObject(m_event, INFINITE);
            running = true;
        }
    }

    m_completed.store(gpuCompleted);

    return running;
}

bool Fence::IsIdle() const
{
    return (m_signal.load() - m_completed.load()) == 0;
}

bool Fence::InUse() const
{
    return (m_signal.load() - m_completed.load()) == 1;
}

bool Fence::IsWaiting() const
{
    return (m_signal.load() - m_completed.load()) > 1;
}

void Fence::IncrementSignal()
{
    m_signal.fetch_add(1);
}
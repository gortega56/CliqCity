#include "stdafx.h"
#include "Fence.h"
#include "Device.h"

using namespace dx12;

Fence::Fence() : 
    m_signal(0),
    m_completed(0),
    m_event(nullptr),
    m_fence(nullptr)
{

}

Fence::~Fence()
{
    SAFE_RELEASE(m_fence);
}

bool Fence::Initialize(std::shared_ptr<const Device> pDevice)
{
    if (!CreateFences(pDevice->DX(), &m_fence, &m_signal, D3D12_FENCE_FLAG_NONE, 1))
    {
        return false;
    }

    m_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_event == nullptr)
    {
        return false;
    }

    return true;
}

bool Fence::Wait()
{
    UINT64 signal = m_signal;
    
    bool result = ::WaitForFence(m_fence, &signal, m_event);
    m_completed = m_signal;
    m_signal = signal;

    return result;
}

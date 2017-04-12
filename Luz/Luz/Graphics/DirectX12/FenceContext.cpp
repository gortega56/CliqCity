#include "stdafx.h"
#include "FenceContext.h"
#include "Device.h"

using namespace dx12;

FenceContext::FenceContext() :
    m_fenceEvent(nullptr)
{

}

FenceContext::~FenceContext()
{
    for (auto& fence : m_fences)
    {
        SAFE_RELEASE(fence);
    }
}

bool FenceContext::Initialize(std::shared_ptr<const Device> pDevice, const u32 numFences)
{
    m_fences.resize(numFences, nullptr);
    m_fenceValues.resize(numFences, 0);

    if (!CreateFences(pDevice->DX(), m_fences.data(), m_fenceValues.data(), D3D12_FENCE_FLAG_NONE, (UINT)m_fences.size()))
    {
        return false;
    }

    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_fenceEvent == nullptr)
    {
        return false;
    }
}

bool FenceContext::WaitForFence(i32 i)
{
    return ::WaitForFence(m_fences[i], &m_fenceValues[i], m_fenceEvent);
}

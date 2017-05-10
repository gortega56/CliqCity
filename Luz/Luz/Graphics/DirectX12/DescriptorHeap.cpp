#include "stdafx.h"
#include "DescriptorHeap.h"
#include "dx12_renderer.h"

using namespace Dx12;

DescriptorHeap::DescriptorHeap(u32 numDescriptors) : m_descriptorHeap(nullptr), m_descriptorHeapSize(0), m_numDescriptors(numDescriptors)
{

}

DescriptorHeap::~DescriptorHeap()
{
    Release();
}

void DescriptorHeap::Release()
{
    SAFE_RELEASE(m_descriptorHeap);
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::CpuHandle(int i)
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart(), i, m_descriptorHeapSize);
}

bool DescriptorHeap::InitializeRTV(std::shared_ptr<const Device> pDevice, std::wstring name)
{
    if (!CreateDescriptorHeap(pDevice->DX(), &m_descriptorHeap, name.c_str(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, m_numDescriptors))
    {
        return false;
    }

    m_descriptorHeapSize = pDevice->DX()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    return true;
}

bool DescriptorHeap::InitializeDSV(std::shared_ptr<const Device> pDevice, std::wstring name)
{
    if (!CreateDescriptorHeap(pDevice->DX(), &m_descriptorHeap, name.c_str(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, m_numDescriptors))
    {
        return false;
    }

    m_descriptorHeapSize = pDevice->DX()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

    return true;
}

bool DescriptorHeap::InitializeMixed(std::shared_ptr<const Device> pDevice, std::wstring name)
{
    if (!CreateDescriptorHeap(pDevice->DX(), &m_descriptorHeap, name.c_str(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, m_numDescriptors))
    {
        return false;
    }

    m_descriptorHeapSize = pDevice->DX()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    return true;
}

bool DescriptorHeap::InitializeSampler(std::shared_ptr<const Device> pDevice, std::wstring name)
{
    if (!CreateDescriptorHeap(pDevice->DX(), &m_descriptorHeap, name.c_str(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, m_numDescriptors))
    {
        return false;
    }

    m_descriptorHeapSize = pDevice->DX()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

    return true;
}
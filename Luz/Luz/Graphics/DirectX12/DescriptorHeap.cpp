#include "stdafx.h"
#include "DescriptorHeap.h"
#include "dx12_internal.h"
#include "dx12_renderer.h"
#include "GpuResource.h"

using namespace Dx12;

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

CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::CpuHandle(int i)
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart(), i, m_descriptorHeapSize);
}

bool DescriptorHeap::Initialize(std::shared_ptr<const Device> pDevice, DescriptorHeapParams const* pParams, std::wstring name)
{
    if (!CreateDescriptorHeap(pDevice->DX(), &m_descriptorHeap, name.c_str(), pParams->Type, pParams->Flags, m_numDescriptors))
    {
        return false;
    }

    m_descriptorHeapSize = pDevice->DX()->GetDescriptorHandleIncrementSize(pParams->Type);

    return true;
}

bool DescriptorHeap::InitializeRTV(std::shared_ptr<const Device> pDevice, std::wstring name)
{
    return Initialize(pDevice, &RtvHeapParams, name);
}

bool DescriptorHeap::InitializeDSV(std::shared_ptr<const Device> pDevice, std::wstring name)
{
    return Initialize(pDevice, &DsvHeapParams, name);

}

bool DescriptorHeap::InitializeMixed(std::shared_ptr<const Device> pDevice, std::wstring name)
{
    return Initialize(pDevice, &CbvSrvUavHeapParams, name);

}

bool DescriptorHeap::InitializeSampler(std::shared_ptr<const Device> pDevice, std::wstring name)
{
    return Initialize(pDevice, &SamplerHeapParams, name);
}

void DescriptorHeap::CreateShaderResourceViews(std::shared_ptr<const Device> pDevice, const PixelBuffer* pixelBuffer, int offset /*= 0*/)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    desc.Format = pixelBuffer->Format();
    desc.ViewDimension = pixelBuffer->ViewDimension();
    desc.Texture2D.MipLevels = pixelBuffer->MipLevels();
    pDevice->DX()->CreateShaderResourceView(pixelBuffer->Resource(), &desc, CD3DX12_CPU_DESCRIPTOR_HANDLE(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart(), offset, m_descriptorHeapSize));
}
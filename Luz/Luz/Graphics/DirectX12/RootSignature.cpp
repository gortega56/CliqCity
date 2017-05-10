#include "stdafx.h"
#include "RootSignature.h"
#include "Device.h"

using namespace Dx12;

DescriptorTable::DescriptorTable(int numRanges)
{
    m_ranges.reserve((size_t)numRanges);
}

DescriptorTable::~DescriptorTable()
{

}

DescriptorTable& DescriptorTable::AppendRangeSRV(UINT numDescriptors, u32 shaderRegister, u32 registerSpace, D3D12_DESCRIPTOR_RANGE_FLAGS flags, UINT offsetInDescriptorsFromTableStart)
{
    m_ranges.emplace_back(CD3DX12_DESCRIPTOR_RANGE1());
    m_ranges.back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, numDescriptors, shaderRegister, registerSpace, flags, offsetInDescriptorsFromTableStart);
    return *this;
}

DescriptorTable& DescriptorTable::AppendRangeUAV(UINT numDescriptors, u32 shaderRegister, u32 registerSpace, D3D12_DESCRIPTOR_RANGE_FLAGS flags, UINT offsetInDescriptorsFromTableStart)
{
    m_ranges.emplace_back(CD3DX12_DESCRIPTOR_RANGE1());
    m_ranges.back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, numDescriptors, shaderRegister, registerSpace, flags, offsetInDescriptorsFromTableStart);
    return *this;
}

DescriptorTable& DescriptorTable::AppendRangeCBV(UINT numDescriptors, u32 shaderRegister, u32 registerSpace, D3D12_DESCRIPTOR_RANGE_FLAGS flags, UINT offsetInDescriptorsFromTableStart)
{
    m_ranges.emplace_back(CD3DX12_DESCRIPTOR_RANGE1());
    m_ranges.back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, numDescriptors, shaderRegister, registerSpace, flags, offsetInDescriptorsFromTableStart);
    return *this;
}

DescriptorTable& DescriptorTable::AppendRangeSampler(UINT numDescriptors, u32 shaderRegister, u32 registerSpace, D3D12_DESCRIPTOR_RANGE_FLAGS flags, UINT offsetInDescriptorsFromTableStart)
{
    m_ranges.emplace_back(CD3DX12_DESCRIPTOR_RANGE1());
    m_ranges.back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, numDescriptors, shaderRegister, registerSpace, flags, offsetInDescriptorsFromTableStart);
    return *this;
}

RootSignature::RootSignature(int numParams) : m_flags(D3D12_ROOT_SIGNATURE_FLAG_NONE)
{
    m_rootParameters.reserve((size_t)numParams);
}

RootSignature::~RootSignature()
{
    SAFE_RELEASE(m_signature);
}

RootSignature& RootSignature::AppendRootConstant(u32 num32BitVals, u32 shaderRegister, u32 registerSpace, D3D12_SHADER_VISIBILITY visibility)
{
    m_rootParameters.emplace_back(CD3DX12_ROOT_PARAMETER1());
    m_rootParameters.back().InitAsConstants(num32BitVals, shaderRegister, registerSpace, visibility);
    return *this;
}

RootSignature& RootSignature::AppendRootCBV(u32 shaderRegister, u32 registerSpace, D3D12_ROOT_DESCRIPTOR_FLAGS flags, D3D12_SHADER_VISIBILITY visibility)
{
    m_rootParameters.emplace_back(CD3DX12_ROOT_PARAMETER1());
    m_rootParameters.back().InitAsConstantBufferView(shaderRegister, registerSpace, flags, visibility);
    return *this;
}

RootSignature& RootSignature::AppendRootSRV(u32 shaderRegister, u32 registerSpace, D3D12_ROOT_DESCRIPTOR_FLAGS flags, D3D12_SHADER_VISIBILITY visibility)
{
    m_rootParameters.emplace_back(CD3DX12_ROOT_PARAMETER1());
    m_rootParameters.back().InitAsShaderResourceView(shaderRegister, registerSpace, flags, visibility);
    return *this;
}

RootSignature& RootSignature::AppendRootUAV(u32 shaderRegister, u32 registerSpace, D3D12_ROOT_DESCRIPTOR_FLAGS flags, D3D12_SHADER_VISIBILITY visibility)
{
    m_rootParameters.emplace_back(CD3DX12_ROOT_PARAMETER1());
    m_rootParameters.back().InitAsUnorderedAccessView(shaderRegister, registerSpace, flags, visibility);
    return *this;
}

RootSignature& RootSignature::AppendRootDescriptorTable(DescriptorTable& descTable, D3D12_SHADER_VISIBILITY visibility)
{
    m_rootParameters.emplace_back(CD3DX12_ROOT_PARAMETER1());
    m_rootParameters.back().InitAsDescriptorTable(descTable.NumRanges(), descTable.Ranges());
    return *this;
}

RootSignature& RootSignature::AppendDefaultSampler(u32 shaderRegister)
{
    m_staticSamplers.emplace_back(CD3DX12_STATIC_SAMPLER_DESC());
    m_staticSamplers.back().Init(shaderRegister);
    return *this;
}

RootSignature& RootSignature::SetFlag(D3D12_ROOT_SIGNATURE_FLAGS flag)
{
    m_flags |= flag;
    return *this;
}

bool RootSignature::Finalize(std::shared_ptr<const Device> pDevice)
{
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc;
    desc.Init_1_1((UINT)m_rootParameters.size(), m_rootParameters.data(), (UINT)m_staticSamplers.size(), m_staticSamplers.data(), m_flags);

    ID3DBlob* pBlob;
    ID3DBlob* pErrorBlob;
    HRESULT hr = D3D12SerializeVersionedRootSignature(&desc, &pBlob, &pErrorBlob);
    if (FAILED(hr))
    {
        return false;
    }

    hr = pDevice->DX()->CreateRootSignature(0, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_PPV_ARGS(&m_signature));
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}
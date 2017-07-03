#include "stdafx.h"
#include "RootSignature.h"
#include "dx12_internal.h"
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
    m_rootParameters.back().InitAsDescriptorTable(descTable.NumRanges(), descTable.Ranges(), visibility);
    return *this;
}

RootSignature& RootSignature::AppendAnisotropicWrapSampler(u32 shaderRegister)
{
    return AppendStaticSampler(shaderRegister, &MinMagAnisotropicWrapParams);
}

RootSignature& RootSignature::AppendAnisotropicClampSampler(u32 shaderRegister)
{
    return AppendStaticSampler(shaderRegister, &MinMagAnisotropicClampParams);
}

RootSignature& RootSignature::AppendPointWrapSampler(u32 shaderRegister)
{
    return AppendStaticSampler(shaderRegister, &MinMagPointWrapParams);
}
RootSignature& RootSignature::AppendPointClampSampler(u32 shaderRegister)
{
    return AppendStaticSampler(shaderRegister, &MinMagPointClampParams);
}

RootSignature& RootSignature::AppendLinearWrapSampler(u32 shaderRegister)
{
    return AppendStaticSampler(shaderRegister, &MinMagLinearWrapParams);
}

RootSignature& RootSignature::AppendLinearClampSampler(u32 shaderRegister)
{
    return AppendStaticSampler(shaderRegister, &MinMagLinearClampParams);
}

RootSignature& RootSignature::AppendStaticSampler(u32 shaderRegister, const SamplerParams* pParams, u32 registerSpace /*= 0U*/, D3D12_SHADER_VISIBILITY shaderVisibility /*= D3D12_SHADER_VISIBILITY_ALL*/)
{
    return AppendStaticSampler(shaderRegister,
        registerSpace,
        pParams->Filter,
        pParams->AddressMode,
        pParams->AddressMode,
        pParams->AddressMode,
        pParams->MipLODBias,
        pParams->MaxAnisotropy,
        pParams->ComparisonFunc,
        pParams->BorderColor,
        pParams->MinLOD,
        pParams->MaxLOD,
        shaderVisibility);
}

RootSignature& RootSignature::AppendStaticSampler(u32 shaderRegister,
    u32 registerSpace /*= 0U*/,
    D3D12_FILTER filter /*= D3D12_FILTER_ANISOTROPIC*/,
    D3D12_TEXTURE_ADDRESS_MODE addressU /*= D3D12_TEXTURE_ADDRESS_MODE_WRAP*/,
    D3D12_TEXTURE_ADDRESS_MODE addressV /*= D3D12_TEXTURE_ADDRESS_MODE_WRAP*/,
    D3D12_TEXTURE_ADDRESS_MODE addressW /*= D3D12_TEXTURE_ADDRESS_MODE_WRAP*/,
    FLOAT mipLODBias /*= 0*/,
    UINT maxAnisotropy /*= 16*/,
    D3D12_COMPARISON_FUNC comparisonFunc /*= D3D12_COMPARISON_FUNC_LESS_EQUAL*/,
    D3D12_STATIC_BORDER_COLOR borderColor /*= D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE*/,
    FLOAT minLOD /*= 0.f*/,
    FLOAT maxLOD /*= D3D12_FLOAT32_MAX*/,
    D3D12_SHADER_VISIBILITY shaderVisibility /*= D3D12_SHADER_VISIBILITY_ALL*/)
{
    m_staticSamplers.emplace_back(CD3DX12_STATIC_SAMPLER_DESC());
    m_staticSamplers.back().Init(shaderRegister, filter, addressU, addressV, addressW, mipLODBias, maxAnisotropy, comparisonFunc, borderColor, minLOD, maxLOD, shaderVisibility, registerSpace);
    return *this;
}

RootSignature& RootSignature::SetFlag(D3D12_ROOT_SIGNATURE_FLAGS flag)
{
    m_flags |= flag;
    return *this;
}

bool RootSignature::Finalize()
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

    hr = Device::SharedInstance()->DX()->CreateRootSignature(0, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_PPV_ARGS(&m_signature));
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}
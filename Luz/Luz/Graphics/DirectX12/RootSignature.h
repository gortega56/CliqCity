#pragma once
#ifndef DX12ROOTSIGNATURE_H
#define DX12ROOTSIGNATURE_H

#ifndef DX12_INTERNAL_H
#include "dx12_internal.h"
#endif

namespace dx12
{
    class DescriptorTable
    {
    public:
        DescriptorTable(int numRanges = 0);
        ~DescriptorTable();

        DescriptorTable& AppendRangeSRV(UINT numDescriptors, u32 shaderRegister, u32 registerSpace = 0U, D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE, UINT offsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
        DescriptorTable& AppendRangeUAV(UINT numDescriptors, u32 shaderRegister, u32 registerSpace = 0U, D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE, UINT offsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
        DescriptorTable& AppendRangeCBV(UINT numDescriptors, u32 shaderRegister, u32 registerSpace = 0U, D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE, UINT offsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
        DescriptorTable& AppendRangeSampler(UINT numDescriptors, u32 shaderRegister, u32 registerSpace = 0U, D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE, UINT offsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);

        D3D12_DESCRIPTOR_RANGE1* Ranges() { return m_ranges.data(); }
        u32 NumRanges() { return (u32)m_ranges.size(); }
    private:
        std::vector<CD3DX12_DESCRIPTOR_RANGE1> m_ranges;
    };

    class Renderer;

    class RootSignature
    {
    public:
        RootSignature(int numParams = 0);
        ~RootSignature();

        ID3D12RootSignature* Signature() { return m_signature; }

        RootSignature& AppendRootConstant(u32 num32BitVals, u32 shaderRegister, u32 registerSpace = 0U, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
        RootSignature& AppendRootCBV(u32 shaderRegister, u32 registerSpace = 0U, D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
        RootSignature& AppendRootSRV(u32 shaderRegister, u32 registerSpace = 0U, D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
        RootSignature& AppendRootUAV(u32 shaderRegister, u32 registerSpace = 0U, D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
        RootSignature& AppendRootDescriptorTable(DescriptorTable& descTable, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
        RootSignature& AppendDefaultSampler(u32 shaderRegister);

        bool Finalize(Renderer* pRenderer);

        void SetFlags(D3D12_ROOT_SIGNATURE_FLAGS flags) { m_flags = flags; }

    private:
        ID3D12RootSignature* m_signature;
        D3D12_ROOT_SIGNATURE_FLAGS m_flags;
        std::vector<CD3DX12_ROOT_PARAMETER1> m_rootParameters;
        std::vector<CD3DX12_STATIC_SAMPLER_DESC> m_staticSamplers;
    };
}

#endif
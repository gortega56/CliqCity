#pragma once
#ifndef DX12_ROOTSIGNATURE_H
#define DX12_ROOTSIGNATURE_H

#ifndef __D3DX12_H__
#include "d3dx12.h"
#endif

namespace Dx12
{
    namespace Material
    {
        class Immutable;
    }

    struct SamplerParams
    {
        D3D12_FILTER Filter;
        D3D12_TEXTURE_ADDRESS_MODE AddressMode;
        FLOAT MipLODBias;
        UINT MaxAnisotropy;
        D3D12_COMPARISON_FUNC ComparisonFunc;
        D3D12_STATIC_BORDER_COLOR BorderColor;
        FLOAT MinLOD;
        FLOAT MaxLOD;
    };

    static const SamplerParams MinMagAnisotropicWrapParams = { D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_WRAP, 0.0f, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, 0.0f, D3D12_FLOAT32_MAX };
    static const SamplerParams MinMagAnisotropicClampParams = { D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, 0.0f, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, 0.0f, D3D12_FLOAT32_MAX };

    static const SamplerParams MinMagPointWrapParams = { D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_WRAP, 0.0f, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, 0.0f, D3D12_FLOAT32_MAX };
    static const SamplerParams MinMagPointClampParams = { D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, 0.0f, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, 0.0f, D3D12_FLOAT32_MAX };

    static const SamplerParams MinMagLinearWrapParams = { D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP, 0.0f, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, 0.0f, D3D12_FLOAT32_MAX };
    static const SamplerParams MinMagLinearClampParams = { D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, 0.0f, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, 0.0f, D3D12_FLOAT32_MAX };

    static const SamplerParams MinMagPointWhiteBorderParams = { D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0.0f, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, 0.0f, D3D12_FLOAT32_MAX };
    static const SamplerParams MinMagPointBlackBorderParams = { D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0.0f, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK, 0.0f, D3D12_FLOAT32_MAX };
    static const SamplerParams MinMagLinearWhiteBorderParams = { D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0.0f, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, 0.0f, D3D12_FLOAT32_MAX };
    static const SamplerParams MinMagLinearBlackBorderParams = { D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0.0f, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK, 0.0f, D3D12_FLOAT32_MAX };

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

    class Device;

    class RootSignature
    {
    public:
        RootSignature(int numParams = 0);
        ~RootSignature();

        ID3D12RootSignature* Signature() const { return m_signature; }

        RootSignature& AppendRootConstant(u32 num32BitVals, u32 shaderRegister, u32 registerSpace = 0U, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
        RootSignature& AppendRootCBV(u32 shaderRegister, u32 registerSpace = 0U, D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
        RootSignature& AppendRootSRV(u32 shaderRegister, u32 registerSpace = 0U, D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
        RootSignature& AppendRootUAV(u32 shaderRegister, u32 registerSpace = 0U, D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
        RootSignature& AppendRootDescriptorTable(DescriptorTable& descTable, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
        
        RootSignature& AppendAnisotropicWrapSampler(u32 shaderRegister);
        RootSignature& AppendAnisotropicClampSampler(u32 shaderRegister);
        RootSignature& AppendPointWrapSampler(u32 shaderRegister);
        RootSignature& AppendPointClampSampler(u32 shaderRegister);
        RootSignature& AppendLinearWrapSampler(u32 shaderRegister);
        RootSignature& AppendLinearClampSampler(u32 shaderRegister);

        RootSignature& AppendStaticSampler(u32 shaderRegister, const SamplerParams* pParams, u32 registerSpace = 0U, D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL);
        RootSignature& AppendStaticSampler(u32 shaderRegister,
            u32 registerSpace = 0U, 
            D3D12_FILTER filter = D3D12_FILTER_ANISOTROPIC,
            D3D12_TEXTURE_ADDRESS_MODE addressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE addressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE addressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            FLOAT mipLODBias = 0,
            UINT maxAnisotropy = 16,
            D3D12_COMPARISON_FUNC comparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL,
            D3D12_STATIC_BORDER_COLOR borderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
            FLOAT minLOD = 0.f,
            FLOAT maxLOD = D3D12_FLOAT32_MAX,
            D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL);

        bool Finalize(std::shared_ptr<const Device> pDevice);

        RootSignature& AllowInputLayout() { return SetFlag(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT); }
        RootSignature& AllowStreamOutput() { return SetFlag(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT); }
        RootSignature& DenyVS() { return SetFlag(D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS); }
        RootSignature& DenyHS() { return SetFlag(D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS); }
        RootSignature& DenyDS() { return SetFlag(D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS); }
        RootSignature& DenyGS() { return SetFlag(D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS); }
        RootSignature& DenyPS() { return SetFlag(D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS); }

        RootSignature& SetFlag(D3D12_ROOT_SIGNATURE_FLAGS flag);
        void SetFlags(D3D12_ROOT_SIGNATURE_FLAGS flags) { m_flags = flags; }

        inline RootSignature& AppendVSRootCBV(u32 shaderRegister, u32 registerSpace = 0U, D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE) { return AppendRootCBV(shaderRegister, registerSpace, flags, D3D12_SHADER_VISIBILITY_VERTEX); }
        inline RootSignature& AppendHSRootCBV(u32 shaderRegister, u32 registerSpace = 0U, D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE) { return AppendRootCBV(shaderRegister, registerSpace, flags, D3D12_SHADER_VISIBILITY_HULL); }
        inline RootSignature& AppendDSRootCBV(u32 shaderRegister, u32 registerSpace = 0U, D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE) { return AppendRootCBV(shaderRegister, registerSpace, flags, D3D12_SHADER_VISIBILITY_DOMAIN); }
        inline RootSignature& AppendGSRootCBV(u32 shaderRegister, u32 registerSpace = 0U, D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE) { return AppendRootCBV(shaderRegister, registerSpace, flags, D3D12_SHADER_VISIBILITY_GEOMETRY); }
        inline RootSignature& AppendPSRootCBV(u32 shaderRegister, u32 registerSpace = 0U, D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE) { return AppendRootCBV(shaderRegister, registerSpace, flags, D3D12_SHADER_VISIBILITY_PIXEL); }

    protected:
        friend class Material::Immutable;

    private:
        ID3D12RootSignature* m_signature;
        D3D12_ROOT_SIGNATURE_FLAGS m_flags;
        std::vector<CD3DX12_ROOT_PARAMETER1> m_rootParameters;
        std::vector<CD3DX12_STATIC_SAMPLER_DESC> m_staticSamplers;
    };
}

#endif
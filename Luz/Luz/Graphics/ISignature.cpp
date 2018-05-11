#include "stdafx.h"
#include "ISignature.h"
#include "MaterialSignature.h"
#include "DirectX12\Device.h"

namespace Graphics
{
    namespace Material
    {
        ISignature::ISignature()
        {

        }

        ISignature::~ISignature()
        {

        }
    }
}

#if _WIN64 || _WIN32

#ifndef __D3DX12_H__
#include "d3dx12.h"
#endif

namespace Dx12
{
    static D3D12_SHADER_VISIBILITY ConvertVisibility(const Graphics::Material::ShaderVisibility& visibility)
    {
        switch (visibility)
        {
        case Graphics::Material::ShaderVisibility::SHADER_VISIBILITY_ALL: return D3D12_SHADER_VISIBILITY_ALL;
        case Graphics::Material::ShaderVisibility::SHADER_VISIBILITY_VS: return D3D12_SHADER_VISIBILITY_VERTEX;
        case Graphics::Material::ShaderVisibility::SHADER_VISIBILITY_HS: return D3D12_SHADER_VISIBILITY_HULL;
        case Graphics::Material::ShaderVisibility::SHADER_VISIBILITY_DS: return D3D12_SHADER_VISIBILITY_DOMAIN;
        case Graphics::Material::ShaderVisibility::SHADER_VISIBILITY_GS: return D3D12_SHADER_VISIBILITY_GEOMETRY;
        case Graphics::Material::ShaderVisibility::SHADER_VISIBILITY_PS: return D3D12_SHADER_VISIBILITY_PIXEL;
        }

        return (D3D12_SHADER_VISIBILITY)-1;
    }

    static D3D12_ROOT_DESCRIPTOR_FLAGS ConvertDescriptorFlags(const Graphics::Material::Parameter::DataFlags& dataFlags)
    {
        D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE;

        if ((dataFlags & Graphics::Material::Parameter::DataFlags::PARAMETER_DATA_FLAG_VOLATILE) != 0) flags &= D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE;
        if ((dataFlags & Graphics::Material::Parameter::DataFlags::PARAMETER_DATA_FLAG_STATIC_EXECUTE) != 0) flags &= D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE;
        if ((dataFlags & Graphics::Material::Parameter::DataFlags::PARAMETER_DATA_FLAG_STATIC) != 0) flags &= D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC;

        return flags;
    }

    static D3D12_FILTER ConvertFilterType(const Graphics::Material::FilterType& filter)
    {
        switch (filter)
        {
        case Graphics::Material::FilterType::FILTER_TYPE_MIN_MAG_MIP_POINT: return D3D12_FILTER_MIN_MAG_MIP_POINT;
        case Graphics::Material::FilterType::FILTER_TYPE_MIN_MAG_MIP_LINEAR: return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        case Graphics::Material::FilterType::FILTER_TYPE_ANISOTROPIC: return D3D12_FILTER_ANISOTROPIC;
        }

        return (D3D12_FILTER)-1;
    }

    static D3D12_TEXTURE_ADDRESS_MODE ConvertAddressType(const Graphics::Material::AddressType& address)
    {
        switch (address)
        {
        case Graphics::Material::AddressType::ADDRESS_TYPE_WRAP: return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        case Graphics::Material::AddressType::ADDRESS_TYPE_MIRROR: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        case Graphics::Material::AddressType::ADDRESS_TYPE_CLAMP: return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        }

        return (D3D12_TEXTURE_ADDRESS_MODE)-1;
    }

    static D3D12_COMPARISON_FUNC ConvertComparisonType(const Graphics::Material::ComparisonType& comparison)
    {
        switch (comparison)
        {
        case Graphics::Material::ComparisonType::COMPARISON_TYPE_NEVER: return D3D12_COMPARISON_FUNC_NEVER;
        case Graphics::Material::ComparisonType::COMPARISON_TYPE_LESS: return D3D12_COMPARISON_FUNC_LESS;
        case Graphics::Material::ComparisonType::COMPARISON_TYPE_EQUAL: return D3D12_COMPARISON_FUNC_EQUAL;
        case Graphics::Material::ComparisonType::COMPARISON_TYPE_LESS_EQUAL: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
        case Graphics::Material::ComparisonType::COMPARISON_TYPE_GREATER: return D3D12_COMPARISON_FUNC_GREATER;
        case Graphics::Material::ComparisonType::COMPARISON_TYPE_NOT_EQUAL: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
        case Graphics::Material::ComparisonType::COMPARISON_TYPE_GREATER_EQUAL: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
        case Graphics::Material::ComparisonType::COMPARISON_TYPE_ALWAYS: return D3D12_COMPARISON_FUNC_ALWAYS;
        }

        return (D3D12_COMPARISON_FUNC)-1;
    }

    static D3D12_STATIC_BORDER_COLOR ConvertBorderColorType(const Graphics::Material::BorderColorType& borderColor)
    {
        switch (borderColor)
        {
        case Graphics::Material::BorderColorType::BORDER_COLOR_TYPE_TRANSPARENT_BLACK: return D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        case Graphics::Material::BorderColorType::BORDER_COLOR_TYPE_OPAQUE_WHITE: return D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
        case Graphics::Material::BorderColorType::BORDER_COLOR_TYPE_OPAQUE_BLACK: return D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
        }

        return (D3D12_STATIC_BORDER_COLOR)-1;
    }

    static D3D12_ROOT_SIGNATURE_FLAGS ConvertSignatureFlags(const Graphics::Material::Signature::Flags& signatureFlags)
    {
        D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

        if ((signatureFlags & Graphics::Material::Signature::Flags::SIGNATURE_FLAG_ALLOW_INPUT_LAYOUT) != 0) flags &= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        if ((signatureFlags & Graphics::Material::Signature::Flags::SIGNATURE_FLAG_DENY_VS) != 0) flags &= D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
        if ((signatureFlags & Graphics::Material::Signature::Flags::SIGNATURE_FLAG_DENY_HS) != 0) flags &= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
        if ((signatureFlags & Graphics::Material::Signature::Flags::SIGNATURE_FLAG_DENY_DS) != 0) flags &= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
        if ((signatureFlags & Graphics::Material::Signature::Flags::SIGNATURE_FLAG_DENY_GS) != 0) flags &= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
        if ((signatureFlags & Graphics::Material::Signature::Flags::SIGNATURE_FLAG_DENY_PS) != 0) flags &= D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
        if ((signatureFlags & Graphics::Material::Signature::Flags::SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT) != 0) flags &= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;

        return flags;
    }

    Signature::Signature()
        : m_signature(nullptr)
    {

    }

    Signature::~Signature()
    {

    }

    bool Signature::Initialize(const Graphics::Material::Signature& signature)
    {
        std::vector<std::vector<CD3DX12_DESCRIPTOR_RANGE1>> paramRanges;    // Just here to keep ranges in scope until finalizing the root signature
        std::vector<CD3DX12_ROOT_PARAMETER1> params;

        u32 numParams = signature.NumParams();
        params.reserve((size_t)numParams);

        for (u32 i = 0; i < numParams; ++i)
        {
            params.emplace_back();
            auto& param = signature.GetParam(i);
            auto paramType = param.GetType();

            switch (paramType)
            {
            case Graphics::Material::Parameter::Type::PARAMETER_TYPE_CONSTANTS:
                params.back().InitAsConstants(param.GetConstantNum32BitValues(), param.GetConstantRegister(), param.GetConstantRegisterSpace(), ConvertVisibility(param.GetVisibility()));
                break;
            case Graphics::Material::Parameter::Type::PARAMETER_TYPE_CONSTANT_VIEW:
                params.back().InitAsConstantBufferView(param.GetDescriptorRegister(), param.GetDescriptorRegisterSpace(), ConvertDescriptorFlags(param.GetDataFlags()), ConvertVisibility(param.GetVisibility()));
                break;
            case Graphics::Material::Parameter::Type::PARAMETER_TYPE_SHADER_VIEW:
                params.back().InitAsShaderResourceView(param.GetDescriptorRegister(), param.GetDescriptorRegisterSpace(), ConvertDescriptorFlags(param.GetDataFlags()), ConvertVisibility(param.GetVisibility()));
                break;
            case Graphics::Material::Parameter::Type::PARAMETER_TYPE_COMPUTE_VIEW:
                params.back().InitAsUnorderedAccessView(param.GetDescriptorRegister(), param.GetDescriptorRegisterSpace(), ConvertDescriptorFlags(param.GetDataFlags()), ConvertVisibility(param.GetVisibility()));
                break;
            case Graphics::Material::Parameter::Type::PARAMETER_TYPE_DESCRIPTOR_TABLE:
            {
                paramRanges.emplace_back();
                auto& ranges = paramRanges.back();

                u32 numRanges = param.NumDescriptorTableRanges();
                ranges.reserve((size_t)numRanges);

                for (u32 j = 0; j < numRanges; ++j)
                {
                    ranges.emplace_back();
                    auto& range = param.GetDescriptorTableRange(j);
                    auto rangeType = range.Kind;

                    switch (rangeType)
                    {
                    case Graphics::Material::DescriptorTable::Range::Type::DESCRIPTOR_TABLE_RANGE_TYPE_CONSTANT_VIEW:
                        ranges.back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, range.NumDescriptors, range.BaseRegister, range.RegisterSpace, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
                        break;
                    case Graphics::Material::DescriptorTable::Range::Type::DESCRIPTOR_TABLE_RANGE_TYPE_SHADER_VIEW:
                        ranges.back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, range.NumDescriptors, range.BaseRegister, range.RegisterSpace, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
                        break;
                    case Graphics::Material::DescriptorTable::Range::Type::DESCRIPTOR_TABLE_RANGE_TYPE_COMPUTE_VIEW:
                        ranges.back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, range.NumDescriptors, range.BaseRegister, range.RegisterSpace, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
                        break;
                    case Graphics::Material::DescriptorTable::Range::Type::DESCRIPTOR_TABLE_RANGE_TYPE_SAMPLER:
                        ranges.back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, range.NumDescriptors, range.BaseRegister, range.RegisterSpace, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
                        break;
                    }
                }

                params.back().InitAsDescriptorTable((UINT)ranges.size(), ranges.data(), ConvertVisibility(param.GetVisibility()));
                break;
            }
            }
        }

        u32 numSamplers = signature.NumSamplers();

        std::vector<CD3DX12_STATIC_SAMPLER_DESC> samplers;
        samplers.reserve((u32)numSamplers);

        for (u32 i = 0; i < numSamplers; ++i)
        {
            samplers.emplace_back();
            auto& sampler = signature.GetSampler(i);

            samplers
                .back()
                .Init(sampler.Register
                    , ConvertFilterType(sampler.Filter)
                    , ConvertAddressType(sampler.AddressU)
                    , ConvertAddressType(sampler.AddressV)
                    , ConvertAddressType(sampler.AddressW)
                    , (FLOAT)sampler.MipLodBias
                    , (UINT)sampler.MaxAnisotropy
                    , ConvertComparisonType(sampler.Comparison)
                    , ConvertBorderColorType(sampler.BorderColor)
                    , (FLOAT)sampler.MinLod
                    , (FLOAT)sampler.MaxLod
                    , ConvertVisibility(sampler.Visibility));
        }

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc;
        desc.Init_1_1((UINT)params.size(), params.data(), (UINT)samplers.size(), samplers.data(), ConvertSignatureFlags(signature.GetFlags()));
        
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
}

#elif __APPLE__

// Convert to native graphics api

#endif
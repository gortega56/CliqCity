#include "stdafx.h"
#include "GraphicsTypes_Dx12.h"

namespace Graphics
{
    DXGI_FORMAT GetDxgiFormat(const Format format)
    {
        switch (format)
        {
        case (GFX_FORMAT_UNKNOWN): return DXGI_FORMAT_UNKNOWN;
        case (GFX_FORMAT_R32G32B32A32_TYPELESS): return DXGI_FORMAT_R32G32B32A32_TYPELESS;
        case (GFX_FORMAT_R32G32B32A32_FLOAT): return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case (GFX_FORMAT_R32G32B32A32_UINT): return DXGI_FORMAT_R32G32B32A32_UINT;
        case (GFX_FORMAT_R32G32B32A32_SINT): return DXGI_FORMAT_R32G32B32A32_SINT;
        case (GFX_FORMAT_R32G32B32_TYPELESS): return DXGI_FORMAT_R32G32B32_TYPELESS;
        case (GFX_FORMAT_R32G32B32_FLOAT): return DXGI_FORMAT_R32G32B32_FLOAT;
        case (GFX_FORMAT_R32G32B32_UINT): return DXGI_FORMAT_R32G32B32_UINT;
        case (GFX_FORMAT_R32G32B32_SINT): return DXGI_FORMAT_R32G32B32_SINT;
        case (GFX_FORMAT_R16G16B16A16_TYPELESS): return DXGI_FORMAT_R16G16B16A16_TYPELESS;
        case (GFX_FORMAT_R16G16B16A16_FLOAT): return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case (GFX_FORMAT_R16G16B16A16_UNORM): return DXGI_FORMAT_R16G16B16A16_UNORM;
        case (GFX_FORMAT_R16G16B16A16_UINT): return DXGI_FORMAT_R16G16B16A16_UINT;
        case (GFX_FORMAT_R16G16B16A16_SNORM): return DXGI_FORMAT_R16G16B16A16_SNORM;
        case (GFX_FORMAT_R16G16B16A16_SINT): return DXGI_FORMAT_R16G16B16A16_SINT;
        case (GFX_FORMAT_R32G32_TYPELESS): return DXGI_FORMAT_R32G32_TYPELESS;
        case (GFX_FORMAT_R32G32_FLOAT): return DXGI_FORMAT_R32G32_FLOAT;
        case (GFX_FORMAT_R32G32_UINT): return DXGI_FORMAT_R32G32_UINT;
        case (GFX_FORMAT_R32G32_SINT): return DXGI_FORMAT_R32G32_SINT;
        case (GFX_FORMAT_R32G8X24_TYPELESS): return DXGI_FORMAT_R32G8X24_TYPELESS;
        case (GFX_FORMAT_D32_FLOAT_S8X24_UINT): return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        case (GFX_FORMAT_R32_FLOAT_X8X24_TYPELESS): return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
        case (GFX_FORMAT_X32_TYPELESS_G8X24_UINT): return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
        case (GFX_FORMAT_R10G10B10A2_TYPELESS): return DXGI_FORMAT_R10G10B10A2_TYPELESS;
        case (GFX_FORMAT_R10G10B10A2_UNORM): return DXGI_FORMAT_R10G10B10A2_UNORM;
        case (GFX_FORMAT_R10G10B10A2_UINT): return DXGI_FORMAT_R10G10B10A2_UINT;
        case (GFX_FORMAT_R11G11B10_FLOAT): return DXGI_FORMAT_R11G11B10_FLOAT;
        case (GFX_FORMAT_R8G8B8A8_TYPELESS): return DXGI_FORMAT_R8G8B8A8_TYPELESS;
        case (GFX_FORMAT_R8G8B8A8_UNORM): return DXGI_FORMAT_R8G8B8A8_UNORM;
        case (GFX_FORMAT_R8G8B8A8_UNORM_SRGB): return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case (GFX_FORMAT_R8G8B8A8_UINT): return DXGI_FORMAT_R8G8B8A8_UINT;
        case (GFX_FORMAT_R8G8B8A8_SNORM): return DXGI_FORMAT_R8G8B8A8_SNORM;
        case (GFX_FORMAT_R8G8B8A8_SINT): return DXGI_FORMAT_R8G8B8A8_SINT;
        case (GFX_FORMAT_R16G16_TYPELESS): return DXGI_FORMAT_R16G16_TYPELESS;
        case (GFX_FORMAT_R16G16_FLOAT): return DXGI_FORMAT_R16G16_FLOAT;
        case (GFX_FORMAT_R16G16_UNORM): return DXGI_FORMAT_R16G16_UNORM;
        case (GFX_FORMAT_R16G16_UINT): return DXGI_FORMAT_R16G16_UINT;;
        case (GFX_FORMAT_R16G16_SNORM): return DXGI_FORMAT_R16G16_SNORM;
        case (GFX_FORMAT_R16G16_SINT): return DXGI_FORMAT_R16G16_SINT;
        case (GFX_FORMAT_R32_TYPELESS): return DXGI_FORMAT_R32_TYPELESS;
        case (GFX_FORMAT_D32_FLOAT): return DXGI_FORMAT_D32_FLOAT;
        case (GFX_FORMAT_R32_FLOAT): return DXGI_FORMAT_R32_FLOAT;
        case (GFX_FORMAT_R32_UINT): return DXGI_FORMAT_R32_UINT;
        case (GFX_FORMAT_R32_SINT): return DXGI_FORMAT_R32_SINT;
        case (GFX_FORMAT_R24G8_TYPELESS): return DXGI_FORMAT_R24G8_TYPELESS;
        case (GFX_FORMAT_D24_UNORM_S8_UINT): return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case (GFX_FORMAT_R24_UNORM_X8_TYPELESS): return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        case (GFX_FORMAT_X24_TYPELESS_G8_UINT): return DXGI_FORMAT_X24_TYPELESS_G8_UINT;
        case (GFX_FORMAT_R8G8_TYPELESS): return DXGI_FORMAT_R8G8_TYPELESS;
        case (GFX_FORMAT_R8G8_UNORM): return DXGI_FORMAT_R8G8_UNORM;
        case (GFX_FORMAT_R8G8_UINT): return DXGI_FORMAT_R8G8_UINT;
        case (GFX_FORMAT_R8G8_SNORM): return DXGI_FORMAT_R8G8_SNORM;
        case (GFX_FORMAT_R8G8_SINT): return DXGI_FORMAT_R8G8_SINT;
        case (GFX_FORMAT_R16_TYPELESS): return DXGI_FORMAT_R16_TYPELESS;
        case (GFX_FORMAT_R16_FLOAT): return DXGI_FORMAT_R16_FLOAT;
        case (GFX_FORMAT_D16_UNORM): return DXGI_FORMAT_D16_UNORM;
        case (GFX_FORMAT_R16_UNORM): return DXGI_FORMAT_R16_UNORM;
        case (GFX_FORMAT_R16_UINT): return DXGI_FORMAT_R16_UINT;
        case (GFX_FORMAT_R16_SNORM): return DXGI_FORMAT_R16_SNORM;
        case (GFX_FORMAT_R16_SINT): return DXGI_FORMAT_R16_SINT;
        case (GFX_FORMAT_R8_TYPELESS): return DXGI_FORMAT_R8_TYPELESS;
        case (GFX_FORMAT_R8_UNORM): return DXGI_FORMAT_R8_UNORM;
        case (GFX_FORMAT_R8_UINT): return DXGI_FORMAT_R8_UINT;;
        case (GFX_FORMAT_R8_SNORM): return DXGI_FORMAT_R8_SNORM;
        case (GFX_FORMAT_R8_SINT): return DXGI_FORMAT_R8_SINT;
        case (GFX_FORMAT_A8_UNORM): return DXGI_FORMAT_A8_UNORM;
        case (GFX_FORMAT_R1_UNORM): return DXGI_FORMAT_R1_UNORM;
        case (GFX_FORMAT_R9G9B9E5_SHAREDEXP): return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
        case (GFX_FORMAT_R8G8_B8G8_UNORM): return DXGI_FORMAT_R8G8_B8G8_UNORM;
        case (GFX_FORMAT_G8R8_G8B8_UNORM): return DXGI_FORMAT_G8R8_G8B8_UNORM;
        case (GFX_FORMAT_BC1_TYPELESS): return DXGI_FORMAT_BC1_TYPELESS;
        case (GFX_FORMAT_BC1_UNORM): return DXGI_FORMAT_BC1_UNORM;
        case (GFX_FORMAT_BC1_UNORM_SRGB): return DXGI_FORMAT_BC1_UNORM_SRGB;
        case (GFX_FORMAT_BC2_TYPELESS): return DXGI_FORMAT_BC2_TYPELESS;
        case (GFX_FORMAT_BC2_UNORM): return DXGI_FORMAT_BC2_UNORM;
        case (GFX_FORMAT_BC2_UNORM_SRGB): return DXGI_FORMAT_BC2_UNORM_SRGB;
        case (GFX_FORMAT_BC3_TYPELESS): return DXGI_FORMAT_BC3_TYPELESS;;
        case (GFX_FORMAT_BC3_UNORM): return DXGI_FORMAT_BC3_UNORM;
        case (GFX_FORMAT_BC3_UNORM_SRGB): return DXGI_FORMAT_BC3_UNORM_SRGB;
        case (GFX_FORMAT_BC4_TYPELESS): return DXGI_FORMAT_BC4_TYPELESS;
        case (GFX_FORMAT_BC4_UNORM): return DXGI_FORMAT_BC4_UNORM;
        case (GFX_FORMAT_BC4_SNORM): return DXGI_FORMAT_BC4_SNORM;
        case (GFX_FORMAT_BC5_TYPELESS): return DXGI_FORMAT_BC5_TYPELESS;
        case (GFX_FORMAT_BC5_UNORM): return DXGI_FORMAT_BC5_UNORM;
        case (GFX_FORMAT_BC5_SNORM): return DXGI_FORMAT_BC5_SNORM;
        case (GFX_FORMAT_B5G6R5_UNORM): return DXGI_FORMAT_B5G6R5_UNORM;
        case (GFX_FORMAT_B5G5R5A1_UNORM): return DXGI_FORMAT_B5G5R5A1_UNORM;
        case (GFX_FORMAT_B8G8R8A8_UNORM): return DXGI_FORMAT_B8G8R8A8_UNORM;
        case (GFX_FORMAT_B8G8R8X8_UNORM): return DXGI_FORMAT_B8G8R8X8_UNORM;
        case (GFX_FORMAT_R10G10B10_XR_BIAS_A2_UNORM): return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
        case (GFX_FORMAT_B8G8R8A8_TYPELESS): return DXGI_FORMAT_B8G8R8A8_TYPELESS;
        case (GFX_FORMAT_B8G8R8A8_UNORM_SRGB): return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        case (GFX_FORMAT_B8G8R8X8_TYPELESS): return DXGI_FORMAT_B8G8R8X8_TYPELESS;
        case (GFX_FORMAT_B8G8R8X8_UNORM_SRGB): return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
        case (GFX_FORMAT_BC6H_TYPELESS): return DXGI_FORMAT_BC6H_TYPELESS;
        case (GFX_FORMAT_BC6H_UF16): return DXGI_FORMAT_BC6H_UF16;
        case (GFX_FORMAT_BC6H_SF16): return DXGI_FORMAT_BC6H_SF16;
        case (GFX_FORMAT_BC7_TYPELESS): return DXGI_FORMAT_BC7_TYPELESS;
        case (GFX_FORMAT_BC7_UNORM): return DXGI_FORMAT_BC7_UNORM;
        case (GFX_FORMAT_BC7_UNORM_SRGB): return DXGI_FORMAT_BC7_UNORM_SRGB;
        case (GFX_FORMAT_AYUV): return DXGI_FORMAT_AYUV;
        case (GFX_FORMAT_Y410): return DXGI_FORMAT_Y410;
        case (GFX_FORMAT_Y416): return DXGI_FORMAT_Y416;
        case (GFX_FORMAT_NV12): return DXGI_FORMAT_NV12;
        case (GFX_FORMAT_P010): return DXGI_FORMAT_P010;
        case (GFX_FORMAT_P016): return DXGI_FORMAT_P016;
        case (GFX_FORMAT_420_OPAQUE): return DXGI_FORMAT_420_OPAQUE;
        case (GFX_FORMAT_YUY2): return DXGI_FORMAT_YUY2;
        case (GFX_FORMAT_Y210): return DXGI_FORMAT_Y210;
        case (GFX_FORMAT_Y216): return DXGI_FORMAT_Y216;
        case (GFX_FORMAT_NV11): return DXGI_FORMAT_NV11;
        case (GFX_FORMAT_AI44): return DXGI_FORMAT_AI44;
        case (GFX_FORMAT_IA44): return DXGI_FORMAT_IA44;
        case (GFX_FORMAT_P8): return DXGI_FORMAT_P8;
        case (GFX_FORMAT_A8P8): return DXGI_FORMAT_A8P8;
        case (GFX_FORMAT_B4G4R4A4_UNORM): return DXGI_FORMAT_B4G4R4A4_UNORM;
        case (GFX_FORMAT_P208): return DXGI_FORMAT_P208;
        case (GFX_FORMAT_V208): return DXGI_FORMAT_V208;
        case (GFX_FORMAT_V408): return DXGI_FORMAT_V408;
        case (GFX_FORMAT_FORCE_UINT): return DXGI_FORMAT_FORCE_UINT;
        }

        return (DXGI_FORMAT)-1;
    }

    D3D12_INPUT_CLASSIFICATION GetD3D12InputClassification(const InputLayoutDesc::Element::Type elementType)
    {
        switch (elementType)
        {
        case(InputLayoutDesc::Element::INPUT_LAYOUT_ELEMENT_TYPE_PER_VERTEX) : return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        case(InputLayoutDesc::Element::INPUT_LAYOUT_ELEMENT_TYPE_PER_INSTANCE): return D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
        }

        return (D3D12_INPUT_CLASSIFICATION)-1;
    }

    D3D12_PRIMITIVE_TOPOLOGY_TYPE GetD3D12PrimitiveTopoglogyType(const PrimitiveTopologyType topology)
    {
        switch (topology)
        {
        case (GFX_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED): return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
        case (GFX_PRIMITIVE_TOPOLOGY_TYPE_POINT): return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
        case (GFX_PRIMITIVE_TOPOLOGY_TYPE_LINE): return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        case (GFX_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE): return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        case (GFX_PRIMITIVE_TOPOLOGY_TYPE_PATCH): return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
        }

        return (D3D12_PRIMITIVE_TOPOLOGY_TYPE)-1;
    }

    D3D12_SHADER_VISIBILITY GetVisibility(const ShaderVisibility& visibility)
    {
        switch (visibility)
        {
        case ShaderVisibility::SHADER_VISIBILITY_ALL: return D3D12_SHADER_VISIBILITY_ALL;
        case ShaderVisibility::SHADER_VISIBILITY_VS: return D3D12_SHADER_VISIBILITY_VERTEX;
        case ShaderVisibility::SHADER_VISIBILITY_HS: return D3D12_SHADER_VISIBILITY_HULL;
        case ShaderVisibility::SHADER_VISIBILITY_DS: return D3D12_SHADER_VISIBILITY_DOMAIN;
        case ShaderVisibility::SHADER_VISIBILITY_GS: return D3D12_SHADER_VISIBILITY_GEOMETRY;
        case ShaderVisibility::SHADER_VISIBILITY_PS: return D3D12_SHADER_VISIBILITY_PIXEL;
        }

        return (D3D12_SHADER_VISIBILITY)-1;
    }

    D3D12_ROOT_DESCRIPTOR_FLAGS GetDescriptorFlags(const Parameter::DataFlags& dataFlags)
    {
        D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE;

        if ((dataFlags & Parameter::DataFlags::PARAMETER_DATA_FLAG_VOLATILE) != 0) flags &= D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE;
        if ((dataFlags & Parameter::DataFlags::PARAMETER_DATA_FLAG_STATIC_EXECUTE) != 0) flags &= D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE;
        if ((dataFlags & Parameter::DataFlags::PARAMETER_DATA_FLAG_STATIC) != 0) flags &= D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC;

        return flags;
    }

    D3D12_FILTER GetFilterType(const FilterType& filter)
    {
        switch (filter)
        {
        case (GFX_FILTER_MIN_MAG_MIP_POINT): return D3D12_FILTER_MIN_MAG_MIP_POINT;
        case (GFX_FILTER_MIN_MAG_POINT_MIP_LINEAR): return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
        case (GFX_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT): return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
        case (GFX_FILTER_MIN_POINT_MAG_MIP_LINEAR): return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
        case (GFX_FILTER_MIN_LINEAR_MAG_MIP_POINT): return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
        case (GFX_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR): return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        case (GFX_FILTER_MIN_MAG_LINEAR_MIP_POINT): return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        case (GFX_FILTER_MIN_MAG_MIP_LINEAR): return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        case (GFX_FILTER_ANISOTROPIC): return D3D12_FILTER_ANISOTROPIC;
        case (GFX_FILTER_COMPARISON_MIN_MAG_MIP_POINT): return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
        case (GFX_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR): return D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
        case (GFX_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT): return D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
        case (GFX_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR): return D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
        case (GFX_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT): return D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
        case (GFX_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR): return D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        case (GFX_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT): return D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
        case (GFX_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR): return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
        case (GFX_FILTER_COMPARISON_ANISOTROPIC): return D3D12_FILTER_COMPARISON_ANISOTROPIC;
        case (GFX_FILTER_MINIMUM_MIN_MAG_MIP_POINT): return D3D12_FILTER_MINIMUM_MIN_MAG_MIP_POINT;
        case (GFX_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR): return D3D12_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR;
        case (GFX_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT): return D3D12_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
        case (GFX_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR): return D3D12_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR;
        case (GFX_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT): return D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT;
        case (GFX_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR): return D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        case (GFX_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT): return D3D12_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT;
        case (GFX_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR): return D3D12_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
        case (GFX_FILTER_MINIMUM_ANISOTROPIC): return D3D12_FILTER_MINIMUM_ANISOTROPIC;
        case (GFX_FILTER_MAXIMUM_MIN_MAG_MIP_POINT): return D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_POINT;
        case (GFX_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR): return D3D12_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR;
        case (GFX_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT): return D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
        case (GFX_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR): return D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR;
        case (GFX_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT): return D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT;
        case (GFX_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR): return D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        case (GFX_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT): return D3D12_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT;
        case (GFX_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR): return D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR;
        case (GFX_FILTER_MAXIMUM_ANISOTROPIC): return D3D12_FILTER_MAXIMUM_ANISOTROPIC;
        }

        return (D3D12_FILTER)-1;
    }

    D3D12_TEXTURE_ADDRESS_MODE GetAddressType(const AddressType& address)
    {
        switch (address)
        {
        case (GFX_TEXTURE_ADDRESS_MODE_WRAP): return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        case (GFX_TEXTURE_ADDRESS_MODE_MIRROR): return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        case (GFX_TEXTURE_ADDRESS_MODE_CLAMP): return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        case (GFX_TEXTURE_ADDRESS_MODE_BORDER): return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        case (GFX_TEXTURE_ADDRESS_MODE_MIRROR_ONCE): return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
        }

        return (D3D12_TEXTURE_ADDRESS_MODE)-1;
    }

    D3D12_COMPARISON_FUNC GetComparisonType(const ComparisonType& comparison)
    {
        switch (comparison)
        {
        case ComparisonType::COMPARISON_TYPE_NEVER: return D3D12_COMPARISON_FUNC_NEVER;
        case ComparisonType::COMPARISON_TYPE_LESS: return D3D12_COMPARISON_FUNC_LESS;
        case ComparisonType::COMPARISON_TYPE_EQUAL: return D3D12_COMPARISON_FUNC_EQUAL;
        case ComparisonType::COMPARISON_TYPE_LESS_EQUAL: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
        case ComparisonType::COMPARISON_TYPE_GREATER: return D3D12_COMPARISON_FUNC_GREATER;
        case ComparisonType::COMPARISON_TYPE_NOT_EQUAL: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
        case ComparisonType::COMPARISON_TYPE_GREATER_EQUAL: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
        case ComparisonType::COMPARISON_TYPE_ALWAYS: return D3D12_COMPARISON_FUNC_ALWAYS;
        }

        return (D3D12_COMPARISON_FUNC)-1;
    }

    D3D12_STATIC_BORDER_COLOR GetBorderColorType(const BorderColorType& borderColor)
    {
        switch (borderColor)
        {
        case BorderColorType::BORDER_COLOR_TYPE_TRANSPARENT_BLACK: return D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        case BorderColorType::BORDER_COLOR_TYPE_OPAQUE_WHITE: return D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
        case BorderColorType::BORDER_COLOR_TYPE_OPAQUE_BLACK: return D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
        }

        return (D3D12_STATIC_BORDER_COLOR)-1;
    }

    D3D12_ROOT_SIGNATURE_FLAGS GetSignatureFlags(const SignatureDesc::Flags& signatureFlags)
    {
        D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

        if ((signatureFlags & SignatureDesc::Flags::SIGNATURE_FLAG_ALLOW_INPUT_LAYOUT) != 0) flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        if ((signatureFlags & SignatureDesc::Flags::SIGNATURE_FLAG_DENY_VS) != 0) flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
        if ((signatureFlags & SignatureDesc::Flags::SIGNATURE_FLAG_DENY_HS) != 0) flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
        if ((signatureFlags & SignatureDesc::Flags::SIGNATURE_FLAG_DENY_DS) != 0) flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
        if ((signatureFlags & SignatureDesc::Flags::SIGNATURE_FLAG_DENY_GS) != 0) flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
        if ((signatureFlags & SignatureDesc::Flags::SIGNATURE_FLAG_DENY_PS) != 0) flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
        if ((signatureFlags & SignatureDesc::Flags::SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT) != 0) flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;

        return flags;
    }

    D3D12_BLEND GetD3D12Blend(const BlendType blend)
    {
        switch (blend)
        {
        case (GFX_BLEND_ZERO): return D3D12_BLEND_ZERO;
        case (GFX_BLEND_ONE): return D3D12_BLEND_ONE;
        case (GFX_BLEND_SRC_COLOR): return D3D12_BLEND_SRC_COLOR;
        case (GFX_BLEND_INV_SRC_COLOR): return D3D12_BLEND_INV_SRC_COLOR;
        case (GFX_BLEND_SRC_ALPHA): return D3D12_BLEND_SRC_ALPHA;
        case (GFX_BLEND_INV_SRC_ALPHA): return D3D12_BLEND_INV_SRC_ALPHA;
        case (GFX_BLEND_DEST_ALPHA): return D3D12_BLEND_DEST_ALPHA;
        case (GFX_BLEND_INV_DEST_ALPHA): return D3D12_BLEND_INV_DEST_ALPHA;
        case (GFX_BLEND_DEST_COLOR): return D3D12_BLEND_DEST_COLOR;
        case (GFX_BLEND_INV_DEST_COLOR): return D3D12_BLEND_INV_DEST_COLOR;
        case (GFX_BLEND_SRC_ALPHA_SAT): return D3D12_BLEND_SRC_ALPHA_SAT;
        case (GFX_BLEND_BLEND_FACTOR): return D3D12_BLEND_BLEND_FACTOR;
        case (GFX_BLEND_INV_BLEND_FACTOR): return D3D12_BLEND_INV_BLEND_FACTOR;
        case (GFX_BLEND_SRC1_COLOR): return D3D12_BLEND_SRC1_COLOR;
        case (GFX_BLEND_INV_SRC1_COLOR): return D3D12_BLEND_INV_SRC1_COLOR;
        case (GFX_BLEND_SRC1_ALPHA): return D3D12_BLEND_SRC1_ALPHA;
        case (GFX_BLEND_INV_SRC1_ALPHA): return D3D12_BLEND_INV_SRC1_ALPHA;
        }

        return (D3D12_BLEND)-1;
    }

    D3D12_BLEND_OP GetD3D12BlendOp(const BlendOpType op)
    {
        switch (op)
        {
        case (GFX_BLEND_OP_ADD): return D3D12_BLEND_OP_ADD;
        case (GFX_BLEND_OP_SUBTRACT): return D3D12_BLEND_OP_SUBTRACT;
        case (GFX_BLEND_OP_REV_SUBTRACT): return D3D12_BLEND_OP_REV_SUBTRACT;
        case (GFX_BLEND_OP_MIN): return D3D12_BLEND_OP_MIN;
        case (GFX_BLEND_OP_MAX): return D3D12_BLEND_OP_MAX;
        }

        return (D3D12_BLEND_OP)-1;
    }

    D3D12_LOGIC_OP GetD3D12LogicOp(const LogicOpType op)
    {
        switch (op)
        {
        case (GFX_LOGIC_OP_CLEAR): return D3D12_LOGIC_OP_CLEAR;
        case (GFX_LOGIC_OP_SET): return D3D12_LOGIC_OP_SET;
        case (GFX_LOGIC_OP_COPY): return D3D12_LOGIC_OP_COPY;
        case (GFX_LOGIC_OP_COPY_INVERTED): return D3D12_LOGIC_OP_COPY_INVERTED;
        case (GFX_LOGIC_OP_NOOP): return D3D12_LOGIC_OP_NOOP;
        case (GFX_LOGIC_OP_INVERT): return D3D12_LOGIC_OP_INVERT;
        case (GFX_LOGIC_OP_AND): return D3D12_LOGIC_OP_AND;
        case (GFX_LOGIC_OP_NAND): return D3D12_LOGIC_OP_NAND;
        case (GFX_LOGIC_OP_OR): return D3D12_LOGIC_OP_OR;
        case (GFX_LOGIC_OP_NOR): return D3D12_LOGIC_OP_NOR;
        case (GFX_LOGIC_OP_XOR): return D3D12_LOGIC_OP_XOR;
        case (GFX_LOGIC_OP_EQUIV): return D3D12_LOGIC_OP_EQUIV;
        case (GFX_LOGIC_OP_AND_REVERSE): return D3D12_LOGIC_OP_AND_REVERSE;
        case (GFX_LOGIC_OP_AND_INVERTED): return D3D12_LOGIC_OP_AND_INVERTED;
        case (GFX_LOGIC_OP_OR_REVERSE): return D3D12_LOGIC_OP_OR_REVERSE;
        case (GFX_LOGIC_OP_OR_INVERTED): return D3D12_LOGIC_OP_OR_INVERTED;
        }

        return (D3D12_LOGIC_OP)-1;
    }

    D3D12_COLOR_WRITE_ENABLE GetD3D12ColorWriteEnable(const ColorWriteEnable colorWrite)
    {
        switch (colorWrite)
        {
        case (GFX_COLOR_WRITE_ENABLE_RED): return D3D12_COLOR_WRITE_ENABLE_RED;
        case (GFX_COLOR_WRITE_ENABLE_GREEN): return D3D12_COLOR_WRITE_ENABLE_GREEN;
        case (GFX_COLOR_WRITE_ENABLE_BLUE): return D3D12_COLOR_WRITE_ENABLE_BLUE;
        case (GFX_COLOR_WRITE_ENABLE_ALPHA): return D3D12_COLOR_WRITE_ENABLE_ALPHA;
        case (GFX_COLOR_WRITE_ENABLE_ALL): return D3D12_COLOR_WRITE_ENABLE_ALL;
        }

        return (D3D12_COLOR_WRITE_ENABLE)-1;
    }


    D3D12_FILL_MODE GetD3D12FillMode(const FillMode fill)
    {
        switch (fill)
        {
        case (GFX_FILL_MODE_SOLID) : return D3D12_FILL_MODE_SOLID;
        case (GFX_FILL_MODE_WIREFRAME): return D3D12_FILL_MODE_WIREFRAME;
        }

        return (D3D12_FILL_MODE)-1;
    }

    D3D12_CULL_MODE GetD3D12CullMode(const CullMode cull)
    {
        switch (cull)
        {
        case (GFX_CULL_MODE_NONE): return D3D12_CULL_MODE_NONE;
        case (GFX_CULL_MODE_FRONT): return D3D12_CULL_MODE_FRONT;
        case (GFX_CULL_MODE_BACK): return D3D12_CULL_MODE_BACK;
        }

        return (D3D12_CULL_MODE)-1;
    }

    D3D12_CONSERVATIVE_RASTERIZATION_MODE GetD3D12RasterizationMode(const ConservativeRasterizationMode mode)
    {
        switch (mode)
        {
        case (GFX_CONSERVATIVE_RASTERIZATION_MODE_UNAVAILABLE): return D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
        case (GFX_CONSERVATIVE_RASTERIZATION_MODE_OFF): return D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
        case (GFX_CONSERVATIVE_RASTERIZATION_MODE_ON): return D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON;
        }

        return (D3D12_CONSERVATIVE_RASTERIZATION_MODE)-1;
    }


    D3D12_DEPTH_WRITE_MASK GetD3D12DepthWriteMask(const DepthWriteMask mask)
    {
        switch (mask)
        {
        case (GFX_DEPTH_WRITE_MASK_ZERO): return D3D12_DEPTH_WRITE_MASK_ZERO;
        case (GFX_DEPTH_WRITE_MASK_ALL): return D3D12_DEPTH_WRITE_MASK_ALL;
        }

        return (D3D12_DEPTH_WRITE_MASK)-1;
    }

    D3D12_STENCIL_OP GetD3D12StencilOp(const StencilOp op)
    {
        switch (op)
        {
        case (GFX_STENCIL_OP_KEEP): return D3D12_STENCIL_OP_KEEP;
        case (GFX_STENCIL_OP_ZERO): return D3D12_STENCIL_OP_ZERO;
        case (GFX_STENCIL_OP_REPLACE): return D3D12_STENCIL_OP_REPLACE;
        case (GFX_STENCIL_OP_INCR_SAT): return D3D12_STENCIL_OP_INCR_SAT;
        case (GFX_STENCIL_OP_DECR_SAT): return D3D12_STENCIL_OP_DECR_SAT;
        case (GFX_STENCIL_OP_INVERT): return D3D12_STENCIL_OP_INVERT;
        case (GFX_STENCIL_OP_INCR): return D3D12_STENCIL_OP_INCR;
        case (GFX_STENCIL_OP_DECR): return D3D12_STENCIL_OP_DECR;
        }

        return (D3D12_STENCIL_OP)-1;
    }

    D3D12_PRIMITIVE_TOPOLOGY GetD3D12PrimitiveTogopology(const PrimitiveSubtopology topology)
    {
        switch (topology)
        {
        case (GFX_PRIMITIVE_TOPOLOGY_UNDEFINED): return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
        case (GFX_PRIMITIVE_TOPOLOGY_POINTLIST): return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_LINELIST): return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
        case (GFX_PRIMITIVE_TOPOLOGY_LINESTRIP): return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
        case (GFX_PRIMITIVE_TOPOLOGY_TRIANGLELIST): return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        case (GFX_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP): return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        case (GFX_PRIMITIVE_TOPOLOGY_LINELIST_ADJ): return D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
        case (GFX_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ): return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
        case (GFX_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ): return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
        case (GFX_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ): return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
        case (GFX_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST;
        case (GFX_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST): return D3D_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST;
        }

        return (D3D12_PRIMITIVE_TOPOLOGY)-1;
    }

    D3D12_RESOURCE_FLAGS GetD3D12ResourceFlags(const ResourceFlags resourceFlags)
    {
        D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

        if (resourceFlags & GFX_RESOURCE_FLAG_ALLOW_RENDER_TARGET) flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        if (resourceFlags & GFX_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        if (resourceFlags & GFX_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        if (resourceFlags & GFX_RESOURCE_FLAG_DENY_SHADER_RESOURCE) flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
        if (resourceFlags & GFX_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER) flags |= D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER;
        if (resourceFlags & GFX_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS) flags |= D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;
        if (resourceFlags & GFX_RESOURCE_FLAG_VIDEO_DECODE_REFERENCE_ONLY) flags |= D3D12_RESOURCE_FLAG_VIDEO_DECODE_REFERENCE_ONLY;

        return flags;
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC GetD3D12ShaderResourceViewDesc(const TextureDesc& td)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC desc;
        ZeroMemory(&desc, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
        desc.Format = GetDxgiFormat(td.Format);
        desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

        if (td.Dimension == GFX_RESOURCE_DIMENSION_TEXTURE1D)
        {
            if (td.Depth > 1)
            {
                desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
                desc.Texture1DArray.ArraySize = td.Depth;
                desc.Texture1DArray.MipLevels = td.MipLevels;
            }
            else
            {
                desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
                desc.Texture1D.MipLevels = td.MipLevels;
            }
        }
        else if (td.Dimension == GFX_RESOURCE_DIMENSION_TEXTURE2D)
        {
            if (td.bIsCube)
            {
                LUZASSERT(td.Depth % 6 == 0);
                if (td.Depth > 6)
                {
                    desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
                    desc.TextureCubeArray.NumCubes = td.Depth / 6;
                    desc.TextureCubeArray.MipLevels = td.MipLevels;
                }
                else
                {
                    desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
                    desc.TextureCube.MipLevels = td.MipLevels;
                }
            }
            else
            {
                if (td.Depth > 1)
                {
                    desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
                    desc.Texture2DArray.ArraySize = td.Depth;
                    desc.Texture2DArray.MipLevels = td.MipLevels;
                }
                else
                {
                    desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                    desc.Texture2D.MipLevels = td.MipLevels;
                }
            }
            
        }
        else if (td.Dimension == GFX_RESOURCE_DIMENSION_TEXTURE3D)
        {
            desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
            desc.Texture3D.MipLevels = td.MipLevels;
        }
        else
        {
            LUZASSERT(false);
        }

        return desc;
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC GetD3D12ShaderResourceViewDesc(const DirectX::TexMetadata& td)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC desc;
        ZeroMemory(&desc, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
        desc.Format = td.format;
        desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

        if (td.dimension == DirectX::TEX_DIMENSION_TEXTURE1D)
        {
            if (td.depth > 1)
            {
                desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
                desc.Texture1DArray.ArraySize = static_cast<UINT>(td.depth);
                desc.Texture1DArray.MipLevels = static_cast<UINT>(td.mipLevels);
            }
            else
            {
                desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
                desc.Texture1D.MipLevels = static_cast<UINT>(td.mipLevels);
            }
        }
        else if (td.dimension == DirectX::TEX_DIMENSION_TEXTURE2D)
        {
            if (td.IsCubemap())
            {
                if (td.depth > 6)
                {
                    LUZASSERT(td.depth % 6 == 0);
                    desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
                    desc.TextureCubeArray.NumCubes = static_cast<UINT>(td.depth) / 6;
                    desc.TextureCubeArray.MipLevels = static_cast<UINT>(td.mipLevels);
                }
                else
                {
                    desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
                    desc.TextureCube.MipLevels = static_cast<UINT>(td.mipLevels);
                }
            }
            else
            {
                if (td.depth > 1)
                {
                    desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
                    desc.Texture2DArray.ArraySize = static_cast<UINT>(td.depth);;
                    desc.Texture2DArray.MipLevels = static_cast<UINT>(td.mipLevels);
                }
                else
                {
                    desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                    desc.Texture2D.MipLevels = static_cast<UINT>(td.mipLevels);
                }
            }

        }
        else if (td.dimension == DirectX::TEX_DIMENSION_TEXTURE3D)
        {
            desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
            desc.Texture3D.MipLevels = static_cast<UINT>(td.mipLevels);
        }
        else
        {
            LUZASSERT(false);
        }

        return desc;
    }

    D3D12_RENDER_TARGET_VIEW_DESC GetD3D12RenderTargetViewDesc(const TextureDesc& td)
    {
        D3D12_RENDER_TARGET_VIEW_DESC desc;
        ZeroMemory(&desc, sizeof(D3D12_RENDER_TARGET_VIEW_DESC));
        desc.Format = GetDxgiFormat(td.Format);
        
        if (td.Dimension == GFX_RESOURCE_DIMENSION_TEXTURE1D)
        {
            if (td.Depth > 1)
            {
                desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
                desc.Texture1DArray.ArraySize = 1;
            }
            else
            {
                desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
            }
        }
        else if (td.Dimension == GFX_RESOURCE_DIMENSION_TEXTURE2D)
        {
            if (td.Depth > 1)
            {
                desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
                desc.Texture2DArray.ArraySize = 1;
            }
            else
            {
                desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
            }

        }
        else if (td.Dimension == GFX_RESOURCE_DIMENSION_TEXTURE3D)
        {
            desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
            desc.Texture3D.WSize = -1;
        }
        else
        {
            LUZASSERT(false);
        }

        return desc;
    }

    D3D12_RENDER_TARGET_VIEW_DESC GetD3D12RenderTargetViewDesc(const DirectX::TexMetadata& td)
    {
        D3D12_RENDER_TARGET_VIEW_DESC desc;
        ZeroMemory(&desc, sizeof(D3D12_RENDER_TARGET_VIEW_DESC));
        desc.Format = td.format;

        if (td.dimension == DirectX::TEX_DIMENSION_TEXTURE1D)
        {
            if (td.depth > 1)
            {
                desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
                desc.Texture1DArray.ArraySize = static_cast<UINT>(td.depth);
            }
            else
            {
                desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
            }
        }
        else if (td.dimension == DirectX::TEX_DIMENSION_TEXTURE2D)
        {
            if (td.depth > 1)
            {
                desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
                desc.Texture2DArray.ArraySize = static_cast<UINT>(td.depth);
            }
            else
            {
                desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
            }

        }
        else if (td.dimension == DirectX::TEX_DIMENSION_TEXTURE3D)
        {
            desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
            desc.Texture3D.WSize = -1;
        }
        else
        {
            LUZASSERT(false);
        }

        return desc;
    }

    D3D12_RESOURCE_STATES GetD3D12ResourceState(const ResourceStates rs)
    {
        D3D12_RESOURCE_STATES states = D3D12_RESOURCE_STATE_COMMON;

        if (rs & GFX_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER) states |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        if (rs & GFX_RESOURCE_STATE_INDEX_BUFFER) states |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
        if (rs & GFX_RESOURCE_STATE_RENDER_TARGET) states |= D3D12_RESOURCE_STATE_RENDER_TARGET;
        if (rs & GFX_RESOURCE_STATE_UNORDERED_ACCESS) states |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        if (rs & GFX_RESOURCE_STATE_DEPTH_WRITE) states |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
        if (rs & GFX_RESOURCE_STATE_DEPTH_READ) states |= D3D12_RESOURCE_STATE_DEPTH_READ;
        if (rs & GFX_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE) states |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        if (rs & GFX_RESOURCE_STATE_PIXEL_SHADER_RESOURCE) states |= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        if (rs & GFX_RESOURCE_STATE_STREAM_OUT) states |= D3D12_RESOURCE_STATE_STREAM_OUT;
        if (rs & GFX_RESOURCE_STATE_INDIRECT_ARGUMENT) states |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
        if (rs & GFX_RESOURCE_STATE_COPY_DEST) states |= D3D12_RESOURCE_STATE_COPY_DEST;
        if (rs & GFX_RESOURCE_STATE_COPY_SOURCE) states |= D3D12_RESOURCE_STATE_COPY_SOURCE;
        if (rs & GFX_RESOURCE_STATE_RESOLVE_DEST) states |= D3D12_RESOURCE_STATE_RESOLVE_DEST;
        if (rs & GFX_RESOURCE_STATE_RESOLVE_SOURCE) states |= D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
        if (rs & GFX_RESOURCE_STATE_GENERIC_READ) states |= D3D12_RESOURCE_STATE_GENERIC_READ;
        if (rs & GFX_RESOURCE_STATE_PRESENT) states |= D3D12_RESOURCE_STATE_PRESENT;
        if (rs & GFX_RESOURCE_STATE_PREDICATION) states |= D3D12_RESOURCE_STATE_PREDICATION;
        if (rs & GFX_RESOURCE_STATE_VIDEO_DECODE_READ) states |= D3D12_RESOURCE_STATE_VIDEO_DECODE_READ;
        if (rs & GFX_RESOURCE_STATE_VIDEO_DECODE_WRITE) states |= D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE;
        if (rs & GFX_RESOURCE_STATE_VIDEO_PROCESS_READ) states |= D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ;
        if (rs & GFX_RESOURCE_STATE_VIDEO_PROCESS_WRITE) states |= D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE;

        return states;
    }
}
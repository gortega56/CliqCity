#include "stdafx.h"
#include "PipelineStates.h"

namespace Graphics
{
    StencilState StencilState::KeepAlways =
    {
        GFX_STENCIL_OP_KEEP,
        GFX_STENCIL_OP_KEEP,
        GFX_STENCIL_OP_KEEP,
        COMPARISON_TYPE_ALWAYS
    };

    DepthStencilState DepthStencilState::DepthCompareLessWriteAll_StencilOff =
    {
        GFX_DEPTH_WRITE_MASK_ALL,
        COMPARISON_TYPE_LESS,
        true,
        0xff,
        0xff,
        StencilState::KeepAlways,
        StencilState::KeepAlways,
        false
    };

    RasterizerState RasterizerState::FillSolidCullCCW =
    {
        GFX_FILL_MODE_SOLID,
        GFX_CULL_MODE_BACK,
        false,
        0,
        0.0f,
        0.0f,
        true,
        false,
        false,
        0,
        GFX_CONSERVATIVE_RASTERIZATION_MODE_OFF
    };

    RasterizerState RasterizerState::FillSolidCullCW =
    {
        GFX_FILL_MODE_SOLID,
        GFX_CULL_MODE_BACK,
        true,
        0,
        0.0f,
        0.0f,
        true,
        false,
        false,
        0,
        GFX_CONSERVATIVE_RASTERIZATION_MODE_OFF
    };

    RenderTargetBlendState RenderTargetBlendState::Replace =
    {
        false,
        false,
        GFX_BLEND_ZERO,
        GFX_BLEND_ZERO,
        GFX_BLEND_OP_ADD,
        GFX_BLEND_ZERO,
        GFX_BLEND_ZERO,
        GFX_BLEND_OP_ADD,
        GFX_LOGIC_OP_NOOP,
        GFX_COLOR_WRITE_ENABLE_ALL
    };

    RenderTargetBlendState RenderTargetBlendState::Additive =
    {
        true,
        false,
        GFX_BLEND_ONE,
        GFX_BLEND_ONE,
        GFX_BLEND_OP_ADD,
        GFX_BLEND_ZERO,
        GFX_BLEND_ZERO,
        GFX_BLEND_OP_ADD,
        GFX_LOGIC_OP_NOOP,
        GFX_COLOR_WRITE_ENABLE_ALL
    };

    RenderTargetBlendState RenderTargetBlendState::Subtractive =
    {
        true,
        false,
        GFX_BLEND_ONE,
        GFX_BLEND_ONE,
        GFX_BLEND_OP_SUBTRACT,
        GFX_BLEND_ZERO,
        GFX_BLEND_ZERO,
        GFX_BLEND_OP_ADD,
        GFX_LOGIC_OP_NOOP,
        GFX_COLOR_WRITE_ENABLE_ALL
    };

    RenderTargetBlendState RenderTargetBlendState::Multiplicative =
    {
        true,
        false,
        GFX_BLEND_ZERO,
        GFX_BLEND_SRC_COLOR,
        GFX_BLEND_OP_ADD,
        GFX_BLEND_ZERO,
        GFX_BLEND_ZERO,
        GFX_BLEND_OP_ADD,
        GFX_LOGIC_OP_NOOP,
        GFX_COLOR_WRITE_ENABLE_ALL
    };

    RenderTargetBlendState RenderTargetBlendState::Alpha =
    {
        false,
        false,
        GFX_BLEND_SRC_ALPHA,
        GFX_BLEND_INV_SRC_ALPHA,
        GFX_BLEND_OP_ADD,
        GFX_BLEND_ZERO,
        GFX_BLEND_ZERO,
        GFX_BLEND_OP_ADD,
        GFX_LOGIC_OP_NOOP,
        GFX_COLOR_WRITE_ENABLE_ALL
    };

    InputLayoutDesc::InputLayoutDesc() 
        : m_byteOffset(0)
    {

    }

    InputLayoutDesc::~InputLayoutDesc()
    {

    }

    InputLayoutDesc& InputLayoutDesc::Append(const char* name, const u32 index, const Format format, const u32 slot, const u32 byteOffset, const Element::Type elementType /*= Element::INPUT_LAYOUT_ELEMENT_TYPE_PER_VERTEX*/, const u32 instanceStepRate /*= 0*/)
    {
        m_elements.push_back({ name, index, slot, byteOffset, instanceStepRate, format, elementType });

        return *this;
    }

    InputLayoutDesc& InputLayoutDesc::AppendFloat4(const char* name)
    {
        return Append(name, GetSemanticIndex(name), Format::GFX_FORMAT_R32G32B32A32_FLOAT, 0, GetByteOffset(16));
    }

    InputLayoutDesc& InputLayoutDesc::AppendFloat3(const char* name)
    {
        return Append(name, GetSemanticIndex(name), Format::GFX_FORMAT_R32G32B32_FLOAT, 0, GetByteOffset(12));
    }

    InputLayoutDesc& InputLayoutDesc::AppendFloat2(const char* name)
    {
        return Append(name, GetSemanticIndex(name), Format::GFX_FORMAT_R32G32_FLOAT, 0, GetByteOffset(8));
    }

    InputLayoutDesc& InputLayoutDesc::AppendFloat1(const char* name)
    {
        return Append(name, GetSemanticIndex(name), Format::GFX_FORMAT_R32_FLOAT, 0, GetByteOffset(4));
    }

    InputLayoutDesc& InputLayoutDesc::AppendPosition3F()
    {
        return AppendFloat3("POSITION");
    }

    InputLayoutDesc& InputLayoutDesc::AppendNormal3F()
    {
        return AppendFloat3("NORMAL");
    }

    InputLayoutDesc& InputLayoutDesc::AppendUV2()
    {
        return AppendFloat2("TEXCOORD");
    }

    InputLayoutDesc& InputLayoutDesc::AppendUV3()
    {
        return AppendFloat3("TEXCOORD");
    }

    InputLayoutDesc& InputLayoutDesc::AppendWorldMatrix()
    {
        return Append("WORLD", GetSemanticIndex("WORLD"), Format::GFX_FORMAT_R32G32B32_FLOAT, 1, GetByteOffset(16), Element::Type::INPUT_LAYOUT_ELEMENT_TYPE_PER_INSTANCE, 1)
            .Append("WORLD", GetSemanticIndex("WORLD"), Format::GFX_FORMAT_R32G32B32_FLOAT, 1, GetByteOffset(16), Element::Type::INPUT_LAYOUT_ELEMENT_TYPE_PER_INSTANCE, 1)
            .Append("WORLD", GetSemanticIndex("WORLD"), Format::GFX_FORMAT_R32G32B32_FLOAT, 1, GetByteOffset(16), Element::Type::INPUT_LAYOUT_ELEMENT_TYPE_PER_INSTANCE, 1)
            .Append("WORLD", GetSemanticIndex("WORLD"), Format::GFX_FORMAT_R32G32B32_FLOAT, 1, GetByteOffset(16), Element::Type::INPUT_LAYOUT_ELEMENT_TYPE_PER_INSTANCE, 1);
    }

    InputLayoutDesc& InputLayoutDesc::AppendBlendIndices1()
    {
        return Append("BLENDINDICES", GetSemanticIndex("BLENDINDICES"), Format::GFX_FORMAT_R32_UINT, 0, GetByteOffset(4));
    }

    InputLayoutDesc& InputLayoutDesc::AppendBlendIndices2()
    {
        return Append("BLENDINDICES", GetSemanticIndex("BLENDINDICES"), Format::GFX_FORMAT_R32G32_UINT, 0, GetByteOffset(8));
    }

    InputLayoutDesc& InputLayoutDesc::AppendBlendIndices3()
    {
        return Append("BLENDINDICES", GetSemanticIndex("BLENDINDICES"), Format::GFX_FORMAT_R32G32B32_UINT, 0, GetByteOffset(12));
    }

    InputLayoutDesc& InputLayoutDesc::AppendBlendIndices4()
    {
        return Append("BLENDINDICES", GetSemanticIndex("BLENDINDICES"), Format::GFX_FORMAT_R32G32B32A32_UINT, 0, GetByteOffset(16));
    }

    InputLayoutDesc& InputLayoutDesc::AppendBlendWeights1()
    {
        return AppendFloat1("BLENDWEIGHTS");
    }

    InputLayoutDesc& InputLayoutDesc::AppendBlendWeights2()
    {
        return AppendFloat2("BLENDWEIGHTS");
    }

    InputLayoutDesc& InputLayoutDesc::AppendBlendWeights3()
    {
        return AppendFloat3("BLENDWEIGHTS");
    }

    InputLayoutDesc& InputLayoutDesc::AppendBlendWeights4()
    {
        return AppendFloat4("BLENDWEIGHTS");
    }

    InputLayoutDesc& InputLayoutDesc::AppendColor4F()
    {
        return AppendFloat4("COLOR");
    }

    u32 InputLayoutDesc::GetByteOffset(size_t size)
    {
        u32 offset = m_byteOffset;
        m_byteOffset += (u32)size;
        return offset;
    }

    u32 InputLayoutDesc::GetSemanticIndex(std::string name)
    {
        u32 idx = 0;
        auto iter = m_semantics.find(name);
        if (iter == m_semantics.end())
        {
            m_semantics.insert({ name, 1 });
        }
        else
        {
            idx = iter->second;
            iter->second += 1;
        }

        return idx;
    }

    u32 InputLayoutDesc::NumElements() const
    {
        return static_cast<u32>(m_elements.size());
    }

    const InputLayoutDesc::Element* InputLayoutDesc::GetElements() const
    {
        return m_elements.data();
    }

    u32 Parameter::GetConstantRegister() const
    {
        return m_constant.Register;
    }

    u32 Parameter::GetConstantRegisterSpace() const
    {
        return m_constant.RegisterSpace;
    }
    u32 Parameter::GetConstantNum32BitValues() const
    {
        return m_constant.Num32BitValues;
    }

    u32 Parameter::GetDescriptorRegister() const
    {
        return m_descriptor.Register;
    }

    u32 Parameter::GetDescriptorRegisterSpace() const
    {
        return m_descriptor.RegisterSpace;
    }

    const DescriptorTable::Range& Parameter::GetDescriptorTableRange(u32 i) const
    {
        return m_descriptorTable.DescriptorRanges[i];
    }

    u32 Parameter::NumDescriptorTableRanges() const
    {
        return (u32)m_descriptorTable.NumRanges;
    }

    Parameter::Type Parameter::GetType() const
    {
        return m_type;
    }

    ShaderVisibility Parameter::GetVisibility() const
    {
        return m_visibility;
    }

    Parameter::DataFlags Parameter::GetDataFlags() const
    {
        return m_dataFlags;
    }

    void Parameter::InitializeConstant(const u32& shaderRegister, const u32& registerSpace, const u32& numValues, const ShaderVisibility& visibility)
    {
        m_constant.Num32BitValues = numValues;
        m_constant.Register = shaderRegister;
        m_constant.RegisterSpace = registerSpace;
        m_visibility = visibility;
        m_dataFlags = PARAMETER_DATA_FLAG_NONE;
    }

    void Parameter::InitializeConstantView(const u32& shaderRegister, const u32& registerSpace, const DataFlags& flags, const ShaderVisibility& visibility)
    {
        InitializeDescriptor(shaderRegister, registerSpace, PARAMETER_TYPE_CONSTANT_VIEW, flags, visibility);
    }

    void Parameter::InitializeShaderView(const u32& shaderRegister, const u32& registerSpace, const DataFlags& flags, const ShaderVisibility& visibility)
    {
        InitializeDescriptor(shaderRegister, registerSpace, PARAMETER_TYPE_SHADER_VIEW, flags, visibility);
    }

    void Parameter::InitializeComputeView(const u32& shaderRegister, const u32& registerSpace, const DataFlags& flags, const ShaderVisibility& visibility)
    {
        InitializeDescriptor(shaderRegister, registerSpace, PARAMETER_TYPE_COMPUTE_VIEW, flags, visibility);
    }

    void Parameter::InitializeDescriptor(const u32& shaderRegister, const u32& registerSpace, const Parameter::Type& type, const DataFlags& flags, const ShaderVisibility& visibility)
    {
        m_type = type;
        m_descriptor.Register = shaderRegister;
        m_descriptor.RegisterSpace = registerSpace;
        m_dataFlags = flags;
        m_visibility = visibility;
    }

    void Parameter::InitializeDescriptorTable(const ShaderVisibility& visibility)
    {
        m_type = PARAMETER_TYPE_DESCRIPTOR_TABLE;
        m_dataFlags = PARAMETER_DATA_FLAG_NONE;
        m_visibility = visibility;
        m_descriptorTable.NumRanges = 0;
    }

    void Parameter::AppendDescriptorTableRange(const u32& numDescriptors, const u32& baseRegister, const u32& registerSpace, const DescriptorTable::Range::Type& type)
    {
        LUZASSERT(m_descriptorTable.NumRanges < DescriptorTable::MaxRanges - 1);

        DescriptorTable::Range& range = m_descriptorTable.DescriptorRanges[m_descriptorTable.NumRanges++];
        range.NumDescriptors = numDescriptors;
        range.BaseRegister = baseRegister;
        range.RegisterSpace = registerSpace;
        range.Kind = type;
    }

    SignatureDesc::SignatureDesc() : m_name("unnamed")
    {

    }

    SignatureDesc::~SignatureDesc()
    {

    }

    const char* SignatureDesc::GetName() const
    {
        return m_name;
    }

    const Parameter& SignatureDesc::GetParam(u32 i) const
    {
        return m_params[i];
    }

    const Sampler& SignatureDesc::GetSampler(u32 i) const
    {
        return m_samplers[i];
    }

    u32 SignatureDesc::NumParams() const
    {
        return (u32)m_params.size();
    }

    u32 SignatureDesc::NumSamplers() const
    {
        return (u32)m_samplers.size();
    }

    SignatureDesc::Flags SignatureDesc::GetFlags() const
    {
        return m_flags;
    }

    SignatureDesc& SignatureDesc::SetName(const char* pName)
    {
        m_name = pName;
        return *this;
    }

    SignatureDesc& SignatureDesc::AppendConstant(const u32& num32BitVals, const u32& shaderRegister, const u32 registerSpace /*= 0U*/, const ShaderVisibility visibility /*= SHADER_VISIBILITY_ALL*/)
    {
        AppendParam()->InitializeConstant(shaderRegister, registerSpace, num32BitVals, visibility);
        return *this;
    }

    SignatureDesc& SignatureDesc::AppendConstantView(const u32& shaderRegister, const u32 registerSpace /*= 0U*/, const Parameter::DataFlags flags /*= PARAMETER_DATA_FLAG_NONE*/, const ShaderVisibility visibility /*= SHADER_VISIBILITY_ALL*/)
    {
        AppendParam()->InitializeConstantView(shaderRegister, registerSpace, flags, visibility);
        return *this;
    }

    SignatureDesc& SignatureDesc::AppendShaderView(const u32& shaderRegister, const u32 registerSpace /*= 0U*/, const Parameter::DataFlags flags /*= PARAMETER_DATA_FLAG_NONE*/, const ShaderVisibility visibility /*= SHADER_VISIBILITY_ALL*/)
    {
        AppendParam()->InitializeShaderView(shaderRegister, registerSpace, flags, visibility);
        return *this;
    }

    SignatureDesc& SignatureDesc::AppendComputeView(const u32& shaderRegister, const u32 registerSpace /*= 0U*/, const Parameter::DataFlags flags /*= PARAMETER_DATA_FLAG_NONE*/, const ShaderVisibility visibility /*= SHADER_VISIBILITY_ALL*/)
    {
        AppendParam()->InitializeComputeView(shaderRegister, registerSpace, flags, visibility);
        return *this;
    }

    SignatureDesc& SignatureDesc::AppendDescriptorTable(const ShaderVisibility visibility /*= SHADER_VISIBILITY_ALL*/)
    {
        AppendParam()->InitializeDescriptorTable(visibility);
        return *this;
    }

    SignatureDesc& SignatureDesc::AppendDescriptorTableRange(const u32& paramIndex, const u32& numDescriptors, const u32& baseRegister, const u32& registerSpace, const DescriptorTable::Range::Type& type)
    {
        LUZASSERT(paramIndex < (u32)m_params.size());
        LUZASSERT(m_params[paramIndex].GetType() == Parameter::Type::PARAMETER_TYPE_DESCRIPTOR_TABLE);

        m_params[paramIndex].AppendDescriptorTableRange(numDescriptors, baseRegister, registerSpace, type);
        return *this;
    }

    SignatureDesc& SignatureDesc::AppendAnisotropicWrapSampler(const u32& shaderRegister)
    {
        return AppendStaticSampler(shaderRegister, 0,
            FilterType::GFX_FILTER_ANISOTROPIC,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_WRAP,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_WRAP,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_WRAP);
    }

    SignatureDesc& SignatureDesc::AppendAnisotropicClampSampler(const u32& shaderRegister)
    {
        return AppendStaticSampler(shaderRegister, 0,
            FilterType::GFX_FILTER_ANISOTROPIC,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_CLAMP,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_CLAMP,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_CLAMP);
    }

    SignatureDesc& SignatureDesc::AppendPointWrapSampler(const u32& shaderRegister)
    {
        return AppendStaticSampler(shaderRegister, 0,
            FilterType::GFX_FILTER_MIN_MAG_MIP_POINT,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_WRAP,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_WRAP,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_WRAP);
    }

    SignatureDesc& SignatureDesc::AppendPointClampSampler(const u32& shaderRegister)
    {
        return AppendStaticSampler(shaderRegister, 0,
            FilterType::GFX_FILTER_MIN_MAG_MIP_POINT,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_CLAMP,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_CLAMP,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_CLAMP);
    }

    SignatureDesc& SignatureDesc::AppendLinearWrapSampler(const u32& shaderRegister)
    {
        return AppendStaticSampler(shaderRegister, 0,
            FilterType::GFX_FILTER_MIN_MAG_MIP_LINEAR,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_WRAP,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_WRAP,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_WRAP);
    }

    SignatureDesc& SignatureDesc::AppendLinearClampSampler(const u32& shaderRegister)
    {
        return AppendStaticSampler(shaderRegister, 0,
            FilterType::GFX_FILTER_MIN_MAG_MIP_LINEAR,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_CLAMP,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_CLAMP,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_CLAMP);
    }

    LUZ_API SignatureDesc& SignatureDesc::AppendComparisonPointBorderSampler(const u32& shaderRegister)
    {
        return AppendStaticSampler(shaderRegister, 0,
            FilterType::GFX_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_BORDER,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_BORDER,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_BORDER,
            0.0f, 
            16U,
            COMPARISON_TYPE_LESS_EQUAL,
            Graphics::BORDER_COLOR_TYPE_OPAQUE_BLACK);
    }

    LUZ_API SignatureDesc& SignatureDesc::AppendWrapSampler(const u32& shaderRegister, const FilterType eFilter)
    {
        return AppendStaticSampler(shaderRegister, 0,
            eFilter,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_WRAP,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_WRAP,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_WRAP);
    }

    LUZ_API SignatureDesc& SignatureDesc::AppendClampSampler(const u32& shaderRegister, const FilterType eFilter)
    {
        return AppendStaticSampler(shaderRegister, 0,
            eFilter,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_CLAMP,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_CLAMP,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_CLAMP);
    }

    LUZ_API SignatureDesc& SignatureDesc::AppendMirrorSampler(const u32& shaderRegister, const FilterType eFilter)
    {
        return AppendStaticSampler(shaderRegister, 0,
            eFilter,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_MIRROR,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_MIRROR,
            AddressType::GFX_TEXTURE_ADDRESS_MODE_MIRROR);
    }

    SignatureDesc& SignatureDesc::AppendStaticSampler(const u32& shaderRegister,
        const u32 registerSpace /*= 0U*/,
        const FilterType filter /*= GFX_FILTER_TYPE_ANISOTROPIC*/,
        const AddressType addressU /*= GFX_TEXTURE_ADDRESS_MODE_WRAP*/,
        const AddressType addressV /*= GFX_TEXTURE_ADDRESS_MODE_WRAP*/,
        const AddressType addressW /*= GFX_TEXTURE_ADDRESS_MODE_WRAP*/,
        const float mipLodBias /*= 0*/,
        const u32 maxAnisotropy /*= 16*/,
        const ComparisonType comparisonFunc /*= COMPARISON_TYPE_LESS_EQUAL*/,
        const BorderColorType borderColor /*= BORDER_COLOR_TYPE_OPAQUE_WHITE*/,
        const float minLod /*= 0.f*/,
        const float maxLod /*= (std::numeric_limits<float>::max)()*/,
        const ShaderVisibility shaderVisibility /*= SHADER_VISIBILITY_ALL*/)
    {
        m_samplers.emplace_back();

        Sampler& sampler = m_samplers.back();
        sampler.Register = shaderRegister;
        sampler.RegisterSpace = registerSpace;
        sampler.Filter = filter;
        sampler.AddressU = addressU;
        sampler.AddressV = addressV;
        sampler.AddressW = addressW;
        sampler.MipLodBias = mipLodBias;
        sampler.MaxAnisotropy = maxAnisotropy;
        sampler.Comparison = comparisonFunc;
        sampler.BorderColor = borderColor;
        sampler.MinLod = minLod;
        sampler.MaxLod = maxLod;
        sampler.Visibility = shaderVisibility;

        return *this;
    }

    SignatureDesc& SignatureDesc::AllowInputLayout() { return SetFlags(SIGNATURE_FLAG_ALLOW_INPUT_LAYOUT); }
    SignatureDesc& SignatureDesc::AllowStreamOutput() { return SetFlags(SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT); }
    SignatureDesc& SignatureDesc::DenyVS() { return SetFlags(SIGNATURE_FLAG_DENY_VS); }
    SignatureDesc& SignatureDesc::DenyHS() { return SetFlags(SIGNATURE_FLAG_DENY_HS); }
    SignatureDesc& SignatureDesc::DenyDS() { return SetFlags(SIGNATURE_FLAG_DENY_DS); }
    SignatureDesc& SignatureDesc::DenyGS() { return SetFlags(SIGNATURE_FLAG_DENY_GS); }
    SignatureDesc& SignatureDesc::DenyPS() { return SetFlags(SIGNATURE_FLAG_DENY_PS); }

    SignatureDesc& SignatureDesc::SetFlags(const SignatureDesc::Flags& flags)
    {
        m_flags = Flags(m_flags | flags);
        return *this;
    }

    Parameter* SignatureDesc::AppendParam()
    {
        return &m_params.emplace_back();
    }
}
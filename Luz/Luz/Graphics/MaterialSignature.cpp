#include "stdafx.h"
#include "MaterialSignature.h"
#include "ISignature.h"

namespace Graphics
{
    namespace Material
    {
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

        Signature::Signature()
        {

        }

        Signature::~Signature()
        {

        }

        const Parameter& Signature::GetParam(u32 i) const
        {
            return m_params[i];
        }

        const Sampler& Signature::GetSampler(u32 i) const
        {
            return m_samplers[i];
        }

        u32 Signature::NumParams() const
        {
            return (u32)m_params.size();
        }

        u32 Signature::NumSamplers() const
        {
            return (u32)m_samplers.size();
        }

        Signature::Flags Signature::GetFlags() const
        {
            return m_flags;
        }

        ISignature* Signature::GetSignatureInterface() const
        {
            return m_signature.get();
        }

        Signature& Signature::AppendConstant(const u32& num32BitVals, const u32& shaderRegister, const u32 registerSpace /*= 0U*/, const ShaderVisibility visibility /*= SHADER_VISIBILITY_ALL*/)
        {
            AppendParam()->InitializeConstant(shaderRegister, registerSpace, num32BitVals, visibility);
            return *this;
        }

        Signature& Signature::AppendConstantView(const u32& shaderRegister, const u32 registerSpace /*= 0U*/, const Parameter::DataFlags flags /*= PARAMETER_DATA_FLAG_NONE*/, const ShaderVisibility visibility /*= SHADER_VISIBILITY_ALL*/)
        {
            AppendParam()->InitializeConstantView(shaderRegister, registerSpace, flags, visibility);
            return *this;
        }

        Signature& Signature::AppendShaderView(const u32& shaderRegister, const u32 registerSpace /*= 0U*/, const Parameter::DataFlags flags /*= PARAMETER_DATA_FLAG_NONE*/, const ShaderVisibility visibility /*= SHADER_VISIBILITY_ALL*/)
        {
            AppendParam()->InitializeShaderView(shaderRegister, registerSpace, flags, visibility);
            return *this;
        }

        Signature& Signature::AppendComputeView(const u32& shaderRegister, const u32 registerSpace /*= 0U*/, const Parameter::DataFlags flags /*= PARAMETER_DATA_FLAG_NONE*/, const ShaderVisibility visibility /*= SHADER_VISIBILITY_ALL*/)
        {
            AppendParam()->InitializeComputeView(shaderRegister, registerSpace, flags, visibility);
            return *this;
        }

        Signature& Signature::AppendDescriptorTableRange(const u32& tableIndex, const u32& numDescriptors, const u32& baseRegister, const u32& registerSpace, const DescriptorTable::Range::Type& type)
        {
            LUZASSERT(tableIndex < (u32)m_params.size());
            LUZASSERT(m_params[tableIndex].GetType() == Parameter::Type::PARAMETER_TYPE_DESCRIPTOR_TABLE);

            m_params[tableIndex].AppendDescriptorTableRange(numDescriptors, baseRegister, registerSpace, type);
            return *this;
        }

        Signature& Signature::AppendAnisotropicWrapSampler(const u32& shaderRegister)
        {
            return AppendStaticSampler(shaderRegister, 0,
                FilterType::FILTER_TYPE_ANISOTROPIC,
                AddressType::ADDRESS_TYPE_WRAP,
                AddressType::ADDRESS_TYPE_WRAP,
                AddressType::ADDRESS_TYPE_WRAP);
        }

        Signature& Signature::AppendAnisotropicClampSampler(const u32& shaderRegister)
        {
            return AppendStaticSampler(shaderRegister, 0,
                FilterType::FILTER_TYPE_ANISOTROPIC,
                AddressType::ADDRESS_TYPE_CLAMP,
                AddressType::ADDRESS_TYPE_CLAMP,
                AddressType::ADDRESS_TYPE_CLAMP);
        }

        Signature& Signature::AppendPointWrapSampler(const u32& shaderRegister)
        {
            return AppendStaticSampler(shaderRegister, 0,
                FilterType::FILTER_TYPE_MIN_MAG_MIP_POINT,
                AddressType::ADDRESS_TYPE_WRAP,
                AddressType::ADDRESS_TYPE_WRAP,
                AddressType::ADDRESS_TYPE_WRAP);
        }

        Signature& Signature::AppendPointClampSampler(const u32& shaderRegister)
        {
            return AppendStaticSampler(shaderRegister, 0,
                FilterType::FILTER_TYPE_MIN_MAG_MIP_POINT,
                AddressType::ADDRESS_TYPE_CLAMP,
                AddressType::ADDRESS_TYPE_CLAMP,
                AddressType::ADDRESS_TYPE_CLAMP);
        }

        Signature& Signature::AppendLinearWrapSampler(const u32& shaderRegister)
        {
            return AppendStaticSampler(shaderRegister, 0,
                FilterType::FILTER_TYPE_MIN_MAG_MIP_LINEAR,
                AddressType::ADDRESS_TYPE_WRAP,
                AddressType::ADDRESS_TYPE_WRAP,
                AddressType::ADDRESS_TYPE_WRAP);
        }

        Signature& Signature::AppendLinearClampSampler(const u32& shaderRegister)
        {
            return AppendStaticSampler(shaderRegister, 0,
                FilterType::FILTER_TYPE_MIN_MAG_MIP_LINEAR,
                AddressType::ADDRESS_TYPE_CLAMP,
                AddressType::ADDRESS_TYPE_CLAMP,
                AddressType::ADDRESS_TYPE_CLAMP);
        }

        Signature& Signature::AppendStaticSampler(const u32& shaderRegister,
            const u32 registerSpace /*= 0U*/,
            const FilterType filter /*= FILTER_TYPE_ANISOTROPIC*/,
            const AddressType addressU /*= ADDRESS_TYPE_WRAP*/,
            const AddressType addressV /*= ADDRESS_TYPE_WRAP*/,
            const AddressType addressW /*= ADDRESS_TYPE_WRAP*/,
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

        Signature& Signature::AllowInputLayout() { return SetFlags(SIGNATURE_FLAG_ALLOW_INPUT_LAYOUT); }
        Signature& Signature::AllowStreamOutput() { return SetFlags(SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT); }
        Signature& Signature::DenyVS() { return SetFlags(SIGNATURE_FLAG_DENY_VS); }
        Signature& Signature::DenyHS() { return SetFlags(SIGNATURE_FLAG_DENY_HS); }
        Signature& Signature::DenyDS() { return SetFlags(SIGNATURE_FLAG_DENY_DS); }
        Signature& Signature::DenyGS() { return SetFlags(SIGNATURE_FLAG_DENY_GS); }
        Signature& Signature::DenyPS() { return SetFlags(SIGNATURE_FLAG_DENY_PS); }

        Signature& Signature::SetFlags(const Signature::Flags& flags)
        {
            m_flags = Flags(m_flags | flags);
            return *this;
        }

        bool Signature::Finalize()
        {
#if _WIN64 || _WIN32
            auto signature = std::make_unique<Dx12::Signature>();
            if (signature->Initialize(*this))
            {
                m_signature = std::move(signature);
            }
#elif __APPLE__

#endif
            return (m_signature != nullptr);
        }

        Parameter* Signature::AppendParam()
        {
            m_params.emplace_back();
            return &m_params.back();
        }
    }
}
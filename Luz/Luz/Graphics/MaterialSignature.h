#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

enum class ParamID : int32_t
{
    INVALID = -1
};


namespace Graphics
{
    namespace Material
    {
        enum FilterType : u8
        {
            FILTER_TYPE_MIN_MAG_MIP_POINT = 0,
            FILTER_TYPE_MIN_MAG_MIP_LINEAR,
            FILTER_TYPE_ANISOTROPIC
        };

        enum AddressType : u8
        {
            ADDRESS_TYPE_WRAP = 0,
            ADDRESS_TYPE_MIRROR,
            ADDRESS_TYPE_CLAMP
        };

        enum ComparisonType : u8
        {
            COMPARISON_TYPE_NEVER = 0,
            COMPARISON_TYPE_LESS,
            COMPARISON_TYPE_EQUAL,
            COMPARISON_TYPE_LESS_EQUAL,
            COMPARISON_TYPE_GREATER,
            COMPARISON_TYPE_NOT_EQUAL,
            COMPARISON_TYPE_GREATER_EQUAL,
            COMPARISON_TYPE_ALWAYS

        };

        enum BorderColorType : u8
        {
            BORDER_COLOR_TYPE_TRANSPARENT_BLACK = 0,
            BORDER_COLOR_TYPE_OPAQUE_WHITE,
            BORDER_COLOR_TYPE_OPAQUE_BLACK
        };

        enum ShaderVisibility : u8
        {
            SHADER_VISIBILITY_ALL = 0,
            SHADER_VISIBILITY_VS,
            SHADER_VISIBILITY_HS,
            SHADER_VISIBILITY_DS,
            SHADER_VISIBILITY_GS,
            SHADER_VISIBILITY_PS
        };

        struct Constant
        {
            u32 Register;
            u32 RegisterSpace;
            u32 Num32BitValues;
        };

        struct Descriptor
        {
            u32 Register;
            u32 RegisterSpace;
        };

        struct DescriptorTable
        {
            struct Range
            {
                enum Type : u8
                {
                    DESCRIPTOR_TABLE_RANGE_TYPE_UNDEFINED = 0,
                    DESCRIPTOR_TABLE_RANGE_TYPE_CONSTANT_VIEW,
                    DESCRIPTOR_TABLE_RANGE_TYPE_SHADER_VIEW,
                    DESCRIPTOR_TABLE_RANGE_TYPE_COMPUTE_VIEW,
                    DESCRIPTOR_TABLE_RANGE_TYPE_SAMPLER,
                    DESCRIPTOR_TABLE_RANGE_TYPE_COUNT
                };

                u32 NumDescriptors;
                u32 BaseRegister;
                u32 RegisterSpace;
                Type Kind;
            };

            const static u8 MaxRanges = 4;
            u8 NumRanges;
            Range DescriptorRanges[MaxRanges];
        };

        class Parameter
        {
        public:
            enum Type : u8
            {
                PARAMETER_TYPE_UNDEFINED = 0,
                PARAMETER_TYPE_CONSTANTS,
                PARAMETER_TYPE_CONSTANT_VIEW,
                PARAMETER_TYPE_SHADER_VIEW,
                PARAMETER_TYPE_COMPUTE_VIEW,
                PARAMETER_TYPE_DESCRIPTOR_TABLE,
                PARAMETER_TYPE_COUNT
            };

            enum DataFlags : u8
            {
                PARAMETER_DATA_FLAG_NONE = 0,
                PARAMETER_DATA_FLAG_VOLATILE = 0x2,
                PARAMETER_DATA_FLAG_STATIC_EXECUTE = 0x4,
                PARAMETER_DATA_FLAG_STATIC = 0x8
            };

            Parameter() {};
            ~Parameter() {};


            u32 GetConstantRegister() const;
            u32 GetConstantRegisterSpace() const;
            u32 GetConstantNum32BitValues() const;

            u32 GetDescriptorRegister() const;
            u32 GetDescriptorRegisterSpace() const;

            const DescriptorTable::Range& GetDescriptorTableRange(u32 i) const;
            u32 NumDescriptorTableRanges() const;

            Type GetType() const;
            ShaderVisibility GetVisibility() const;
            DataFlags GetDataFlags() const;

            void InitializeConstant(const u32& shaderRegister, const u32& registerSpace, const u32& numValues, const ShaderVisibility& visibility);
            void InitializeConstantView(const u32& shaderRegister, const u32& registerSpace, const DataFlags& flags, const ShaderVisibility& visibility);
            void InitializeShaderView(const u32& shaderRegister, const u32& registerSpace, const DataFlags& flags, const ShaderVisibility& visibility);
            void InitializeComputeView(const u32& shaderRegister, const u32& registerSpace, const DataFlags& flags, const ShaderVisibility& visibility);
            void InitializeDescriptorTable(const ShaderVisibility& visibility);
            void AppendDescriptorTableRange(const u32& numDescriptors, const u32& baseRegister, const u32& registerSpace, const DescriptorTable::Range::Type& type);

        private:
            union
            {
                Constant m_constant;
                Descriptor m_descriptor;
                DescriptorTable m_descriptorTable;
            };

            Type m_type;
            ShaderVisibility m_visibility;
            DataFlags m_dataFlags;

            void InitializeDescriptor(const u32& shaderRegister, const u32& registerSpace, const Type& type, const DataFlags& flags, const ShaderVisibility& visibility);
        };

        struct Sampler
        {
            u32 Register;
            u32 RegisterSpace;
            FilterType Filter;
            AddressType AddressU;
            AddressType AddressV;
            AddressType AddressW;
            ComparisonType Comparison;
            BorderColorType BorderColor;
            float MipLodBias;
            float MinLod;
            float MaxLod;
            u32 MaxAnisotropy;
            ShaderVisibility Visibility;
        };

        class ISignature;

        class Signature
        {
        public:            
            enum Flags : u16
            {
                SIGNATURE_FLAG_NONE = 0,
                SIGNATURE_FLAG_ALLOW_INPUT_LAYOUT = 0x1,
                SIGNATURE_FLAG_DENY_VS = 0x2,
                SIGNATURE_FLAG_DENY_HS = 0x4,
                SIGNATURE_FLAG_DENY_DS = 0x8,
                SIGNATURE_FLAG_DENY_GS = 0x10,
                SIGNATURE_FLAG_DENY_PS = 0x20,
                SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT = 0x40
            };

            Signature();
            ~Signature();

            const Parameter& GetParam(u32 i) const;
            const Sampler& GetSampler(u32 i) const;
            Flags GetFlags() const;
            ISignature* GetSignatureInterface() const;

            u32 NumParams() const;
            u32 NumSamplers() const;

            Signature& AppendConstant(const u32& num32BitVals, const u32& shaderRegister, const u32 registerSpace = 0U, const ShaderVisibility visibility = SHADER_VISIBILITY_ALL);
            Signature& AppendConstantView(const u32& shaderRegister, const u32 registerSpace = 0U, const Parameter::DataFlags flags = Parameter::DataFlags::PARAMETER_DATA_FLAG_NONE, const ShaderVisibility visibility = SHADER_VISIBILITY_ALL);
            Signature& AppendShaderView(const u32& shaderRegister, const u32 registerSpace = 0U, const Parameter::DataFlags flags = Parameter::DataFlags::PARAMETER_DATA_FLAG_NONE, const ShaderVisibility visibility = SHADER_VISIBILITY_ALL);
            Signature& AppendComputeView(const u32& shaderRegister, const u32 registerSpace = 0U, const Parameter::DataFlags flags = Parameter::DataFlags::PARAMETER_DATA_FLAG_NONE, const ShaderVisibility visibility = SHADER_VISIBILITY_ALL);
            Signature& AppendDescriptorTableRange(const u32& tableIndex, const u32& numDescriptors, const u32& baseRegister, const u32& registerSpace, const DescriptorTable::Range::Type& type);

            Signature& AppendAnisotropicWrapSampler(const u32& shaderRegister);
            Signature& AppendAnisotropicClampSampler(const u32& shaderRegister);
            Signature& AppendPointWrapSampler(const u32& shaderRegister);
            Signature& AppendPointClampSampler(const u32& shaderRegister);
            Signature& AppendLinearWrapSampler(const u32& shaderRegister);
            Signature& AppendLinearClampSampler(const u32& shaderRegister);
            Signature& AppendStaticSampler(const u32& shaderRegister,
                const u32 registerSpace = 0U,
                const FilterType filter = FILTER_TYPE_ANISOTROPIC,
                const AddressType addressU = ADDRESS_TYPE_WRAP,
                const AddressType addressV = ADDRESS_TYPE_WRAP,
                const AddressType addressW = ADDRESS_TYPE_WRAP,
                const float mipLODBias = 0,
                const u32 maxAnisotropy = 16,
                const ComparisonType comparisonFunc = COMPARISON_TYPE_LESS_EQUAL,
                const BorderColorType borderColor = BORDER_COLOR_TYPE_OPAQUE_WHITE,
                const float minLOD = 0.f,
                const float maxLOD = (std::numeric_limits<float>::max)(),
                const ShaderVisibility shaderVisibility = SHADER_VISIBILITY_ALL);

            Signature& AllowInputLayout();
            Signature& AllowStreamOutput();
            Signature& DenyVS();
            Signature& DenyHS();
            Signature& DenyDS();
            Signature& DenyGS();
            Signature& DenyPS();
                               
            bool Finalize();

        private:
            std::unique_ptr<ISignature> m_signature;
            std::vector<Parameter> m_params;
            std::vector<Sampler> m_samplers;
            Flags m_flags;

            Parameter* AppendParam();
            Signature& SetFlags(const Flags& flags);
        };
    }
}

#endif
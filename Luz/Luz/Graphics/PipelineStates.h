#pragma once
#ifndef PIPELINESTATES_H
#define PIPELINESTATES_H

#ifndef GRAPHICSTYPES_H
#include "GraphicsTypes.h"
#endif

namespace Graphics
{
    enum LUZ_API PrimitiveTopologyType : u8
    {
        GFX_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED = 0,
        GFX_PRIMITIVE_TOPOLOGY_TYPE_POINT = 1,
        GFX_PRIMITIVE_TOPOLOGY_TYPE_LINE = 2,
        GFX_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE = 3,
        GFX_PRIMITIVE_TOPOLOGY_TYPE_PATCH = 4
    };

    enum LUZ_API FilterType : u8
    {
        FILTER_TYPE_MIN_MAG_MIP_POINT = 0,
        FILTER_TYPE_MIN_MAG_MIP_LINEAR,
        FILTER_TYPE_ANISOTROPIC
    };

    enum LUZ_API AddressType : u8
    {
        ADDRESS_TYPE_WRAP = 0,
        ADDRESS_TYPE_MIRROR,
        ADDRESS_TYPE_CLAMP
    };

    enum LUZ_API ComparisonType : u8
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

    enum LUZ_API BorderColorType : u8
    {
        BORDER_COLOR_TYPE_TRANSPARENT_BLACK = 0,
        BORDER_COLOR_TYPE_OPAQUE_WHITE,
        BORDER_COLOR_TYPE_OPAQUE_BLACK
    };

    enum LUZ_API ShaderVisibility : u8
    {
        SHADER_VISIBILITY_ALL = 0,
        SHADER_VISIBILITY_VS,
        SHADER_VISIBILITY_HS,
        SHADER_VISIBILITY_DS,
        SHADER_VISIBILITY_GS,
        SHADER_VISIBILITY_PS
    };

    class InputLayoutDesc
    {
    public:
        struct Element
        {
            enum Type : u8
            {
                INPUT_LAYOUT_ELEMENT_TYPE_PER_VERTEX = 0,
                INPUT_LAYOUT_ELEMENT_TYPE_PER_INSTANCE
            };

            const char* SemanticName;
            u32 SemanticIndex;
            u32 InputSlot;
            u32 ByteOffset;
            u32 InstanceRate;
            Format Format;
            Type ElementType;
        };

        LUZ_API InputLayoutDesc();
        LUZ_API ~InputLayoutDesc();

        LUZ_API InputLayoutDesc& Append(const char* name, const u32 index, const Format format, const u32 slot, const u32 byteOffset, const Element::Type elementType = Element::INPUT_LAYOUT_ELEMENT_TYPE_PER_VERTEX, const u32 instanceStepRate = 0);

        LUZ_API InputLayoutDesc& AppendPosition3F();
        LUZ_API InputLayoutDesc& AppendNormal3F();
        LUZ_API InputLayoutDesc& AppendUV2();
        LUZ_API InputLayoutDesc& AppendUV3();

        LUZ_API InputLayoutDesc& AppendWorldMatrix();
        LUZ_API InputLayoutDesc& AppendBlendIndices1();
        LUZ_API InputLayoutDesc& AppendBlendIndices2();
        LUZ_API InputLayoutDesc& AppendBlendIndices3();
        LUZ_API InputLayoutDesc& AppendBlendIndices4();

        LUZ_API InputLayoutDesc& AppendBlendWeights1();
        LUZ_API InputLayoutDesc& AppendBlendWeights2();
        LUZ_API InputLayoutDesc& AppendBlendWeights3();
        LUZ_API InputLayoutDesc& AppendBlendWeights4();

        LUZ_API InputLayoutDesc& AppendColor4F();

        LUZ_API InputLayoutDesc& AppendFloat4(const char* name);
        LUZ_API InputLayoutDesc& AppendFloat3(const char* name);
        LUZ_API InputLayoutDesc& AppendFloat2(const char* name);
        LUZ_API InputLayoutDesc& AppendFloat1(const char* name);

        u32 NumElements() const;
        const Element* GetElements() const;

    private:
        std::unordered_map<std::string, u32> m_semantics;
        std::vector<Element> m_elements;
        u32 m_byteOffset;

        u32 GetByteOffset(size_t size);
        u32 GetSemanticIndex(std::string name);
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

    class SignatureDesc
    {
    public:
        enum LUZ_API Flags : u16
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

        LUZ_API SignatureDesc();
        LUZ_API ~SignatureDesc();

        const Parameter& GetParam(u32 i) const;
        const Sampler& GetSampler(u32 i) const;
        Flags GetFlags() const;

        u32 NumParams() const;
        u32 NumSamplers() const;

        LUZ_API SignatureDesc& AppendConstant(const u32& num32BitVals, const u32& shaderRegister, const u32 registerSpace = 0U, const ShaderVisibility visibility = SHADER_VISIBILITY_ALL);
        LUZ_API SignatureDesc& AppendConstantView(const u32& shaderRegister, const u32 registerSpace = 0U, const Parameter::DataFlags flags = Parameter::DataFlags::PARAMETER_DATA_FLAG_NONE, const ShaderVisibility visibility = SHADER_VISIBILITY_ALL);
        LUZ_API SignatureDesc& AppendShaderView(const u32& shaderRegister, const u32 registerSpace = 0U, const Parameter::DataFlags flags = Parameter::DataFlags::PARAMETER_DATA_FLAG_NONE, const ShaderVisibility visibility = SHADER_VISIBILITY_ALL);
        LUZ_API SignatureDesc& AppendComputeView(const u32& shaderRegister, const u32 registerSpace = 0U, const Parameter::DataFlags flags = Parameter::DataFlags::PARAMETER_DATA_FLAG_NONE, const ShaderVisibility visibility = SHADER_VISIBILITY_ALL);
        LUZ_API SignatureDesc& AppendDescriptorTableRange(const u32& tableIndex, const u32& numDescriptors, const u32& baseRegister, const u32& registerSpace, const DescriptorTable::Range::Type& type);

        LUZ_API SignatureDesc& AppendAnisotropicWrapSampler(const u32& shaderRegister);
        LUZ_API SignatureDesc& AppendAnisotropicClampSampler(const u32& shaderRegister);
        LUZ_API SignatureDesc& AppendPointWrapSampler(const u32& shaderRegister);
        LUZ_API SignatureDesc& AppendPointClampSampler(const u32& shaderRegister);
        LUZ_API SignatureDesc& AppendLinearWrapSampler(const u32& shaderRegister);
        LUZ_API SignatureDesc& AppendLinearClampSampler(const u32& shaderRegister);
        LUZ_API SignatureDesc& AppendStaticSampler(const u32& shaderRegister,
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

        LUZ_API SignatureDesc& AllowInputLayout();
        LUZ_API SignatureDesc& AllowStreamOutput();
        LUZ_API SignatureDesc& DenyVS();
        LUZ_API SignatureDesc& DenyHS();
        LUZ_API SignatureDesc& DenyDS();
        LUZ_API SignatureDesc& DenyGS();
        LUZ_API SignatureDesc& DenyPS();

    private:
        std::vector<Parameter> m_params;
        std::vector<Sampler> m_samplers;
        Flags m_flags;

        Parameter* AppendParam();
        SignatureDesc& SetFlags(const Flags& flags);
    };

    enum DepthWriteMask : u8
    {
        GFX_DEPTH_WRITE_MASK_ZERO = 0,
        GFX_DEPTH_WRITE_MASK_ALL = 1
    };

    enum StencilOp : u8
    {
        GFX_STENCIL_OP_KEEP = 1,
        GFX_STENCIL_OP_ZERO = 2,
        GFX_STENCIL_OP_REPLACE = 3,
        GFX_STENCIL_OP_INCR_SAT = 4,
        GFX_STENCIL_OP_DECR_SAT = 5,
        GFX_STENCIL_OP_INVERT = 6,
        GFX_STENCIL_OP_INCR = 7,
        GFX_STENCIL_OP_DECR = 8
    };

    struct StencilState
    {
        StencilOp StencilFailOp = GFX_STENCIL_OP_KEEP;
        StencilOp StencilDepthFailOp = GFX_STENCIL_OP_KEEP;
        StencilOp StencilPassOp = GFX_STENCIL_OP_KEEP;
        ComparisonType Comparison = COMPARISON_TYPE_ALWAYS;
    };

    struct DepthStencilState
    {
        DepthWriteMask WriteMask = GFX_DEPTH_WRITE_MASK_ALL;
        ComparisonType Comparison = COMPARISON_TYPE_LESS;
        bool DepthEnable = true;
        u8 StencilReadMask = 0xff;
        u8 StencilWriteMask = 0xff;
        StencilState FrontFace;
        StencilState BackFace;
        bool StencilEnable = true;
    };

    enum FillMode : u8
    {
        GFX_FILL_MODE_WIREFRAME = 2,
        GFX_FILL_MODE_SOLID = 3
    };

    enum CullMode : u8
    {
        GFX_CULL_MODE_NONE = 1,
        GFX_CULL_MODE_FRONT = 2,
        GFX_CULL_MODE_BACK = 3
    };

    enum ConservativeRasterizationMode : u8
    {
        GFX_CONSERVATIVE_RASTERIZATION_MODE_UNAVAILABLE,
        GFX_CONSERVATIVE_RASTERIZATION_MODE_OFF,
        GFX_CONSERVATIVE_RASTERIZATION_MODE_ON,
    };

    struct RasterizerState
    {
        FillMode Fill = GFX_FILL_MODE_SOLID;
        CullMode Cull = GFX_CULL_MODE_BACK;
        bool FrontCounterClockwise = false;
        u32 DepthBias = 0;
        float DepthBiasClamp = 0.0f;
        float SlopeScaledDepthBias = 0.0f;
        bool DepthClipEnable = true;
        bool MsaaEnable = false;
        bool AntialiasedLineEnable = false;
        u32 ForcedSampleCount = 0;
        ConservativeRasterizationMode RasterizationMode = GFX_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    };

    enum BlendType
    {
        GFX_BLEND_ZERO = 1,
        GFX_BLEND_ONE = 2,
        GFX_BLEND_SRC_COLOR = 3,
        GFX_BLEND_INV_SRC_COLOR = 4,
        GFX_BLEND_SRC_ALPHA = 5,
        GFX_BLEND_INV_SRC_ALPHA = 6,
        GFX_BLEND_DEST_ALPHA = 7,
        GFX_BLEND_INV_DEST_ALPHA = 8,
        GFX_BLEND_DEST_COLOR = 9,
        GFX_BLEND_INV_DEST_COLOR = 10,
        GFX_BLEND_SRC_ALPHA_SAT = 11,
        GFX_BLEND_BLEND_FACTOR = 14,
        GFX_BLEND_INV_BLEND_FACTOR = 15,
        GFX_BLEND_SRC1_COLOR = 16,
        GFX_BLEND_INV_SRC1_COLOR = 17,
        GFX_BLEND_SRC1_ALPHA = 18,
        GFX_BLEND_INV_SRC1_ALPHA = 19
    };

    enum BlendOpType
    {
        GFX_BLEND_OP_ADD = 1,
        GFX_BLEND_OP_SUBTRACT = 2,
        GFX_BLEND_OP_REV_SUBTRACT = 3,
        GFX_BLEND_OP_MIN = 4,
        GFX_BLEND_OP_MAX = 5
    };

    enum LogicOpType : u8
    {
        GFX_LOGIC_OP_CLEAR = 0,
        GFX_LOGIC_OP_SET = (GFX_LOGIC_OP_CLEAR + 1),
        GFX_LOGIC_OP_COPY = (GFX_LOGIC_OP_SET + 1),
        GFX_LOGIC_OP_COPY_INVERTED = (GFX_LOGIC_OP_COPY + 1),
        GFX_LOGIC_OP_NOOP = (GFX_LOGIC_OP_COPY_INVERTED + 1),
        GFX_LOGIC_OP_INVERT = (GFX_LOGIC_OP_NOOP + 1),
        GFX_LOGIC_OP_AND = (GFX_LOGIC_OP_INVERT + 1),
        GFX_LOGIC_OP_NAND = (GFX_LOGIC_OP_AND + 1),
        GFX_LOGIC_OP_OR = (GFX_LOGIC_OP_NAND + 1),
        GFX_LOGIC_OP_NOR = (GFX_LOGIC_OP_OR + 1),
        GFX_LOGIC_OP_XOR = (GFX_LOGIC_OP_NOR + 1),
        GFX_LOGIC_OP_EQUIV = (GFX_LOGIC_OP_XOR + 1),
        GFX_LOGIC_OP_AND_REVERSE = (GFX_LOGIC_OP_EQUIV + 1),
        GFX_LOGIC_OP_AND_INVERTED = (GFX_LOGIC_OP_AND_REVERSE + 1),
        GFX_LOGIC_OP_OR_REVERSE = (GFX_LOGIC_OP_AND_INVERTED + 1),
        GFX_LOGIC_OP_OR_INVERTED = (GFX_LOGIC_OP_OR_REVERSE + 1)
    };

    enum ColorWriteEnable : u8
    {
        GFX_COLOR_WRITE_ENABLE_RED = 1,
        GFX_COLOR_WRITE_ENABLE_GREEN = 2,
        GFX_COLOR_WRITE_ENABLE_BLUE = 4,
        GFX_COLOR_WRITE_ENABLE_ALPHA = 8,
        GFX_COLOR_WRITE_ENABLE_ALL = (((GFX_COLOR_WRITE_ENABLE_RED | GFX_COLOR_WRITE_ENABLE_GREEN) | GFX_COLOR_WRITE_ENABLE_BLUE) | GFX_COLOR_WRITE_ENABLE_ALPHA)
    };

    struct RenderTargetBlendState
    {
        bool BlendEnable = false;
        bool LogicOpEnable = false;
        BlendType SrcBlend = GFX_BLEND_ONE;
        BlendType DestBlend = GFX_BLEND_ONE;
        BlendOpType BlendOp = GFX_BLEND_OP_ADD;
        BlendType SrcBlendAlpha = GFX_BLEND_ONE;
        BlendType DestBlendAlpha = GFX_BLEND_ONE;
        BlendOpType BlendOpAlpha = GFX_BLEND_OP_ADD;
        LogicOpType LogicOp = GFX_LOGIC_OP_NOOP;
        ColorWriteEnable RenderTargetWriteMask = GFX_COLOR_WRITE_ENABLE_ALL;
    };

    struct BlendState
    {
        RenderTargetBlendState BlendStates[8];
        bool AlphaToCoverageEnable;
        bool IndependentBlendEnable;
    };

    struct LUZ_API PipelineDesc
    {
        InputLayoutDesc InputLayout;
        SignatureDesc Signature;
        DepthStencilState DepthStencil;
        RasterizerState Rasterizer;
        BlendState Blend;
        ShaderHandle VertexShaderHandle = GPU_RESOURCE_HANDLE_INVALID;
        ShaderHandle HullShaderHandle = GPU_RESOURCE_HANDLE_INVALID;
        ShaderHandle DomainShaderHandle = GPU_RESOURCE_HANDLE_INVALID;
        ShaderHandle GeometryShaderHandle = GPU_RESOURCE_HANDLE_INVALID;
        ShaderHandle PixelShaderHandle = GPU_RESOURCE_HANDLE_INVALID;
        PrimitiveTopologyType Topology = GFX_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
        u32 SampleCount = 1;
        u32 SampleQuality = 0;
        u32 SampleMask = 0xffffffff;
        TextureHandle* pRenderTargets = nullptr;
        DepthStencilHandle DsHandle = GPU_RESOURCE_HANDLE_INVALID;
    };
}

#endif
#pragma once
#ifndef GRAPHICSTYPES_H
#define GRAPHICSTYPES_H

namespace Graphics
{
    enum LUZ_API Format : uint32_t
    {
        GFX_FORMAT_UNKNOWN = 0,
        GFX_FORMAT_R32G32B32A32_TYPELESS = 1,
        GFX_FORMAT_R32G32B32A32_FLOAT = 2,
        GFX_FORMAT_R32G32B32A32_UINT = 3,
        GFX_FORMAT_R32G32B32A32_SINT = 4,
        GFX_FORMAT_R32G32B32_TYPELESS = 5,
        GFX_FORMAT_R32G32B32_FLOAT = 6,
        GFX_FORMAT_R32G32B32_UINT = 7,
        GFX_FORMAT_R32G32B32_SINT = 8,
        GFX_FORMAT_R16G16B16A16_TYPELESS = 9,
        GFX_FORMAT_R16G16B16A16_FLOAT = 10,
        GFX_FORMAT_R16G16B16A16_UNORM = 11,
        GFX_FORMAT_R16G16B16A16_UINT = 12,
        GFX_FORMAT_R16G16B16A16_SNORM = 13,
        GFX_FORMAT_R16G16B16A16_SINT = 14,
        GFX_FORMAT_R32G32_TYPELESS = 15,
        GFX_FORMAT_R32G32_FLOAT = 16,
        GFX_FORMAT_R32G32_UINT = 17,
        GFX_FORMAT_R32G32_SINT = 18,
        GFX_FORMAT_R32G8X24_TYPELESS = 19,
        GFX_FORMAT_D32_FLOAT_S8X24_UINT = 20,
        GFX_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
        GFX_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
        GFX_FORMAT_R10G10B10A2_TYPELESS = 23,
        GFX_FORMAT_R10G10B10A2_UNORM = 24,
        GFX_FORMAT_R10G10B10A2_UINT = 25,
        GFX_FORMAT_R11G11B10_FLOAT = 26,
        GFX_FORMAT_R8G8B8A8_TYPELESS = 27,
        GFX_FORMAT_R8G8B8A8_UNORM = 28,
        GFX_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
        GFX_FORMAT_R8G8B8A8_UINT = 30,
        GFX_FORMAT_R8G8B8A8_SNORM = 31,
        GFX_FORMAT_R8G8B8A8_SINT = 32,
        GFX_FORMAT_R16G16_TYPELESS = 33,
        GFX_FORMAT_R16G16_FLOAT = 34,
        GFX_FORMAT_R16G16_UNORM = 35,
        GFX_FORMAT_R16G16_UINT = 36,
        GFX_FORMAT_R16G16_SNORM = 37,
        GFX_FORMAT_R16G16_SINT = 38,
        GFX_FORMAT_R32_TYPELESS = 39,
        GFX_FORMAT_D32_FLOAT = 40,
        GFX_FORMAT_R32_FLOAT = 41,
        GFX_FORMAT_R32_UINT = 42,
        GFX_FORMAT_R32_SINT = 43,
        GFX_FORMAT_R24G8_TYPELESS = 44,
        GFX_FORMAT_D24_UNORM_S8_UINT = 45,
        GFX_FORMAT_R24_UNORM_X8_TYPELESS = 46,
        GFX_FORMAT_X24_TYPELESS_G8_UINT = 47,
        GFX_FORMAT_R8G8_TYPELESS = 48,
        GFX_FORMAT_R8G8_UNORM = 49,
        GFX_FORMAT_R8G8_UINT = 50,
        GFX_FORMAT_R8G8_SNORM = 51,
        GFX_FORMAT_R8G8_SINT = 52,
        GFX_FORMAT_R16_TYPELESS = 53,
        GFX_FORMAT_R16_FLOAT = 54,
        GFX_FORMAT_D16_UNORM = 55,
        GFX_FORMAT_R16_UNORM = 56,
        GFX_FORMAT_R16_UINT = 57,
        GFX_FORMAT_R16_SNORM = 58,
        GFX_FORMAT_R16_SINT = 59,
        GFX_FORMAT_R8_TYPELESS = 60,
        GFX_FORMAT_R8_UNORM = 61,
        GFX_FORMAT_R8_UINT = 62,
        GFX_FORMAT_R8_SNORM = 63,
        GFX_FORMAT_R8_SINT = 64,
        GFX_FORMAT_A8_UNORM = 65,
        GFX_FORMAT_R1_UNORM = 66,
        GFX_FORMAT_R9G9B9E5_SHAREDEXP = 67,
        GFX_FORMAT_R8G8_B8G8_UNORM = 68,
        GFX_FORMAT_G8R8_G8B8_UNORM = 69,
        GFX_FORMAT_BC1_TYPELESS = 70,
        GFX_FORMAT_BC1_UNORM = 71,
        GFX_FORMAT_BC1_UNORM_SRGB = 72,
        GFX_FORMAT_BC2_TYPELESS = 73,
        GFX_FORMAT_BC2_UNORM = 74,
        GFX_FORMAT_BC2_UNORM_SRGB = 75,
        GFX_FORMAT_BC3_TYPELESS = 76,
        GFX_FORMAT_BC3_UNORM = 77,
        GFX_FORMAT_BC3_UNORM_SRGB = 78,
        GFX_FORMAT_BC4_TYPELESS = 79,
        GFX_FORMAT_BC4_UNORM = 80,
        GFX_FORMAT_BC4_SNORM = 81,
        GFX_FORMAT_BC5_TYPELESS = 82,
        GFX_FORMAT_BC5_UNORM = 83,
        GFX_FORMAT_BC5_SNORM = 84,
        GFX_FORMAT_B5G6R5_UNORM = 85,
        GFX_FORMAT_B5G5R5A1_UNORM = 86,
        GFX_FORMAT_B8G8R8A8_UNORM = 87,
        GFX_FORMAT_B8G8R8X8_UNORM = 88,
        GFX_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
        GFX_FORMAT_B8G8R8A8_TYPELESS = 90,
        GFX_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
        GFX_FORMAT_B8G8R8X8_TYPELESS = 92,
        GFX_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
        GFX_FORMAT_BC6H_TYPELESS = 94,
        GFX_FORMAT_BC6H_UF16 = 95,
        GFX_FORMAT_BC6H_SF16 = 96,
        GFX_FORMAT_BC7_TYPELESS = 97,
        GFX_FORMAT_BC7_UNORM = 98,
        GFX_FORMAT_BC7_UNORM_SRGB = 99,
        GFX_FORMAT_AYUV = 100,
        GFX_FORMAT_Y410 = 101,
        GFX_FORMAT_Y416 = 102,
        GFX_FORMAT_NV12 = 103,
        GFX_FORMAT_P010 = 104,
        GFX_FORMAT_P016 = 105,
        GFX_FORMAT_420_OPAQUE = 106,
        GFX_FORMAT_YUY2 = 107,
        GFX_FORMAT_Y210 = 108,
        GFX_FORMAT_Y216 = 109,
        GFX_FORMAT_NV11 = 110,
        GFX_FORMAT_AI44 = 111,
        GFX_FORMAT_IA44 = 112,
        GFX_FORMAT_P8 = 113,
        GFX_FORMAT_A8P8 = 114,
        GFX_FORMAT_B4G4R4A4_UNORM = 115,
        GFX_FORMAT_P208 = 130,
        GFX_FORMAT_V208 = 131,
        GFX_FORMAT_V408 = 132,
        GFX_FORMAT_FORCE_UINT = 0xffffffff
    };

    enum LUZ_API ResourceDimension : uint8_t
    {
        GFX_RESOURCE_DIMENSION_UNKNOWN = 0,
        GFX_RESOURCE_DIMENSION_BUFFER = 1,
        GFX_RESOURCE_DIMENSION_TEXTURE1D = 2,
        GFX_RESOURCE_DIMENSION_TEXTURE2D = 3,
        GFX_RESOURCE_DIMENSION_TEXTURE3D = 4
    };

    enum LUZ_API PrimitiveTopologyType : u8
    {
        GFX_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED = 0,
        GFX_PRIMITIVE_TOPOLOGY_TYPE_POINT = 1,
        GFX_PRIMITIVE_TOPOLOGY_TYPE_LINE = 2,
        GFX_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE = 3,
        GFX_PRIMITIVE_TOPOLOGY_TYPE_PATCH = 4
    };

    enum LUZ_API PrimitiveSubtopology : u8
    {
        GFX_PRIMITIVE_TOPOLOGY_UNDEFINED = 0,
        GFX_PRIMITIVE_TOPOLOGY_POINTLIST = 1,
        GFX_PRIMITIVE_TOPOLOGY_LINELIST = 2,
        GFX_PRIMITIVE_TOPOLOGY_LINESTRIP = 3,
        GFX_PRIMITIVE_TOPOLOGY_TRIANGLELIST = 4,
        GFX_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP = 5,
        GFX_PRIMITIVE_TOPOLOGY_LINELIST_ADJ = 10,
        GFX_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ = 11,
        GFX_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ = 12,
        GFX_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ = 13,
        GFX_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST = 33,
        GFX_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST = 34,
        GFX_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST = 35,
        GFX_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST = 36,
        GFX_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST = 37,
        GFX_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST = 38,
        GFX_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST = 39,
        GFX_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST = 40,
        GFX_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST = 41,
        GFX_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST = 42,
        GFX_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST = 43,
        GFX_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST = 44,
        GFX_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST = 45,
        GFX_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST = 46,
        GFX_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST = 47,
        GFX_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST = 48,
        GFX_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST = 49,
        GFX_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST = 50,
        GFX_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST = 51,
        GFX_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST = 52,
        GFX_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST = 53,
        GFX_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST = 54,
        GFX_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST = 55,
        GFX_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST = 56,
        GFX_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST = 57,
        GFX_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST = 58,
        GFX_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST = 59,
        GFX_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST = 60,
        GFX_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST = 61,
        GFX_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST = 62,
        GFX_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST = 63,
        GFX_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST = 64,
    };

    enum LUZ_API DepthStencilFlags : uint8_t
    {
        GFX_DEPTH_STENCIL_FLAG_NONE = 0,
        GFX_DEPTH_STENCIL_FLAG_SHADER_RESOURCE
    };

    enum LUZ_API ResourceFlags : uint8_t
    {
        GFX_RESOURCE_FLAG_NONE = 0,
        GFX_RESOURCE_FLAG_ALLOW_RENDER_TARGET = 0x1,
        GFX_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL = 0x2,
        GFX_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS = 0x4,
        GFX_RESOURCE_FLAG_DENY_SHADER_RESOURCE = 0x8,
        GFX_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER = 0x10,
        GFX_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS = 0x20,
        GFX_RESOURCE_FLAG_VIDEO_DECODE_REFERENCE_ONLY = 0x40
    };

    enum LUZ_API ResourceStates
    {
        GFX_RESOURCE_STATE_COMMON = 0,
        GFX_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER = 0x1,
        GFX_RESOURCE_STATE_INDEX_BUFFER = 0x2,
        GFX_RESOURCE_STATE_RENDER_TARGET = 0x4,
        GFX_RESOURCE_STATE_UNORDERED_ACCESS = 0x8,
        GFX_RESOURCE_STATE_DEPTH_WRITE = 0x10,
        GFX_RESOURCE_STATE_DEPTH_READ = 0x20,
        GFX_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE = 0x40,
        GFX_RESOURCE_STATE_PIXEL_SHADER_RESOURCE = 0x80,
        GFX_RESOURCE_STATE_STREAM_OUT = 0x100,
        GFX_RESOURCE_STATE_INDIRECT_ARGUMENT = 0x200,
        GFX_RESOURCE_STATE_COPY_DEST = 0x400,
        GFX_RESOURCE_STATE_COPY_SOURCE = 0x800,
        GFX_RESOURCE_STATE_RESOLVE_DEST = 0x1000,
        GFX_RESOURCE_STATE_RESOLVE_SOURCE = 0x2000,
        GFX_RESOURCE_STATE_GENERIC_READ = (((((0x1 | 0x2) | 0x40) | 0x80) | 0x200) | 0x800),
        GFX_RESOURCE_STATE_PRESENT = 0,
        GFX_RESOURCE_STATE_PREDICATION = 0x200,
        GFX_RESOURCE_STATE_VIDEO_DECODE_READ = 0x10000,
        GFX_RESOURCE_STATE_VIDEO_DECODE_WRITE = 0x20000,
        GFX_RESOURCE_STATE_VIDEO_PROCESS_READ = 0x40000,
        GFX_RESOURCE_STATE_VIDEO_PROCESS_WRITE = 0x80000
    };

    inline constexpr ResourceStates operator | (ResourceStates a, ResourceStates b) throw() { return ResourceStates(((std::underlying_type<ResourceStates>::type)a) | ((std::underlying_type<ResourceStates>::type)b)); }
    inline ResourceStates &operator |= (ResourceStates &a, ResourceStates b) throw() { return (ResourceStates &)(((std::underlying_type<ResourceStates>::type &)a) |= ((std::underlying_type<ResourceStates>::type)b)); }
    inline constexpr ResourceStates operator & (ResourceStates a, ResourceStates b) throw() { return ResourceStates(((std::underlying_type<ResourceStates>::type)a) & ((std::underlying_type<ResourceStates>::type)b)); }
    inline ResourceStates &operator &= (ResourceStates &a, ResourceStates b) throw() { return (ResourceStates &)(((std::underlying_type<ResourceStates>::type &)a) &= ((std::underlying_type<ResourceStates>::type)b)); }
    inline constexpr ResourceStates operator ~ (ResourceStates a) throw() { return ResourceStates(~((std::underlying_type<ResourceStates>::type)a)); }
    inline constexpr ResourceStates operator ^ (ResourceStates a, ResourceStates b) throw() { return ResourceStates(((std::underlying_type<ResourceStates>::type)a) ^ ((std::underlying_type<ResourceStates>::type)b)); }
    inline ResourceStates &operator ^= (ResourceStates &a, ResourceStates b) throw() { return (ResourceStates &)(((std::underlying_type<ResourceStates>::type &)a) ^= ((std::underlying_type<ResourceStates>::type)b)); }

    struct LUZ_API BufferDesc
    {
        uint64_t Alignment;
        uint64_t SizeInBytes;
        uint16_t StrideInBytes;
        void* pData = nullptr;
    };

    struct LUZ_API ConstantBufferDesc
    {
        uint64_t Alignment;
        uint64_t SizeInBytes;
        uint16_t StrideInBytes;
        bool AllocHeap;
        void* pData = nullptr;
    };

    struct LUZ_API TextureDesc
    {
        uint64_t SizeInBytes;
        uint64_t Width;
        uint32_t Height;
        uint16_t Depth = 1;
        uint16_t MipLevels = 0;
        uint32_t SampleCount = 1;
        uint32_t SampleQuality = 0;
        Format Format;
        ResourceDimension Dimension;
        ResourceFlags Flags;
        bool bIsCube = false;
        float* pColor = nullptr;
        void* pData = nullptr;
    };

    struct LUZ_API TextureFileDesc
    {
        const char* Filename;
        bool GenMips = false;
    };

    struct LUZ_API RenderTargetDesc
    {
        uint64_t Width;
        uint32_t Height;
        Format Format;
        float* pColor;
    };

    struct LUZ_API DepthStencilDesc
    {
        uint64_t Width;
        uint32_t Height;
        float ClearDepth;
        uint8_t ClearStencil;
        Format Format;
        DepthStencilFlags Flags;
    };

    struct LUZ_API DescriptorTableDesc
    {

    };

    typedef uint16_t GpuResourceHandle;

    typedef GpuResourceHandle VertexBufferHandle;
    
    typedef GpuResourceHandle IndexBufferHandle;
    
    typedef GpuResourceHandle ConstantBufferHandle;
    
    typedef GpuResourceHandle StructuredBufferHandle;
    
    typedef GpuResourceHandle TextureHandle;
    
    typedef GpuResourceHandle RenderTargetHandle;
    
    typedef GpuResourceHandle DepthStencilHandle;

    typedef GpuResourceHandle ShaderHandle;
    
    typedef GpuResourceHandle PipelineStateHandle;

    typedef GpuResourceHandle CommandStreamHandle;

    typedef GpuResourceHandle DescriptorHandle;

    typedef GpuResourceHandle DescriptorTableHandle;

    extern const Graphics::GpuResourceHandle GPU_RESOURCE_HANDLE_INVALID;

    typedef uint8_t CommandQueueHandle;

    enum CommandQueueType : uint8_t
    {
        GFX_COMMAND_QUEUE_TYPE_DRAW,
        GFX_COMMAND_QUEUE_TYPE_COPY,
        GFX_COMMAND_QUEUE_TYPE_COMPUTE,
        GFX_COMMAND_QUEUE_TYPE_NUM_TYPES
    };

    struct LUZ_API CommandStreamDesc
    {
        CommandQueueType QueueType;
        CommandQueueHandle QueueHandle;
        PipelineStateHandle PipelineHandle;
        ConstantBufferHandle pConstants[16];
        TextureHandle pTextures[32];
    };

    struct LUZ_API Viewport
    {
        float TopLeftX;
        float TopLeftY;
        float Width;
        float Height;
        float MinDepth;
        float MaxDepth;
    };

    struct LUZ_API Rect
    {
        u32 Left;
        u32 Top;
        u32 Right;
        u32 Bottom;
    };
}

#endif
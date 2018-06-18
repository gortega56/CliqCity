#pragma once
#ifndef GRAPHICSTYPES_H
#define GRAPHICSTYPES_H

namespace Graphics
{
    enum Format : uint8_t
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

    enum GpuResourceFlags : uint16_t
    {

    };

    struct BufferDesc
    {
        uint64_t SizeInBytes;
        uint64_t Alignment;
        uint16_t StrideInBytes;
    };

    struct TextureDesc
    {

    };

    struct DepthStencilDesc
    {
        uint64_t Width;
        uint32_t Height;
        float ClearDepth;
        uint8_t ClearStencil;
        Format Format;
    };

    enum GpuResourceHandle : uint16_t
    {
        GPU_RESOURCE_HANDLE_INVALID = -1
    };

    typedef GpuResourceHandle VertexBufferHandle;
    typedef GpuResourceHandle IndexBufferHandle;
    typedef GpuResourceHandle ConstantBufferHandle;
    typedef GpuResourceHandle StructuredBufferHandle;
    typedef GpuResourceHandle TextureHandle;
    typedef GpuResourceHandle DepthStencilHandle;
    typedef GpuResourceHandle ShaderHandle;
    typedef GpuResourceHandle PipelineStateHandle;
    
}

#endif
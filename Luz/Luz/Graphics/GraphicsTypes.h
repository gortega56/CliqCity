#pragma once
#ifndef GRAPHICSTYPES_H
#define GRAPHICSTYPES_H

namespace Graphics
{
    enum class GpuResourceHandle : uint16_t
    {
        GPU_RESOURCE_HANDLE_INVALID = -1
    };

    typedef GpuResourceHandle VertexBufferHandle;
    typedef GpuResourceHandle IndexBufferHandle;
    typedef GpuResourceHandle ConstantBufferHandle;
    typedef GpuResourceHandle StructuredBufferHandle;
    typedef GpuResourceHandle TextureHandle;
    typedef GpuResourceHandle DepthStencilHandle;

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
}

#endif
#pragma once
#ifndef COMMANDSTREAM_H
#define COMMANDSTREAM_H

#ifndef GRAPHICS_H
#include "Graphics.h"
#endif

namespace Graphics
{
    struct DescriptorTableRange
    {
        static constexpr unsigned int s_max_constants = 14;
        static constexpr unsigned int s_max_textures = 128;
        static constexpr unsigned int s_max_resources = s_max_constants + s_max_textures;

        unsigned int Register;
        unsigned int nHandles;
        GpuResourceHandle pHandles[s_max_resources];
    };

    class LUZ_API CommandStream
    {
    public:
        CommandStream();

        CommandStream(const CommandStreamHandle handle);

        ~CommandStream();

       //void Reset(const PipelineStateHandle handle);

        void SetPipeline(const PipelineStateHandle handle);

        void SetRenderTargets(const u32 numRenderTargets, const TextureHandle* pTextureHandles, const u32* pMipSlices, const u32* pArraySlices, const DepthStencilHandle dsHandle);

        void SetRenderTargets(const u32 numRenderTargets, const RenderTargetHandle* pRtHandles, const DepthStencilHandle dsHandle);

        void SetRenderTargets();    // Sets swap chain

        void SetViewport(const Viewport& viewport);

        void SetViewport(const float topLeftX, const float topLeftY, const float width, const float height, const float minDepth, const float maxDepth);

        void SetScissorRect(const Rect& rect);

        void SetScissorRect(const u32 left, const u32 top, const u32 right, const u32 bottom);

        void ClearRenderTarget(const float* pColor, const TextureHandle handle, const u32 mipSlice = 0, const u32 arraySlice = 0);

        void ClearRenderTarget(const float* pColor);

        void ClearDepthStencil(const float depth, const u8 stencil, const DepthStencilHandle handle);

        void ClearDepthStencil(const float depth, const u8 stencil);

        void SetPrimitiveTopology(const PrimitiveSubtopology topology);

        void SetVertexBuffer(const VertexBufferHandle handle);

        void SetIndexBuffer(const IndexBufferHandle handle);

        void SetConstantBuffer(const u32 param, const ConstantBufferHandle handle);

        void SetTexture(const u32 param, const TextureHandle handle);

        void SetDescriptorTable(const u32 param, const GpuResourceHandle baseHandle);

        void SetDescriptorTable(const u32 param, const GpuResourceHandle* pHandles, const u32 nHandles);

        void SetDescriptorTable(const DescriptorTableRange* pDescriptorTableRanges, const u32 nDescriptorTableRanges);

        void TransitionDepthStencil(const DepthStencilHandle handle, const ResourceStates before, const ResourceStates after);

        void TransitionRenderTarget(const RenderTargetHandle handle, const ResourceStates before, const ResourceStates after);

        void CommandStream::TransitionRenderTarget(const ResourceStates before, const ResourceStates after);

        void TransitionTexture(const TextureHandle handle, const ResourceStates before, const ResourceStates after);

        void DrawInstanceIndexed(const IndexBufferHandle handle, const u32 instanceCount = 1, const u32 startIndex = 0, const i32 baseVertexLocation = 0, const u32 startInstanceLocation = 0);

        inline const CommandStreamHandle GetHandle() const
        {
            return m_handle;
        }

    private:
        CommandStreamHandle m_handle;
    };

}

#endif
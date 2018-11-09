#pragma once
#ifndef COMMANDSTREAM_H
#define COMMANDSTREAM_H

#ifndef GRAPHICS_H
#include "Graphics.h"
#endif

namespace Graphics
{
    class LUZ_API CommandStream
    {
    public:
        CommandStream();

        CommandStream(const CommandStreamHandle handle);

        ~CommandStream();

       //void Reset(const PipelineStateHandle handle);

        void SetPipeline(const PipelineStateHandle handle);

        void SetRenderTargets(const u32 numRenderTargets, const RenderTargetHandle* pRtHandles, const DepthStencilHandle dsHandle);

        void SetRenderTargets();    // Clears swap chain

        void SetViewport(const Viewport& viewport);

        void SetScissorRect(const Rect& rect);

        void ClearRenderTarget(const float* pColor, const RenderTargetHandle handle);

        void ClearRenderTarget(const float* pColor);

        void ClearDepthStencil(const float depth, const u8 stencil, const DepthStencilHandle handle);

        void ClearDepthStencil(const float depth, const u8 stencil);

        void SetPrimitiveTopology(const PrimitiveSubtopology topology);

        void SetVertexBuffer(const VertexBufferHandle handle);

        void SetIndexBuffer(const IndexBufferHandle handle);

        void SetConstantBuffer(const u32 param, const ConstantBufferHandle handle);

        void SetTexture(const u32 param, const TextureHandle handle);

        void SetDescriptorTable(const u32 param, const GpuResourceHandle baseHandle);

        void TransitionDepthStencilToDepthWrite(const DepthStencilHandle handle);

        void TransitionDepthStencilToTexture(const DepthStencilHandle handle);

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
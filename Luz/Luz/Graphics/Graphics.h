#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#ifndef GRAPHICSTYPES_H
#include "GraphicsTypes.h"
#endif

#ifndef PIPELINESTATES_H
#include "PipelineStates.h"
#endif

#define DX12

class Window;

/* TODO: 
    1. Integrate shadow maps in pixel shader.
    2. Remove the hacks for finding a descriptor by a handle. 
*/
namespace Graphics
{
    class CommandStream;

    LUZ_API bool Initialize(Window* pWindow, u32 numBackBuffers);

    LUZ_API void Shutdown();

    LUZ_API ShaderHandle CreateVertexShader(const char* filename);
    
    LUZ_API ShaderHandle CreateHullShader(const char* filename);
    
    LUZ_API ShaderHandle CreateDomainShader(const char* filename);
    
    LUZ_API ShaderHandle CreateGeometryShader(const char* filename);
    
    LUZ_API ShaderHandle CreatePixelShader(const char* filename);

    LUZ_API PipelineStateHandle CreateGraphicsPipelineState(const PipelineDesc& desc);

    LUZ_API RenderTargetHandle CreateRenderTarget(const RenderTargetDesc& desc);
    
    LUZ_API DepthStencilHandle CreateDepthStencil(const DepthStencilDesc& desc);
    
    LUZ_API VertexBufferHandle CreateVertexBuffer(const BufferDesc& desc);
    
    LUZ_API IndexBufferHandle CreateIndexBuffer(const BufferDesc& desc);
    
    LUZ_API ConstantBufferHandle CreateConstantBuffer(const ConstantBufferDesc& desc);
    
    LUZ_API TextureHandle CreateTexture(const TextureDesc& desc);

    LUZ_API TextureHandle CreateTexture(const TextureFileDesc& desc);

    LUZ_API void ReleaseShader(const ShaderHandle handle);

    LUZ_API void ReleasePipeline(const PipelineStateHandle handle);

    LUZ_API void ReleaseRenderTarget(const RenderTargetHandle handle);

    LUZ_API void ReleaseDepthStencil(const DepthStencilHandle handle);

    LUZ_API void ReleaseVertexBuffer(const VertexBufferHandle handle);

    LUZ_API void ReleaseIndexBuffer(const IndexBufferHandle handle);

    LUZ_API void ReleaseConstantBuffer(const ConstantBufferHandle handle);

    LUZ_API void ReleaseTexture(const TextureHandle handle);

    LUZ_API void UpdateConstantBuffer(const void* pData, const u64 sizeInBytes, const ConstantBufferHandle handle);

    LUZ_API void SubmitCommandStream(const CommandStream* pCommandStream, bool wait = false);

    LUZ_API void Present();

    LUZ_API void CreateCommandStream(const CommandStreamDesc& desc, CommandStream* pCommandStream);

    LUZ_API void ReleaseCommandStream(CommandStream* pCommandStream);

    class LUZ_API CommandStream
    {
    public:
        CommandStream();

        CommandStream(const CommandStreamHandle handle);
        
        ~CommandStream();

        void Reset(const PipelineStateHandle handle);

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

        void SetDescriptorTable(const u32 param, const ConstantBufferHandle baseHandle);

        void SetDescriptorTable_FixLater(const u32 param, const ConstantBufferHandle cb, const DepthStencilHandle baseHandle);

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

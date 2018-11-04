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
}

#endif

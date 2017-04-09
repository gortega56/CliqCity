#pragma once
#ifndef DX12RENDERER_H
#define DX12RENDERER_H 

#ifndef TYPEDEFS_H
#include "TypeDefs.h"
#endif

#ifndef DESCRIPTORHEAP_H
#include "DescriptorHeap.h"
#endif

#ifndef DX12COMMANDCONTEXT_H
#include "CommandContext.h"
#endif

#ifndef GPURESOURCE_H
#include "GpuResource.h"
#endif

#ifndef DX12PIPELINE_H
#include "PipelineState.h"
#endif

#ifndef DX12RENDERCONTEXT_H
#include "RenderContext.h"
#endif 


#ifndef DEVICE_H
#include "Device.h"
#endif

#ifndef DX12VIEWPORT_H
#include "Viewport.h"
#endif

#include <memory>
#include <vector>

class OSWin;

namespace dx12
{
    class CommandQueue;
    class SwapChain;

    class Renderer
    {
    public:
        Renderer(OSWin* pOS, u32 numThreads, u32 numFrameBuffers);
        ~Renderer();

        bool Initialize(int width, int height, bool fullscreen);
        bool Shutdown();

        friend class Renderable;
        friend class DescriptorHeap;
        friend class CommandContext;
        friend class GraphicsCommandContext;
        friend class GpuResource;
        friend class GpuBuffer;
        friend class UploadBuffer;
        friend class PixelBuffer;
        friend class DepthBuffer;
        friend class Pipeline;
        friend class GraphicsPipeline;
        friend class RootSignature;
        friend class RenderContext;

        void SetRenderContext();
        void ClearRenderContext();
        void SetViewport();


        void Prepare(Renderable* pRenderable);

        void DrawIndexedInstanced(Renderable* pRenderable, u32 instanceCount = 1, u32 startIndex = 0, i32 baseVertex = 0, u32 startInstance = 0);

        void Present();

        void SetGraphicsRootConstantBuffer(GraphicsCommandContext* pContext, UploadBuffer* pUploadBuffer, u32 paramIndex = 0);
        void SetGraphicsRootConstantBuffer(UploadBuffer* pUploadBuffer, u32 paramIndex = 0);

        void ExecuteGraphicsCommandContext(std::shared_ptr<GraphicsCommandContext> pGraphicsCommandCtx);
        void ExecuteGraphicsCommandContext();

        std::shared_ptr<const Device> GetDevice() const { return m_device; };
        std::shared_ptr<const CommandQueue> GetCommandQueue() const { return m_commandQueue; }
        std::shared_ptr<GraphicsCommandContext> GetGraphicsContext() const { return m_graphicsCommandCtx; }
        std::shared_ptr<RenderContext> GetRenderContext() const { return m_renderContext; }

        u32 NumThreads() { return m_numThreads; }
        u32 NumFrameBuffers() { return m_graphicsCommandCtx->NumFrameBuffers(); }

        u32 Width() const { return m_renderContext->Width(); }
        u32 Height() const { return m_renderContext->Height(); }
        float AspectRatio() const { return m_renderContext->AspectRatio(); }

    protected:
        HWND m_hwnd;

        std::shared_ptr<Device> m_device;
        std::shared_ptr<CommandQueue> m_commandQueue;
        std::shared_ptr<GraphicsCommandContext> m_graphicsCommandCtx;
        std::shared_ptr<RenderContext> m_renderContext;
        std::shared_ptr<SwapChain> m_swapChain;

        Viewport m_viewport;

        ID3D12Debug* m_debug;

    private:
        u32 m_numThreads;
        bool m_running;

        Renderer(const Renderer& other) = delete;
        Renderer& operator=(const Renderer& other) = delete;
    };
}

#endif

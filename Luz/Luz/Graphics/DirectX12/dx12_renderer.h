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
        friend class DynamicBuffer;
        friend class PixelBuffer;
        friend class DepthBuffer;
        friend class Pipeline;
        friend class GraphicsPipeline;
        friend class RootSignature;
        friend class RenderContext;

        void SetGraphicsPipeline(GraphicsPipeline* pGraphicsPipeline);

        void SetRootSignature(RootSignature* pRootSignature);

        void SetRenderContext(RenderContext* pRenderContext);
        void SetRenderContext();

        void ClearRenderContext(RenderContext* pRenderContext);
        void ClearRenderContext();

        void SetViewport();

        void UpdatePipeline(GraphicsPipeline* pGraphicsPipeline);

        void Prepare(Renderable* pRenderable);

        void DrawIndexedInstanced(Renderable* pRenderable, u32 instanceCount = 1, u32 startIndex = 0, i32 baseVertex = 0, u32 startInstance = 0);

        void Present();

        void ExecuteGraphicsCommandContext(GraphicsCommandContext* pGraphicsCommandCtx);
        void ExecuteGraphicsCommandContext();

        u32 NumThreads() { return m_graphicsCommandCtx.NumThreads(); }
        u32 NumFrameBuffers() { return m_graphicsCommandCtx.NumFrameBuffers(); }

    protected:
        HWND m_hwnd;

        Device m_device;
        GraphicsCommandContext m_graphicsCommandCtx;

        RenderContext m_renderContext;
        
        ID3D12CommandQueue* m_commandQueue;


        Viewport m_viewport;

    private:
        Renderer(const Renderer& other) = delete;
        Renderer& operator=(const Renderer& other) = delete;
        bool m_running;
    };
}

#endif

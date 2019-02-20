#include "stdafx.h"
#include "CommandStream.h"

#ifdef DX12
#include "Dx12GraphicsTypes.h"
#include "Dx12DescriptorAllocator.h"

namespace Graphics
{
    static void TransitionResource(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* pResource, const D3D12_RESOURCE_STATES before, const D3D12_RESOURCE_STATES after)
    {
        LUZASSERT(pCommandList);
        LUZASSERT(pResource);

        pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pResource, before, after));
    }

    CommandStream::CommandStream()
        : CommandStream((CommandStreamHandle)-1)
    {

    }

    CommandStream::CommandStream(const CommandStreamHandle handle)
        : m_handle(handle)
    {

    }

    CommandStream::~CommandStream()
    {
        ReleaseCommandStream(this);
    }

    //void CommandStream::Reset(const PipelineStateHandle handle)
    //{
    //    ResetCommandStream(this, handle);
    //}

    void CommandStream::SetPipeline(const PipelineStateHandle handle)
    {
        Pipeline& pso = s_pipelineCollection.GetData(handle);
        CommandList& cl = s_commandListCollection.GetData(m_handle);

        cl.pGraphicsCommandList->SetPipelineState(pso.pPipelineState);
        cl.pGraphicsCommandList->SetGraphicsRootSignature(pso.pSignature);
    }

    void CommandStream::SetRenderTargets(const u32 numRenderTargets, const TextureHandle* pTextureHandles, const u32* pMipSlices, const u32* pArraySlices, const DepthStencilHandle dsHandle)
    {
        CommandList& cl = s_commandListCollection.GetData(m_handle);

        D3D12_CPU_DESCRIPTOR_HANDLE pHandles[8] = { 0 };
        if (pMipSlices && pArraySlices)
        {
            for (u32 i = 0; i < numRenderTargets; ++i)
            {
                const Texture& texture = s_textureCollection.GetData(pTextureHandles[i]);
                const D3D12_RESOURCE_DESC desc = texture.pResource->GetDesc();
                const UINT offset = static_cast<UINT>(pMipSlices[i]) * desc.DepthOrArraySize + static_cast<UINT>(pArraySlices[i]);
                const UINT size = GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
                pHandles[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(texture.RtvHandle.CpuHandle, offset, size);
            }
        }
        else
        {
            for (u32 i = 0; i < numRenderTargets; ++i)
            {
                const Texture& texture = s_textureCollection.GetData(pTextureHandles[i]);
                pHandles[i] = texture.RtvHandle.CpuHandle;
            }
        }

        D3D12_CPU_DESCRIPTOR_HANDLE* pDsHandle = nullptr;
        if (dsHandle)
        {
            pDsHandle = &s_depthStencilCollection.GetData(dsHandle).DsvHandle.CpuHandle;
        }

        cl.pGraphicsCommandList->OMSetRenderTargets(numRenderTargets, pHandles, FALSE, pDsHandle);
    }

    void CommandStream::SetRenderTargets(const u32 numRenderTargets, const RenderTargetHandle* pRtHandles, const DepthStencilHandle dsHandle)
    {
        CommandList& cl = s_commandListCollection.GetData(m_handle);

        D3D12_CPU_DESCRIPTOR_HANDLE pHandles[8] = { 0 };
        for (u32 i = 0; i < numRenderTargets; ++i)
        {
            pHandles[i] = s_renderTargetCollection.GetData(pRtHandles[i]).CpuHandle;
        }

        D3D12_CPU_DESCRIPTOR_HANDLE* pDsHandle = nullptr;
        if (dsHandle)
        {
            pDsHandle = &s_depthStencilCollection.GetData(dsHandle).DsvHandle.CpuHandle;
        }

        cl.pGraphicsCommandList->OMSetRenderTargets(numRenderTargets, pHandles, FALSE, pDsHandle);
    }

    void CommandStream::SetRenderTargets()
    {
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        
        cl.pGraphicsCommandList->OMSetRenderTargets(1, &s_swapChain.RenderTargetViewHandles[s_swapChain.FrameIndex], TRUE, &s_swapChain.DepthStencilViewHandle);
    }

    void CommandStream::SetViewport(const Viewport& viewport)
    {
        D3D12_VIEWPORT vp;
        ZeroMemory(&vp, sizeof(D3D12_VIEWPORT));
        vp.TopLeftX = static_cast<FLOAT>(viewport.TopLeftX);
        vp.TopLeftY = static_cast<FLOAT>(viewport.TopLeftY);
        vp.Width = static_cast<FLOAT>(viewport.Width);
        vp.Height = static_cast<FLOAT>(viewport.Height);
        vp.MinDepth = static_cast<FLOAT>(viewport.MinDepth);
        vp.MaxDepth = static_cast<FLOAT>(viewport.MaxDepth);

        CommandList& cl = s_commandListCollection.GetData(m_handle);
        cl.pGraphicsCommandList->RSSetViewports(1, &vp);
    }

    void CommandStream::SetViewport(const float topLeftX, const float topLeftY, const float width, const float height, const float minDepth, const float maxDepth)
    {
        D3D12_VIEWPORT vp;
        ZeroMemory(&vp, sizeof(D3D12_VIEWPORT));
        vp.TopLeftX = static_cast<FLOAT>(topLeftX);
        vp.TopLeftY = static_cast<FLOAT>(topLeftY);
        vp.Width = static_cast<FLOAT>(width);
        vp.Height = static_cast<FLOAT>(height);
        vp.MinDepth = static_cast<FLOAT>(minDepth);
        vp.MaxDepth = static_cast<FLOAT>(maxDepth);

        CommandList& cl = s_commandListCollection.GetData(m_handle);
        cl.pGraphicsCommandList->RSSetViewports(1, &vp);
    }

    void CommandStream::SetScissorRect(const Rect& rect)
    {
        D3D12_RECT scissor;
        ZeroMemory(&scissor, sizeof(D3D12_RECT));
        scissor.left = static_cast<LONG>(rect.Left);
        scissor.top = static_cast<LONG>(rect.Top);
        scissor.right = static_cast<LONG>(rect.Right);
        scissor.bottom = static_cast<LONG>(rect.Bottom);

        CommandList& cl = s_commandListCollection.GetData(m_handle);
        cl.pGraphicsCommandList->RSSetScissorRects(1, &scissor);
    }

    void CommandStream::SetScissorRect(const u32 left, const u32 top, const u32 right, const u32 bottom)
    {
        D3D12_RECT scissor;
        ZeroMemory(&scissor, sizeof(D3D12_RECT));
        scissor.left = static_cast<LONG>(left);
        scissor.top = static_cast<LONG>(top);
        scissor.right = static_cast<LONG>(right);
        scissor.bottom = static_cast<LONG>(bottom);

        CommandList& cl = s_commandListCollection.GetData(m_handle);
        cl.pGraphicsCommandList->RSSetScissorRects(1, &scissor);
    }

    void CommandStream::ClearRenderTarget(const float* pColor, const TextureHandle handle, const u32 mipSlice /*= 0*/, const u32 arraySlice /*= 0*/)
    {
        LUZASSERT(handle != GPU_RESOURCE_HANDLE_INVALID);
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        Texture& texture = s_textureCollection.GetData(handle);

        const D3D12_RESOURCE_DESC desc = texture.pResource->GetDesc();
        const UINT offset = static_cast<UINT>(mipSlice) * desc.DepthOrArraySize + static_cast<UINT>(arraySlice);
        const UINT size = GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        cl.pGraphicsCommandList->ClearRenderTargetView(CD3DX12_CPU_DESCRIPTOR_HANDLE(texture.RtvHandle.CpuHandle, offset, size), pColor, 0, nullptr);
    }

    void CommandStream::ClearRenderTarget(const float* pColor)
    {
        CommandList& cl = s_commandListCollection.GetData(m_handle);

        cl.pGraphicsCommandList->ClearRenderTargetView(s_swapChain.RenderTargetViewHandles[s_swapChain.FrameIndex], pColor, 0, nullptr);
    }

    void CommandStream::ClearDepthStencil(const float depth, const u8 stencil, const DepthStencilHandle handle)
    {
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        DepthStencil& ds = s_depthStencilCollection.GetData(handle);

        cl.pGraphicsCommandList->ClearDepthStencilView(ds.DsvHandle.CpuHandle, D3D12_CLEAR_FLAG_DEPTH, depth, stencil, 0, nullptr);
    }

    void CommandStream::ClearDepthStencil(const float depth, const u8 stencil)
    {
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        cl.pGraphicsCommandList->ClearDepthStencilView(s_swapChain.DepthStencilViewHandle, D3D12_CLEAR_FLAG_DEPTH, depth, stencil, 0, nullptr);
    }

    void CommandStream::SetPrimitiveTopology(const PrimitiveSubtopology topology)
    {
        CommandList& cl = s_commandListCollection.GetData(m_handle);

        cl.pGraphicsCommandList->IASetPrimitiveTopology(GetD3D12PrimitiveTogopology(topology));
    }

    void CommandStream::SetVertexBuffer(const VertexBufferHandle handle)
    {
        CommandList& cl = s_commandListCollection.GetData(m_handle);

        D3D12_VERTEX_BUFFER_VIEW* pView = (handle == GPU_RESOURCE_HANDLE_INVALID) ? nullptr : &s_vertexBufferCollection.GetData(handle).View;
        UINT numViews = (handle == GPU_RESOURCE_HANDLE_INVALID) ? 0 : 1;

        cl.pGraphicsCommandList->IASetVertexBuffers(0, numViews, pView);
    }

    void CommandStream::SetIndexBuffer(const IndexBufferHandle handle)
    {
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        IndexBuffer& ib = s_indexBufferCollection.GetData(handle);

        cl.pGraphicsCommandList->IASetIndexBuffer(&ib.View);
    }

    void CommandStream::SetConstantBuffer(const u32 param, const ConstantBufferHandle handle)
    {
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        ConstantBuffer cb = s_constantBufferCollection.GetData(handle);

        cl.pGraphicsCommandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(param), cb.GpuVirtualAddress);
    }

    void CommandStream::SetTexture(const u32 param, const TextureHandle handle)
    {
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        Texture tx = s_textureCollection.GetData(handle);

        cl.pGraphicsCommandList->SetGraphicsRootShaderResourceView(static_cast<UINT>(param), tx.GpuVirtualAddress);
    }

    void CommandStream::SetDescriptorTable(const u32 param, const GpuResourceHandle baseHandle)
    {
        Descriptor descriptor;

        auto eHandleType = static_cast<DescriptorHandleType>(HandleEncoder<GpuResourceHandle>::DecodeHandleValue(baseHandle, s_nDescriptorTypeBits));
        switch (eHandleType)
        {
        case GFX_DESCRIPTOR_HANDLE_TYPE_CBV: descriptor = s_constantBufferCollection.GetData(baseHandle).CbvHandle; break;
        case GFX_DESCRIPTOR_HANDLE_TYPE_SRV: descriptor = s_textureCollection.GetData(baseHandle).SrvHandle; break;
        case GFX_DESCRIPTOR_HANDLE_TYPE_UAV: LUZASSERT(false); break;
        case GFX_DESCRIPTOR_HANDLE_TYPE_DSV: descriptor = s_depthStencilCollection.GetData(baseHandle).SrvHandle; break;
        case GFX_DESCRIPTOR_HANDLE_TYPE_RTV: LUZASSERT(false); break;
        default: LUZASSERT(false); break;
        }

        ID3D12DescriptorHeap* pHeap = s_descriptorAllocatorCollection[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV].GetHeap(descriptor.Handle);

        CommandList& cl = s_commandListCollection.GetData(m_handle);
        cl.pGraphicsCommandList->SetDescriptorHeaps(1, &pHeap);
        cl.pGraphicsCommandList->SetGraphicsRootDescriptorTable(static_cast<UINT>(param), descriptor.GpuHandle);
    }

    void CommandStream::SetDescriptorTable(const u32 param, const GpuResourceHandle* pHandles, const u32 nHandles)
    {
        CommandList& cl = s_commandListCollection.GetData(m_handle);

        ID3D12DescriptorHeap* pDescriptorHeap = cl.pDescriptorHeap;
        D3D12_CPU_DESCRIPTOR_HANDLE srcHandle = pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        UINT incrementSize = s_descriptorAllocatorCollection[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV].m_descriptorHandleIncrementSize;

        for (u32 i = 0; i < nHandles; ++i)
        {
            auto handle = pHandles[i];
            auto eHandleType = static_cast<DescriptorHandleType>(HandleEncoder<GpuResourceHandle>::DecodeHandleValue(handle, s_nDescriptorTypeBits));
        
            D3D12_CPU_DESCRIPTOR_HANDLE dstHandle;
            switch (eHandleType)
            {
            case GFX_DESCRIPTOR_HANDLE_TYPE_CBV: dstHandle = s_constantBufferCollection.GetData(handle).CbvHandle.CpuHandle; break;
            case GFX_DESCRIPTOR_HANDLE_TYPE_SRV: dstHandle = s_textureCollection.GetData(handle).SrvHandle.CpuHandle; break;
            case GFX_DESCRIPTOR_HANDLE_TYPE_UAV: LUZASSERT(false); break;
            case GFX_DESCRIPTOR_HANDLE_TYPE_DSV: dstHandle = s_depthStencilCollection.GetData(handle).SrvHandle.CpuHandle; break;
            case GFX_DESCRIPTOR_HANDLE_TYPE_RTV: LUZASSERT(false); break;
            default: LUZASSERT(false); break;
            }

            s_device.pDevice->CopyDescriptorsSimple(1, CD3DX12_CPU_DESCRIPTOR_HANDLE(srcHandle, i, incrementSize), dstHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        }

        cl.pGraphicsCommandList->SetDescriptorHeaps(1, &pDescriptorHeap);
        cl.pGraphicsCommandList->SetGraphicsRootDescriptorTable(param, pDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
    }

    void CommandStream::SetDescriptorTable(const DescriptorTableRange* pDescriptorTableRanges, const u32 nDescriptorTableRanges)
    {
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        
        ID3D12DescriptorHeap* pDescriptorHeap = cl.pDescriptorHeap;
        cl.pGraphicsCommandList->SetDescriptorHeaps(1, &pDescriptorHeap);

        UINT incrementSize = s_descriptorAllocatorCollection[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV].m_descriptorHandleIncrementSize;
        UINT cpuOffset = 0;
        UINT gpuOffset = 0;

        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = pDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

        for (u32 i = 0; i < nDescriptorTableRanges; ++i)
        {
            const DescriptorTableRange& range = pDescriptorTableRanges[i];

            for (u32 j = 0; j < range.nHandles; ++j)
            {
                GpuResourceHandle handle = range.pHandles[j];
                DescriptorHandleType eHandleType = static_cast<DescriptorHandleType>(HandleEncoder<GpuResourceHandle>::DecodeHandleValue(handle, s_nDescriptorTypeBits));
            
                D3D12_CPU_DESCRIPTOR_HANDLE dstHandle;
                switch (eHandleType)
                {
                case GFX_DESCRIPTOR_HANDLE_TYPE_CBV: dstHandle = s_constantBufferCollection.GetData(handle).CbvHandle.CpuHandle; break;
                case GFX_DESCRIPTOR_HANDLE_TYPE_SRV: dstHandle = s_textureCollection.GetData(handle).SrvHandle.CpuHandle; break;
                case GFX_DESCRIPTOR_HANDLE_TYPE_UAV: LUZASSERT(false); break;
                case GFX_DESCRIPTOR_HANDLE_TYPE_DSV: dstHandle = s_depthStencilCollection.GetData(handle).SrvHandle.CpuHandle; break;
                case GFX_DESCRIPTOR_HANDLE_TYPE_RTV: LUZASSERT(false); break;
                default: LUZASSERT(false); break;
                }

                s_device.pDevice->CopyDescriptorsSimple(1, CD3DX12_CPU_DESCRIPTOR_HANDLE(cpuHandle, cpuOffset), dstHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            
                cpuOffset += incrementSize;
            }

            cl.pGraphicsCommandList->SetGraphicsRootDescriptorTable(range.Register, CD3DX12_GPU_DESCRIPTOR_HANDLE(gpuHandle, gpuOffset));

            gpuOffset = cpuOffset;
        }
    }

    void CommandStream::TransitionDepthStencil(const DepthStencilHandle handle, const ResourceStates before, const ResourceStates after)
    {
        DepthStencil& ds = s_depthStencilCollection.GetData(handle);
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        D3D12_RESOURCE_STATES b = GetD3D12ResourceState(before);
        D3D12_RESOURCE_STATES a = GetD3D12ResourceState(after);
        TransitionResource(cl.pGraphicsCommandList, ds.pResource, b, a);
    }

    void CommandStream::TransitionRenderTarget(const RenderTargetHandle handle, const ResourceStates before, const ResourceStates after)
    {
        RenderTarget& rt = s_renderTargetCollection.GetData(handle);
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        D3D12_RESOURCE_STATES b = GetD3D12ResourceState(before);
        D3D12_RESOURCE_STATES a = GetD3D12ResourceState(after);
        TransitionResource(cl.pGraphicsCommandList, rt.pResource, b, a);
    }

    void CommandStream::TransitionRenderTarget(const ResourceStates before, const ResourceStates after)
    {
        ID3D12Resource* pResource = nullptr;
        HRESULT hr = s_swapChain.pSwapChain3->GetBuffer(s_swapChain.FrameIndex, IID_PPV_ARGS(&pResource));
        LUZASSERT(SUCCEEDED(hr));

        //cl.pGraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pResource, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        D3D12_RESOURCE_STATES b = GetD3D12ResourceState(before);
        D3D12_RESOURCE_STATES a = GetD3D12ResourceState(after);
        TransitionResource(cl.pGraphicsCommandList, pResource, b, a);
    }

    void CommandStream::TransitionTexture(const TextureHandle handle, const ResourceStates before, const ResourceStates after)
    {
        Texture texture = s_textureCollection.GetData(handle);
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        D3D12_RESOURCE_STATES b = GetD3D12ResourceState(before);
        D3D12_RESOURCE_STATES a = GetD3D12ResourceState(after);
        TransitionResource(cl.pGraphicsCommandList, texture.pResource, b, a);
    }

    void CommandStream::DrawInstanceIndexed(const IndexBufferHandle handle, const u32 instanceCount /*= 1*/, const u32 startIndex /*= 0*/, const i32 baseVertexLocation /*= 0*/, const u32 startInstanceLocation /*= 0*/)
    {
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        IndexBuffer& ib = s_indexBufferCollection.GetData(handle);

        cl.pGraphicsCommandList->DrawIndexedInstanced(ib.NumIndices, instanceCount, startIndex, baseVertexLocation, startInstanceLocation);
    }
}
#endif
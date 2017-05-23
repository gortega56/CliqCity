#pragma once
#ifndef DX12_GPUSTATE_H
#define DX12_GPUSTATE_H

#ifndef __D3DX12_H__
#include "d3dx12.h"
#endif

namespace Dx12
{
    template<class DESC>
    class GpuState
    {
    public:
        GpuState() { ZeroMemory(&m_desc, sizeof(DESC)); }
        ~GpuState() {};

        DESC Desc() { return m_desc; }

    protected:
        DESC m_desc;
    };

    class RasterizerState : public GpuState<D3D12_RASTERIZER_DESC>
    {
    public:
        RasterizerState() { m_desc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); }
        ~RasterizerState() {}
    };

    class BlendState : public GpuState<D3D12_BLEND_DESC>
    {
    public:
        BlendState() { m_desc = CD3DX12_BLEND_DESC(D3D12_DEFAULT); }
        ~BlendState() {}
    };

    class DepthStencilState : public GpuState<D3D12_DEPTH_STENCIL_DESC>
    {
    public:
        DepthStencilState() { m_desc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); }
        ~DepthStencilState() {}
    };
}

#endif
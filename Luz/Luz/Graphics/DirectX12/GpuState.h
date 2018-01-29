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

    class LUZ_API RasterizerState : public GpuState<D3D12_RASTERIZER_DESC>
    {
    public:
        RasterizerState() { m_desc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); /*m_desc.FillMode = D3D12_FILL_MODE_WIREFRAME;*/ }
        ~RasterizerState() {}
    };

    class LUZ_API BlendState : public GpuState<D3D12_BLEND_DESC>
    {
    public:
        BlendState() { m_desc = CD3DX12_BLEND_DESC(D3D12_DEFAULT); }
        ~BlendState() {}
    };

    class LUZ_API DepthStencilState : public GpuState<D3D12_DEPTH_STENCIL_DESC>
    {
    public:
        DepthStencilState() { m_desc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); }
        ~DepthStencilState() {}
    };
}

#endif
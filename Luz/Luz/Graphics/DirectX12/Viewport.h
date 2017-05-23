#pragma once
#ifndef DX12_VIEWPORT_H
#define DX12_VIEWPORT_H

#ifndef __D3DX12_H__
#include "d3dx12.h"
#endif

namespace Dx12
{
    class Viewport
    {
    public:
        Viewport();
        ~Viewport();

        void SetViewportRect(float topLeftX, float topLeftY, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);
        void SetScissorRect(u32 topLeftX, u32 topLeftY, u32 width, u32 height);

        const D3D12_VIEWPORT& ViewportRect() const { return m_viewport; }
        const D3D12_RECT& ScissorRect() const { return m_scissorRect; }

    private:
        D3D12_VIEWPORT m_viewport;
        D3D12_RECT m_scissorRect;
    };
}

#endif

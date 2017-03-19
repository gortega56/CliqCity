#pragma once
#ifndef DX12VIEWPORT_H
#define DX12VIEWPORT_H

#ifndef DX12_INTERNAL_H
#include "dx12_internal.h"
#endif

namespace dx12
{
    class Viewport
    {
    public:
        Viewport();
        ~Viewport();

        void SetViewportRect(float topLeftX, float topLeftY, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);
        void SetScissorRect(u32 topLeftX, u32 topLeftY, u32 width, u32 height);

        D3D12_VIEWPORT ViewportRect() const { return m_viewport; }
        D3D12_RECT ScissorRect() const { return m_scissorRect; }

    private:
        D3D12_VIEWPORT m_viewport;
        D3D12_RECT m_scissorRect;
    };
}

#endif

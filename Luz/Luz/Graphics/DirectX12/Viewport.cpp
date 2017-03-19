#include "stdafx.h"
#include "Viewport.h"

using namespace dx12;

Viewport::Viewport()
{

}

Viewport::~Viewport()
{

}

void Viewport::SetViewportRect(float topLeftX, float topLeftY, float width, float height, float minDepth/* = 0.0f*/, float maxDepth/* = 1.0f*/)
{
    m_viewport.TopLeftX = topLeftX;
    m_viewport.TopLeftY = topLeftY;
    m_viewport.Width = width;
    m_viewport.Height = height;
    m_viewport.MinDepth = minDepth;
    m_viewport.MaxDepth = maxDepth;
}

void Viewport::SetScissorRect(u32 topLeftX, u32 topLeftY, u32 width, u32 height)
{
    m_scissorRect.left = topLeftX;
    m_scissorRect.top = topLeftY;
    m_scissorRect.right = width;
    m_scissorRect.bottom = height;
}
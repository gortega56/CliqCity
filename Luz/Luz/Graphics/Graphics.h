#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#ifndef GRAPHICSTYPES_H
#include "GraphicsTypes.h"
#endif

#define DX12

class Window;

namespace Graphics
{
    LUZ_API bool Initialize(Window* pWindow, u32 numBackBuffers);
    LUZ_API void Shutdown();

    LUZ_API VertexBufferHandle CreateVertexBuffer(const BufferDesc& desc);
    LUZ_API IndexBufferHandle CreateIndexBuffer(const BufferDesc& desc);
    LUZ_API ConstantBufferHandle CreateConstantBuffer(const BufferDesc& desc);
    LUZ_API TextureHandle CreateTexture(const TextureDesc& desc);
}

#endif

#pragma once
#ifndef DX12GRAPHICS_H
#define DX12GRAPHICS_H 

class Window;

namespace Dx12
{
    class SwapChainContext;

    bool Initialize(Window* pWindow, u32 numBackBuffers);
    void Shutdown();

    // TODO: Get ride of SwapChainContext
    SwapChainContext* SharedSwapChainContext();
}

#endif

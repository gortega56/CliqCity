#include "stdafx.h"
#include "Graphics.h"
#include "Window.h"

#if _WIN64 || _WIN32
#include "DirectX12\Dx12Graphics.h"
#elif __APPLE__
#endif

namespace Graphics
{
    bool Initialize(Window* pWindow, u32 numBackBuffers)
    {
#if _WIN64 || _WIN32
        return Dx12::Initialize(pWindow, numBackBuffers);
#elif __APPLE__
        return false;
#endif
    }

    void Shutdown()
    {
#if _WIN64 || _WIN32
        return Dx12::Shutdown();
#elif __APPLE__
        return false;
#endif
    }
}
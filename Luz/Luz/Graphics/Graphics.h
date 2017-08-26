#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

class Window;

namespace Graphics
{
    bool Initialize(Window* pWindow, u32 numBackBuffers);
    void Shutdown();
}

#if _WIN64 || _WIN32
#ifndef DX12_SHADER_H
#include "DirectX12\Shader.h"
#endif

typedef Dx12::Shader Shader;
typedef Dx12::InputLayout InputLayout;

#ifndef DX12_PIPELINESTATE_H
#include "DirectX12\PipelineState.h"
#endif

typedef Dx12::GraphicsPipeline GraphicsPipeline;

#ifndef DX12_ROOTSIGNATURE_H
#include "DirectX12\RootSignature.h"
#endif

typedef Dx12::RootSignature RootSignature;

#ifndef DX12_GPUSTATE_H
#include "DirectX12\GpuState.h"
#endif

typedef Dx12::RasterizerState RasterizerState;
typedef Dx12::DepthStencilState DepthStencilState;
typedef Dx12::BlendState BlendState;
#elif __APPLE__

#endif

#endif

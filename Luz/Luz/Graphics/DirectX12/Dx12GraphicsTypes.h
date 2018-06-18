#pragma once
#ifndef DX12GRAPHICSTYPES_H
#define DX12GRAPHICSTYPES_H

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include "d3dx12.h"

// DirectX12TK
#include "Audio.h"
#include "CommonStates.h"
#include "DDSTextureLoader.h"
#include "DescriptorHeap.h"
#include "DirectXHelpers.h"
#include "Effects.h"
#include "GamePad.h"
#include "GeometricPrimitive.h"
#include "GraphicsMemory.h"
#include "Keyboard.h"
#include "Model.h"
#include "Mouse.h"
#include "PrimitiveBatch.h"
#include "RenderTargetState.h"
#include "ResourceUploadBatch.h"
#include "ScreenGrab.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"
#include "WICTextureLoader.h"

#include "DirectXTex.h"
#include <wincodec.h>

#ifndef GRAPHICSTYPES_H
#include "GraphicsTypes.h"
#endif

#ifndef PIPELINESTATES_H
#include "PipelineStates.h"
#endif

namespace Graphics
{
    constexpr u8 s_maxSwapChainBuffers = 4;

    struct DescriptorHandle
    {
        D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle;
    };

    struct Device
    {
        IDXGIFactory* pFactory;
        IDXGIFactory1* pFactory1;
        IDXGIFactory2* pFactory2;
        IDXGIFactory3* pFactory3;
        IDXGIFactory4* pFactory4;

        ID3D12Device* pDevice;
        ID3D12Device1* pDevice1;
    };

    struct SwapChainContext
    {
        static constexpr u8 MaxBuffers = 4;

        IDXGISwapChain* pSwapChain;
        IDXGISwapChain1* pSwapChain1;
        IDXGISwapChain2* pSwapChain2;
        IDXGISwapChain3* pSwapChain3;
        ID3D12Resource* pDepthStencilResource;

        D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetViewHandles[MaxBuffers];
        D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilViewHandle;

        DXGI_SWAP_EFFECT SwapEffect;
        DXGI_FORMAT Format;
        DXGI_USAGE Usage;
        HWND Handle;

        u32 Width;
        u32 Height;
        u32 NumBuffers;
        u32 FrameIndex;

        bool FullScreen;
    };

    struct Shader
    {
        ID3DBlob* pShader = nullptr;
        D3D12_SHADER_BYTECODE ByteCode;
    };

    struct Pipeline
    {
        ID3D12RootSignature* pSignature = nullptr;
        ID3D12PipelineState* pPipelineState = nullptr;
    };

    struct RenderTargetView
    {
        D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle;
    };

    struct DepthStencil
    {
        ID3D12Resource* pResource;
        DescriptorHandle DepthStencilViewHandle;
        DescriptorHandle ShaderResourceViewHandle;
    };

    struct ConstantBuffer
    {
        ID3D12Resource* pResource;
        DescriptorHandle ConstantBufferViewHandle;
    };

    struct Texture
    {
        ID3D12Resource* pResource;
        DescriptorHandle ShaderResourceViewHandle;
        DescriptorHandle RenderTargetViewHandle;
    };

    DXGI_FORMAT GetDxgiFormat(const Format format);
    D3D12_INPUT_CLASSIFICATION GetD3D12InputClassification(const InputLayoutDesc::Element::Type elementType);
    D3D12_PRIMITIVE_TOPOLOGY_TYPE GetD3D12PrimitiveTopoglogyType(const PrimitiveTopologyType topology);

    D3D12_SHADER_VISIBILITY GetVisibility(const ShaderVisibility& visibility);
    D3D12_ROOT_DESCRIPTOR_FLAGS GetDescriptorFlags(const Parameter::DataFlags& dataFlags);
    D3D12_FILTER GetFilterType(const FilterType& filter);
    D3D12_TEXTURE_ADDRESS_MODE GetAddressType(const AddressType& address);
    D3D12_COMPARISON_FUNC GetComparisonType(const ComparisonType& comparison);
    D3D12_STATIC_BORDER_COLOR GetBorderColorType(const BorderColorType& borderColor);
    D3D12_ROOT_SIGNATURE_FLAGS GetSignatureFlags(const SignatureDesc::Flags& signatureFlags);

    D3D12_BLEND GetD3D12Blend(const BlendType blend);
    D3D12_BLEND_OP GetD3D12BlendOp(const BlendOpType op);
    D3D12_LOGIC_OP GetD3D12LogicOp(const LogicOpType op);
    D3D12_COLOR_WRITE_ENABLE GetD3D12ColorWriteEnable(const ColorWriteEnable colorWrite);

    D3D12_FILL_MODE GetD3D12FillMode(const FillMode fill);
    D3D12_CULL_MODE GetD3D12CullMode(const CullMode cull);
    D3D12_CONSERVATIVE_RASTERIZATION_MODE GetD3D12RasterizationMode(const ConservativeRasterizationMode mode);

    D3D12_DEPTH_WRITE_MASK GetD3D12DepthWriteMask(const DepthWriteMask mask);
    D3D12_STENCIL_OP GetD3D12StencilOp(const StencilOp op);
}

#endif
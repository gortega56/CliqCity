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

#ifndef RESOURCECOLLECTION_H
#include "ResourceCollection.h"
#endif

#ifndef DX12DESCRIPTORALLOCATOR_H
#include "Dx12DescriptorAllocator.h"
#endif

#define PIPELINE_MAX 32

#define SHADER_MAX 32

#define RENDER_TARGET_MAX 32

#define DEPTH_STENCIL_MAX 1024

#define VERTEX_BUFFER_MAX 1024

#define INDEX_BUFFER_MAX 1024

#define CONSTANT_BUFFER_MAX 1024

#define TEXTURE_MAX 1024

#define COMMAND_LIST_MAX 32

#define DESCRIPTOR_HEAP_MAX 1024

namespace Graphics
{
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

    struct SwapChain
    {

    };

    struct SwapChainContext
    {
        static constexpr u8 MaxBuffers = 4;

        IDXGISwapChain* pSwapChain;
        IDXGISwapChain1* pSwapChain1;
        IDXGISwapChain2* pSwapChain2;
        IDXGISwapChain3* pSwapChain3;
        
        ID3D12DescriptorHeap* pRenderTargetDescriptorHeap;
        ID3D12DescriptorHeap* pDepthStencilDescriptorHeap;
        
        ID3D12Resource* pDepthStencilResource;

        D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetViewHandles[MaxBuffers];
        D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilViewHandle;

        DXGI_SWAP_EFFECT SwapEffect;
        DXGI_FORMAT BufferFormat;
        DXGI_FORMAT RenderTargetFormat;
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

    struct RenderTarget
    {
        ID3D12Resource* pResource;
        D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle;
    };

    struct DepthStencil
    {
        ID3D12Resource* pResource;
        D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc;
        D3D12_DEPTH_STENCIL_VIEW_DESC DsvDesc;
        Descriptor DsvHandle;
        Descriptor SrvHandle;
    };

    struct VertexBuffer
    {
        ID3D12Resource* pResource;
        D3D12_VERTEX_BUFFER_VIEW View;
    };

    struct IndexBuffer
    {
        ID3D12Resource* pResource;
        D3D12_INDEX_BUFFER_VIEW View;
        u32 NumIndices;
    };

    struct ConstantBuffer
    {
        ID3D12Resource* pResource;
        D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualAddress;
        Descriptor CbvHandle;
    };

    struct Texture
    {
        ID3D12Resource* pResource;
        D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualAddress;
        Descriptor SrvHandle;
        Descriptor RtvHandle;
        Descriptor UavHandle;
    };

    struct CommandList
    {
        union
        {
            ID3D12CommandList* pCommandList;
            ID3D12GraphicsCommandList* pGraphicsCommandList;
        };

        ID3D12CommandQueue* pCommandQueue;
        void* pAlloc;
    };

    enum DescriptorHandleType
    {
        GFX_DESCRIPTOR_HANDLE_TYPE_CBV,
        GFX_DESCRIPTOR_HANDLE_TYPE_SRV,
        GFX_DESCRIPTOR_HANDLE_TYPE_UAV,
        GFX_DESCRIPTOR_HANDLE_TYPE_DSV,
        GFX_DESCRIPTOR_HANDLE_TYPE_RTV,
        GFX_DESCRIPTOR_HANDLE_TYPE_NUM_TYPES
    };

    extern Device s_device;

    extern SwapChainContext s_swapChain;

    extern ID3D12CommandQueue* s_pGraphicsQueue;

    extern ID3D12Debug* s_pDebug;

    extern ID3D12DebugDevice* s_pDebugDevice;

    typedef ResourceCollection<PipelineStateHandle, Pipeline, PIPELINE_MAX> PipelineCollection;

    typedef ResourceCollection<ShaderHandle, Shader, SHADER_MAX> ShaderCollection;

    typedef ResourceCollection<RenderTargetHandle, RenderTarget, RENDER_TARGET_MAX> RenderTargetCollection;

    typedef ResourceCollection<DepthStencilHandle, DepthStencil, DEPTH_STENCIL_MAX> DepthStencilCollection;

    typedef ResourceCollection<VertexBufferHandle, VertexBuffer, VERTEX_BUFFER_MAX> VertexBufferCollection;

    typedef ResourceCollection<IndexBufferHandle, IndexBuffer, INDEX_BUFFER_MAX> IndexBufferCollection;

    typedef ResourceCollection<ConstantBufferHandle, ConstantBuffer, CONSTANT_BUFFER_MAX> ConstantBufferCollection;

    typedef ResourceCollection<TextureHandle, Texture, TEXTURE_MAX> TextureCollection;

    typedef ResourceCollection<CommandStreamHandle, CommandList, COMMAND_LIST_MAX> CommandListCollection;

    extern ShaderCollection s_shaderCollection;

    extern PipelineCollection s_pipelineCollection;

    extern RenderTargetCollection s_renderTargetCollection;

    extern DepthStencilCollection s_depthStencilCollection;

    extern VertexBufferCollection s_vertexBufferCollection;

    extern IndexBufferCollection s_indexBufferCollection;

    extern ConstantBufferCollection s_constantBufferCollection;

    extern TextureCollection s_textureCollection;

    extern CommandListCollection s_commandListCollection;

    extern const uint32_t s_nDescriptorTypeBits;

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

    D3D12_PRIMITIVE_TOPOLOGY GetD3D12PrimitiveTogopology(const PrimitiveSubtopology topology);
}

#endif
#pragma once
#ifndef DX12_INTERNAL_H
#define DX12_INTERNAL_H

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

#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }

namespace Dx12
{
    struct ResourceParams
    {
        D3D12_HEAP_PROPERTIES HeapProperties;
        D3D12_HEAP_FLAGS HeapFlags;
        D3D12_RESOURCE_STATES InitialState;
    };

    static const ResourceParams DefaultParams = { CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON };
    static const ResourceParams UploadParams = { CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ };
    static const ResourceParams DestinationParams = { CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_COPY_DEST };
}

bool InitDXGIFactory(IDXGIFactory4** ppFactory);

bool InitD3D12Device(IDXGIFactory4* pFactory, ID3D12Device** ppDevice);

bool InitD3D12SwapChain(IDXGIFactory4* pFactory, ID3D12CommandQueue* pCommandQueue, IDXGISwapChain3** ppSwapChain, HWND hwnd, UINT width, UINT height, DXGI_FORMAT format, DXGI_USAGE usage, DXGI_SWAP_EFFECT swapEffect, UINT bufferCount, bool fullScreen);

bool CreateCommandQueue(ID3D12Device* pDevice, ID3D12CommandQueue** ppCommandQueue, D3D12_COMMAND_LIST_TYPE type, D3D12_COMMAND_QUEUE_FLAGS flags, INT priority);

bool CreateDescriptorHeap(ID3D12Device* pDevice, ID3D12DescriptorHeap** ppDescriptorHeap, LPCWSTR name, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, UINT descriptorCount);

bool GetSwapChainBuffers(IDXGISwapChain* pSwapChain, ID3D12Resource* pOutResources[], UINT resourceCount);

bool CreateSwapChainRenderTargetViews(ID3D12Device* pDevice, ID3D12DescriptorHeap* pDescriptorHeap, IDXGISwapChain* pSwapChain, ID3D12Resource* pResources[], UINT rtvCount);

void CreateRenderTargetViews(ID3D12Device* pDevice, ID3D12DescriptorHeap* pDescriptorHeap, ID3D12Resource* pResources[], UINT rtvCount);

void CreateDepthStencilViews(ID3D12Device* pDevice, ID3D12DescriptorHeap* pDescriptorHeap, ID3D12Resource* pResources[], UINT dsvCount);

bool CreateDepthStencilResource(
    ID3D12Device* pDevice,
    ID3D12Resource** ppResource,
    UINT64 width,
    UINT height,
    UINT16 arraySize = 1,
    UINT16 mipLevels = 0,
    UINT sampleCount = 1,
    UINT sampleQuality = 0,
    UINT64 alignment = 0,
    FLOAT clearDepth = 1.0f,
    UINT8 clearStencil = 0);

bool CreateResource(ID3D12Device* pDevice, D3D12_RESOURCE_DESC* pDesc, ID3D12Resource** ppResource, Dx12::ResourceParams const* pParams);

bool CreateBuffer(ID3D12Device* pDevice, ID3D12Resource** ppResource, Dx12::ResourceParams const* pParams, UINT64 width, UINT64 alignment = 0);
bool CreateUploadBuffer(ID3D12Device* pDevice, ID3D12Resource** ppResource, UINT64 width, UINT64 alignment = 0);
bool CreateDestinationBuffer(ID3D12Device* pDevice, ID3D12Resource** ppResource, UINT64 width, UINT64 alignment = 0);

bool CreateTexture2D(ID3D12Device* pDevice, ID3D12Resource** ppResource, Dx12::ResourceParams const* pParams, UINT64 width, UINT height, UINT16 mipLevels, DXGI_FORMAT format, UINT sampleCount = 1, UINT sampleQuality = 0, D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_UNKNOWN);
bool CreateUploadTexture2D(ID3D12Device* pDevice, ID3D12Resource** ppResource, UINT64 width, UINT height, UINT16 mipLevels, DXGI_FORMAT format, UINT sampleCount = 1, UINT sampleQuality = 0, D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_UNKNOWN);
bool CreateDestinationTexture2D(ID3D12Device* pDevice, ID3D12Resource** ppResource, UINT64 width, UINT height, UINT16 mipLevels, DXGI_FORMAT format, UINT sampleCount = 1, UINT sampleQuality = 0, D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_UNKNOWN);

bool CreateCommandAllocators(ID3D12Device* pDevice, ID3D12CommandAllocator* pCommandAllocators[], UINT count, D3D12_COMMAND_LIST_TYPE type);

bool CreateGraphicsCommandAllocators(ID3D12Device* pDevice, ID3D12CommandAllocator* pCommandAllocators[], UINT count);

bool CreateGraphicsCommandList(ID3D12Device* pDevice, ID3D12CommandAllocator* pCommandAllocator, ID3D12PipelineState* pPipeLineState, ID3D12GraphicsCommandList** ppCommandList);

inline D3D12_SUBRESOURCE_DATA SubResourceData(void* data, LONG_PTR rowPitch, LONG_PTR slicePitch)
{
    D3D12_SUBRESOURCE_DATA resourceData = {};
    resourceData.pData = reinterpret_cast<BYTE*>(data);
    resourceData.RowPitch = rowPitch;
    resourceData.SlicePitch = slicePitch;
    return resourceData;
}

void UpdateBufferResource(
    ID3D12GraphicsCommandList* pCommandList,
    ID3D12Resource* pBufferResource, 
    ID3D12Resource* pUploadResource, 
    void* data, 
    LONG_PTR rowPitch, 
    LONG_PTR slicePitch, 
    UINT64 uploadOffset = 0,
    UINT firstResource = 0,
    UINT numResources = 1);

void UploadVertexOrContantBufferResource(ID3D12GraphicsCommandList* pCommandList,
    ID3D12Resource* pBufferResource,
    ID3D12Resource* pUploadResource,
    void* data,
    LONG_PTR bufferSize);

void UploadIndexBufferResource(ID3D12GraphicsCommandList* pCommandList,
    ID3D12Resource* pBufferResource,
    ID3D12Resource* pUploadResource,
    void* data,
    LONG_PTR bufferSize);

void TransitionResource(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* pResource, D3D12_RESOURCE_STATES initialState, D3D12_RESOURCE_STATES finalState);

bool CreateFences(ID3D12Device* pDevice, ID3D12Fence* pFences[], UINT64 pFenceValues[], D3D12_FENCE_FLAGS flags, UINT count);

bool WaitForPreviousFrame(IDXGISwapChain3* pSwapChain, ID3D12Fence* pFences[], UINT64 pFenceValues[], HANDLE fenceEvent, UINT* pFrameIndex);
bool WaitForFence(ID3D12Fence* pFence, UINT64* pFenceValue, HANDLE fenceEvent);

bool ExecuteCommandLists(ID3D12CommandQueue* pCommandQueue, ID3D12Fence* pFence, UINT64* pFenceValue, ID3D12CommandList* ppCommandLists[], UINT count);
bool ExecuteCommandList(ID3D12CommandQueue* pCommandQueue, ID3D12Fence* pFence, UINT64* pFenceValue, ID3D12GraphicsCommandList* pCommandList);

DXGI_FORMAT IndexFormat(unsigned size);

int GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat);
DXGI_FORMAT GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID);
WICPixelFormatGUID GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID);

#endif // !DX12_INTERNAL_H

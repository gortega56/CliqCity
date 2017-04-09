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

#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }

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

bool CreateDestinationBufferResource(ID3D12Device* pDevice, ID3D12Resource** ppResource, UINT64 width, UINT64 alignment = 0);

bool CreateUploadBufferResource(ID3D12Device* pDevice, ID3D12Resource** ppResource, UINT64 width, UINT64 alignment = 0);

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

#endif // !DX12_INTERNAL_H

#include "stdafx.h"
#include "dx12_internal.h"

bool InitDXGIFactory(IDXGIFactory4** ppFactory)
{
    HRESULT hr;

    hr = CreateDXGIFactory1(IID_PPV_ARGS(ppFactory));
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

bool InitD3D12Device(IDXGIFactory4* pFactory, ID3D12Device** ppDevice)
{
    IDXGIAdapter1* pAdapter;

    bool adapterFound = false;
    int adapterIdx = 0;

    while (pFactory->EnumAdapters1(adapterIdx, &pAdapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC1 desc;
        pAdapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            ++adapterIdx;
            continue;
        }

        HRESULT hr = D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(ppDevice));
        if (SUCCEEDED(hr))
        {
            adapterFound = true;
            break;
        }
    }

    if (!adapterFound)
    {
        return false;
    }

    return true;
}

bool CreateCommandQueue(ID3D12Device* pDevice, ID3D12CommandQueue** ppCommandQueue, D3D12_COMMAND_LIST_TYPE type, D3D12_COMMAND_QUEUE_FLAGS flags, INT priority)
{
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
    ZeroMemory(&commandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
    commandQueueDesc.Type = type;
    commandQueueDesc.Priority = priority;
    commandQueueDesc.Flags = flags;

    HRESULT hr = pDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(ppCommandQueue));
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

bool InitD3D12SwapChain(IDXGIFactory4* pFactory, ID3D12CommandQueue* pCommandQueue, IDXGISwapChain3** ppSwapChain, HWND hwnd, UINT width, UINT height, DXGI_FORMAT format, DXGI_USAGE usage, DXGI_SWAP_EFFECT swapEffect, UINT bufferCount, bool fullScreen)
{
    DXGI_MODE_DESC backBufferDesc;
    ZeroMemory(&backBufferDesc, sizeof(DXGI_MODE_DESC));
    backBufferDesc.Width = width;
    backBufferDesc.Height = height;
    backBufferDesc.Format = format;

    DXGI_SAMPLE_DESC sampleDesc;
    ZeroMemory(&sampleDesc, sizeof(DXGI_SAMPLE_DESC));
    sampleDesc.Count = 1;

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
    swapChainDesc.BufferCount = bufferCount;
    swapChainDesc.BufferDesc = backBufferDesc;
    swapChainDesc.BufferUsage = usage;
    swapChainDesc.SwapEffect = swapEffect;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.SampleDesc = sampleDesc;
    swapChainDesc.Windowed = !fullScreen;

    IDXGISwapChain* pTempSwapChain;
    HRESULT hr = pFactory->CreateSwapChain(pCommandQueue, &swapChainDesc, &pTempSwapChain);
    if (FAILED(hr))
    {
        return false;
    }

    *ppSwapChain = static_cast<IDXGISwapChain3*>(pTempSwapChain);

    return true;
}

bool CreateDescriptorHeap(ID3D12Device* pDevice, ID3D12DescriptorHeap** ppDescriptorHeap, LPCWSTR name, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, UINT descriptorCount)
{
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
    ZeroMemory(&heapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
    heapDesc.Type = type;
    heapDesc.Flags = flags;
    heapDesc.NumDescriptors = descriptorCount;

    HRESULT hr = pDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(ppDescriptorHeap));
    if (FAILED(hr))
    {
        return false;
    }

    (*ppDescriptorHeap)->SetName(name);

    return true;
}

bool GetSwapChainBuffers(IDXGISwapChain* pSwapChain, ID3D12Resource* pOutResources[], UINT resourceCount)
{
    for (UINT i = 0; i < resourceCount; ++i)
    {
        HRESULT hr = pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pOutResources[i]));
        if (FAILED(hr))
        {
            return false;
        }
    }

    return true;
}

bool CreateSwapChainRenderTargetViews(ID3D12Device* pDevice, ID3D12DescriptorHeap* pDescriptorHeap, IDXGISwapChain* pSwapChain, ID3D12Resource* pResources[], UINT rtvCount)
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(pDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    UINT rtvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    if (!GetSwapChainBuffers(pSwapChain, pResources, rtvCount))
    {
        return false;
    }

    for (UINT i = 0; i < rtvCount; ++i)
    {
        /*HRESULT hr = pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pResources[i]));
        if (FAILED(hr))
        {
            return false;
        }*/

        pDevice->CreateRenderTargetView(pResources[i], nullptr, rtvHandle);

        rtvHandle.Offset(1, rtvDescriptorSize);
    }

    return true;
}

void CreateRenderTargetViews(ID3D12Device* pDevice, ID3D12DescriptorHeap* pDescriptorHeap, ID3D12Resource* pResources[], UINT rtvCount)
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(pDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    UINT rtvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    for (UINT i = 0; i < rtvCount; ++i)
    {
        pDevice->CreateRenderTargetView(pResources[i], nullptr, rtvHandle);

        rtvHandle.Offset(1, rtvDescriptorSize);
    }
}

bool CreateDepthStencilResource(
    ID3D12Device* pDevice,
    ID3D12Resource** ppResource,
    UINT64 width,
    UINT height,
    UINT16 arraySize    /*= 1*/,
    UINT16 mipLevels    /*= 0*/,
    UINT sampleCount    /*= 1*/,
    UINT sampleQuality  /*= 0*/,
    UINT64 alignment    /*= 0*/,
    FLOAT clearDepth    /*= 1.0f*/,
    UINT8 clearStencil  /*= 0*/)
{
    D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
    depthOptimizedClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthOptimizedClearValue.DepthStencil.Depth = clearDepth;
    depthOptimizedClearValue.DepthStencil.Stencil = clearStencil;

    HRESULT hr = pDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D24_UNORM_S8_UINT, width, height, arraySize, mipLevels, sampleCount, sampleQuality, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &depthOptimizedClearValue,
        IID_PPV_ARGS(ppResource));

    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

void CreateDepthStencilViews(ID3D12Device* pDevice, ID3D12DescriptorHeap* pDescriptorHeap, ID3D12Resource* pResources[], UINT dsvCount)
{
    D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(pDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    int dsvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

    for (UINT i = 0; i < dsvCount; ++i)
    {
        pDevice->CreateDepthStencilView(pResources[i], &depthStencilDesc, dsvHandle);
        dsvHandle.Offset(dsvDescriptorSize);
    }
}

bool CreateResource(ID3D12Device* pDevice, D3D12_RESOURCE_DESC* pDesc, ID3D12Resource** ppResource, Dx12::ResourceParams const* pParams)
{
    HRESULT hr = pDevice->CreateCommittedResource(&pParams->HeapProperties, pParams->HeapFlags, pDesc, pParams->InitialState, nullptr, IID_PPV_ARGS(ppResource));
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

bool CreateBuffer(ID3D12Device* pDevice, ID3D12Resource** ppResource, const Dx12::ResourceParams* pParams, UINT64 width, UINT64 alignment /*= 0*/)
{
    return CreateResource(pDevice, &CD3DX12_RESOURCE_DESC::Buffer(width, D3D12_RESOURCE_FLAG_NONE, alignment), ppResource, pParams);
}

bool CreateUploadBuffer(ID3D12Device* pDevice, ID3D12Resource** ppResource, UINT64 width, UINT64 alignment /*= 0*/)
{
    return CreateBuffer(pDevice, ppResource, &Dx12::UploadParams, width, alignment);
}

bool CreateDestinationBuffer(ID3D12Device* pDevice, ID3D12Resource** ppResource, UINT64 width, UINT64 alignment /*= 0*/)
{
    return CreateBuffer(pDevice, ppResource, &Dx12::DestinationParams, width, alignment);
}

bool CreateTexture2D(ID3D12Device* pDevice, ID3D12Resource** ppResource, const Dx12::ResourceParams* pParams, UINT64 width, UINT height, UINT16 mipLevels, DXGI_FORMAT format, UINT sampleCount /*= 1*/, UINT sampleQuality /*= 0*/, D3D12_TEXTURE_LAYOUT layout /*= D3D12_TEXTURE_LAYOUT_UNKNOWN*/)
{
    return CreateResource(pDevice, &CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, 1, mipLevels, sampleCount, sampleQuality, D3D12_RESOURCE_FLAG_NONE, layout), ppResource, pParams);
}

bool CreateUploadTexture2D(ID3D12Device* pDevice, ID3D12Resource** ppResource, UINT64 width, UINT height, UINT16 mipLevels, DXGI_FORMAT format, UINT sampleCount /*= 1*/, UINT sampleQuality /*= 0*/, D3D12_TEXTURE_LAYOUT layout /*= D3D12_TEXTURE_LAYOUT_UNKNOWN*/)
{
    UINT64 textureUploadBufferSize;
    // this function gets the size an upload buffer needs to be to upload a texture to the gpu.
    // each row must be 256 byte aligned except for the last row, which can just be the size in bytes of the row
    // eg. textureUploadBufferSize = ((((width * numBytesPerPixel) + 255) & ~255) * (height - 1)) + (width * numBytesPerPixel);
    //textureUploadBufferSize = (((imageBytesPerRow + 255) & ~255) * (textureDesc.Height - 1)) + imageBytesPerRow;
    pDevice->GetCopyableFootprints(&CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, 1, mipLevels, sampleCount, sampleQuality, D3D12_RESOURCE_FLAG_NONE, layout), 0, 1, 0, nullptr, nullptr, nullptr, &textureUploadBufferSize);

    return CreateUploadBuffer(pDevice, ppResource, textureUploadBufferSize);
}

bool CreateDestinationTexture2D(ID3D12Device* pDevice, ID3D12Resource** ppResource, UINT64 width, UINT height, UINT16 mipLevels, DXGI_FORMAT format, UINT sampleCount /*= 1*/, UINT sampleQuality /*= 0*/, D3D12_TEXTURE_LAYOUT layout /*= D3D12_TEXTURE_LAYOUT_UNKNOWN*/)
{
    return CreateTexture2D(pDevice, ppResource, &Dx12::DestinationParams, width, height, mipLevels, format, sampleCount, sampleQuality, layout);
}

bool CreateGraphicsCommandAllocators(ID3D12Device* pDevice, ID3D12CommandAllocator* pCommandAllocators[], UINT count)
{
    for (UINT i = 0; i < count; ++i)
    {
        HRESULT hr = pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCommandAllocators[i]));
        if (FAILED(hr))
        {
            return false;
        }
    }

    return true;
}

bool CreateGraphicsCommandList(ID3D12Device* pDevice, ID3D12CommandAllocator* pCommandAllocator, ID3D12PipelineState* pPipeLineState, ID3D12GraphicsCommandList** ppCommandList)
{
    HRESULT hr = pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandAllocator, pPipeLineState, IID_PPV_ARGS(ppCommandList));
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

void UpdateBufferResource(
    ID3D12GraphicsCommandList* pCommandList,
    ID3D12Resource* pBufferResource,
    ID3D12Resource* pUploadResource,
    void* data,
    LONG_PTR rowPitch,
    LONG_PTR slicePitch,
    UINT64 uploadOffset /*= 0*/,
    UINT firstResource /*= 0*/,
    UINT numResources /*= 1*/)
{
    D3D12_SUBRESOURCE_DATA resourceData = SubResourceData(data, rowPitch, slicePitch);
    UpdateSubresources(pCommandList, pBufferResource, pUploadResource, uploadOffset, firstResource, numResources, &resourceData);
}

void UploadVertexOrContantBufferResource(ID3D12GraphicsCommandList* pCommandList,
    ID3D12Resource* pBufferResource,
    ID3D12Resource* pUploadResource,
    void* data,
    LONG_PTR bufferSize)
{
    UpdateBufferResource(pCommandList, pBufferResource, pUploadResource, data, bufferSize, bufferSize);
    TransitionResource(pCommandList, pBufferResource, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
}

void UploadIndexBufferResource(ID3D12GraphicsCommandList* pCommandList,
    ID3D12Resource* pBufferResource,
    ID3D12Resource* pUploadResource,
    void* data,
    LONG_PTR bufferSize)
{
    UpdateBufferResource(pCommandList, pBufferResource, pUploadResource, data, bufferSize, bufferSize);
    TransitionResource(pCommandList, pBufferResource, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_INDEX_BUFFER);
}

void TransitionResource(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* pResource, D3D12_RESOURCE_STATES initialState, D3D12_RESOURCE_STATES finalState)
{
    pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pResource, initialState, finalState));
}

bool CreateFences(ID3D12Device* pDevice, ID3D12Fence* pFences[], UINT64 pFenceValues[], D3D12_FENCE_FLAGS flags, UINT count)
{
    for (UINT i = 0; i < count; ++i)
    {
        HRESULT hr = pDevice->CreateFence(0, flags, IID_PPV_ARGS(&pFences[i]));
        if (FAILED(hr))
        {
            return false;
        }        
        
        pFenceValues[i] = 0;
    }

    return true;
}

bool WaitForFence(ID3D12Fence* pFence, UINT64* pFenceValue, HANDLE fenceEvent)
{
    bool running = true;

    UINT64 fenceValue = *pFenceValue;

    // if the current fence value is still less than "fenceValue", then we know the GPU has not finished executing
    // the command queue since it has not reached the "commandQueue->Signal(fence, fenceValue)" command
    UINT64 completedValue = pFence->GetCompletedValue();
    if (completedValue < fenceValue)
    {
        HRESULT hr = pFence->SetEventOnCompletion(fenceValue, fenceEvent);
        if (FAILED(hr))
        {
            running = false;
        }

        // We will wait until the fence has triggered the event that it's current value has reached "fenceValue". once it's value
        // has reached "fenceValue", we know the command queue has finished executing
        WaitForSingleObject(fenceEvent, INFINITE);
    }

    return running;
}

bool WaitForPreviousFrame(IDXGISwapChain3* pSwapChain, ID3D12Fence* pFences[], UINT64 pFenceValues[], HANDLE fenceEvent, UINT* pFrameIndex)
{
    UINT frameIndex = *pFrameIndex;

    bool running = WaitForFence(pFences[frameIndex], &pFenceValues[frameIndex], fenceEvent);

    *pFrameIndex = pSwapChain->GetCurrentBackBufferIndex();

    return running;
    //UINT idx = *frameIndex;
    //
    //// if the current fence value is still less than "fenceValue", then we know the GPU has not finished executing
    //// the command queue since it has not reached the "commandQueue->Signal(fence, fenceValue)" command
    //if (pFences[idx]->GetCompletedValue() < pFenceValues[idx])
    //{
    //    HRESULT hr = pFences[idx]->SetEventOnCompletion(pFenceValues[idx], fenceEvent);
    //    if (FAILED(hr))
    //    {
    //        *pRunning = false;
    //    }

    //    // We will wait until the fence has triggered the event that it's current value has reached "fenceValue". once it's value
    //    // has reached "fenceValue", we know the command queue has finished executing
    //    WaitForSingleObject(fenceEvent, INFINITE);
    //}

    //// increment fenceValue for next frame
    //pFenceValues[idx]++;

    //// swap the current rtv buffer index so we draw on the correct buffer
    //*frameIndex = pSwapChain->GetCurrentBackBufferIndex();
}

bool ExecuteCommandLists(ID3D12CommandQueue* pCommandQueue, ID3D12Fence* pFence, UINT64* pFenceValue, ID3D12CommandList* ppCommandLists[], UINT count)
{
    // TO DO: these may need to be closed

    // Execute command list
    pCommandQueue->ExecuteCommandLists(count, ppCommandLists);

    // Increment fence value so we don't start drawing before verts are uploaded
    *pFenceValue++;

    HRESULT hr = pCommandQueue->Signal(pFence, *pFenceValue);
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

bool ExecuteCommandList(ID3D12CommandQueue* pCommandQueue, ID3D12Fence* pFence, UINT64* pFenceValue, ID3D12GraphicsCommandList* pCommandList)
{
    pCommandList->Close();

    ID3D12CommandList* ppCommandList[] = { pCommandList };
    return ExecuteCommandLists(pCommandQueue, pFence, pFenceValue, ppCommandList, 1);
}

DXGI_FORMAT IndexFormat(unsigned size)
{
    switch (size)
    {
    case 1: return DXGI_FORMAT_R8_UINT;
    case 2: return DXGI_FORMAT_R16_UINT;
    case 4: return DXGI_FORMAT_R32_UINT;
    default:
        assert(false);
    }

    return DXGI_FORMAT_UNKNOWN;
}

// get the number of bits per pixel for a dxgi format
int GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat)
{
    if (dxgiFormat == DXGI_FORMAT_R32G32B32A32_FLOAT) return 128;
    else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_FLOAT) return 64;
    else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_UNORM) return 64;
    else if (dxgiFormat == DXGI_FORMAT_R8G8B8A8_UNORM) return 32;
    else if (dxgiFormat == DXGI_FORMAT_B8G8R8A8_UNORM) return 32;
    else if (dxgiFormat == DXGI_FORMAT_B8G8R8X8_UNORM) return 32;
    else if (dxgiFormat == DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM) return 32;

    else if (dxgiFormat == DXGI_FORMAT_R10G10B10A2_UNORM) return 32;
    else if (dxgiFormat == DXGI_FORMAT_B5G5R5A1_UNORM) return 16;
    else if (dxgiFormat == DXGI_FORMAT_B5G6R5_UNORM) return 16;
    else if (dxgiFormat == DXGI_FORMAT_R32_FLOAT) return 32;
    else if (dxgiFormat == DXGI_FORMAT_R16_FLOAT) return 16;
    else if (dxgiFormat == DXGI_FORMAT_R16_UNORM) return 16;
    else if (dxgiFormat == DXGI_FORMAT_R8_UNORM) return 8;
    else if (dxgiFormat == DXGI_FORMAT_A8_UNORM) return 8;

    return -1;
}
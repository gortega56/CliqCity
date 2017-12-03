#include "stdafx.h"
#include "Dx12Graphics.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "Device.h"
#include "CommandContext.h"
#include "RenderContext.h"
#include "MSPlatformWindow.h"
#include "DescriptorHeap.h"

using namespace Dx12;

static SwapChainContext* g_swapChainContext = nullptr;

static ID3D12Debug* g_debug = nullptr;
static ID3D12DebugDevice* g_debugDevice = nullptr;
static void EnableDebugLayer();
static void ConfigureDebugLayer();

bool Dx12::Initialize(Window* pWindow, u32 numBackBuffers)
{
#ifdef _DEBUG
    EnableDebugLayer();
#endif

    auto pPlatformWindow = dynamic_cast<MS::PlatformWindow*>(pWindow);
    auto handle = pPlatformWindow->Handle();
    auto width = pPlatformWindow->Width();
    auto height = pPlatformWindow->Height();
    auto fullscreen = pPlatformWindow->FullScreen();

    auto pDevice = Device::SharedInstance();

    auto pMainQueue = CommandQueue::CreateGraphicsQueue();
    if (!pMainQueue) return false;

    DescriptorHeapAllocator::Initialize();

    // TODO: SwapChainContext will keep this alive, but would like to find a cleaner way to keep shared data around
    auto pSwapChain = 
        std::make_shared<SwapChain>(DXGI_FORMAT_R8G8B8A8_UNORM, 
        DXGI_USAGE_RENDER_TARGET_OUTPUT, 
        DXGI_SWAP_EFFECT_FLIP_DISCARD);

    if (!pSwapChain->Initialize(pDevice.get(), 
        pMainQueue.get(), 
        handle, 
        numBackBuffers, 
        width, 
        height, 
        fullscreen))
    {
        return false;
    }

    // TODO: Need to keep this
    g_swapChainContext = new SwapChainContext();
    if (!g_swapChainContext->Initialize(pSwapChain, pMainQueue))
    {
        return false;
    }

    float color[4] = { 0.44f, 0.86f, 0.91f, 1.0f };
    g_swapChainContext->SetColor(color);

    CommandContext::Initialize();
    
    return true;
}

void Dx12::Shutdown()
{
#ifdef _DEBUG
    auto pDevice = Device::SharedInstance();
    pDevice->DX()->QueryInterface(IID_PPV_ARGS(&g_debugDevice));
    if (g_debugDevice)
    {
        g_debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
    }

    SAFE_RELEASE(g_debug);
    SAFE_RELEASE(g_debugDevice);
#endif

    // wait for the gpu to finish all frames
    CommandAllocatorPool::Destroy();
    delete g_swapChainContext;
    DescriptorHeapAllocator::Destroy();
}

SwapChainContext* Dx12::SharedSwapChainContext()
{
    return g_swapChainContext;
}

void EnableDebugLayer()
{
    HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&g_debug));
    if (SUCCEEDED(hr))
    {
        g_debug->EnableDebugLayer();
    }
}

void ConfigureDebugLayer()
{
        ID3D12InfoQueue* pInfoQueue = nullptr;
        if (SUCCEEDED(Device::SharedInstance()->DX()->QueryInterface(IID_PPV_ARGS(&pInfoQueue))))
        {
            // Suppress whole categories of messages
            //D3D12_MESSAGE_CATEGORY Categories[] = {};
    
            // Suppress messages based on their severity level
            D3D12_MESSAGE_SEVERITY Severities[] =
            {
                D3D12_MESSAGE_SEVERITY_CORRUPTION ,
                D3D12_MESSAGE_SEVERITY_ERROR,
                D3D12_MESSAGE_SEVERITY_WARNING,
                D3D12_MESSAGE_SEVERITY_INFO ,
                D3D12_MESSAGE_SEVERITY_MESSAGE
            };
    
            // Suppress individual messages by their ID
            D3D12_MESSAGE_ID DenyIds[] =
            {
                // This occurs when there are uninitialized descriptors in a descriptor table, even when a
                // shader does not access the missing descriptors.  I find this is common when switching
                // shader permutations and not wanting to change much code to reorder resources.
                D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,
    
                // Triggered when a shader does not export all color components of a render target, such as
                // when only writing RGB to an R10G10B10A2 buffer, ignoring alpha.
                D3D12_MESSAGE_ID_CREATEGRAPHICSPIPELINESTATE_PS_OUTPUT_RT_OUTPUT_MISMATCH,
    
                // This occurs when a descriptor table is unbound even when a shader does not access the missing
                // descriptors.  This is common with a root signature shared between disparate shaders that
                // don't all need the same types of resources.
                D3D12_MESSAGE_ID_COMMAND_LIST_DESCRIPTOR_TABLE_NOT_SET,
    
                // RESOURCE_BARRIER_DUPLICATE_SUBRESOURCE_TRANSITIONS
                (D3D12_MESSAGE_ID)1008,
            };
    
            D3D12_INFO_QUEUE_FILTER NewFilter = {};
            //NewFilter.DenyList.NumCategories = _countof(Categories);
            //NewFilter.DenyList.pCategoryList = Categories;
            NewFilter.DenyList.NumSeverities = _countof(Severities);
            NewFilter.DenyList.pSeverityList = Severities;
            NewFilter.DenyList.NumIDs = 0;// _countof(DenyIds);
            NewFilter.DenyList.pIDList = nullptr;// DenyIds;
    
            pInfoQueue->PushStorageFilter(&NewFilter);
            pInfoQueue->Release();
        }
}
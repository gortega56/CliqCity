#include "stdafx.h"
#include "Graphics.h"
#include "CommandStream.h"

#ifdef DX12
#include "GraphicsTypes_Dx12.h"

#include "ResourceCollection.h"

#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

#define DX_DEBUG

#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }

#define CHECK_DEVICE_REMOVED(hr)                        \
if (FAILED(hr))                                         \
{                                                       \
    hr = s_device.pDevice->GetDeviceRemovedReason();    \
    LUZASSERT(false);                                   \
}

void ErrorDescription(HRESULT hr)
{
    if (FACILITY_WINDOWS == HRESULT_FACILITY(hr))
        hr = HRESULT_CODE(hr);
    CHAR* szErrMsg;

    if (FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&szErrMsg, 0, NULL) != 0)
    {
        printf("%s", szErrMsg);
        LocalFree(szErrMsg);
    }
    else
        printf("[Could not find a description for error # %#x.]\n", hr);
}

namespace Internal
{
    class ShaderInclude : public ID3DInclude
    {
    public:
        HRESULT Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFilename, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes) override;
        
        HRESULT Close(LPCVOID pData) override;

        std::string m_filename;
    };

    HRESULT ShaderInclude::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFilename, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
    {
        static const std::string s_directory = LUZ_SHADER_DIRECTORY;
        std::string filename = s_directory + pFilename;

        std::ifstream ifs(filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

        if (!ifs.is_open())
        {
            return E_FAIL;
        }
        
        long long fileSize = ifs.tellg();
        char* buffer = new char[fileSize];
        ifs.seekg(0, std::ios::beg);
        ifs.read(buffer, fileSize);
        ifs.close();
        *ppData = buffer;
        *pBytes = static_cast<UINT>(fileSize);

        return S_OK;
    }

    HRESULT ShaderInclude::Close(LPCVOID pData)
    {
        char* buffer = (char*)pData;
        delete[] buffer;
        return S_OK;
    }
    
    static void ConfigureDebugLayer(ID3D12Device* pDevice);

    static bool CompileShader(const char* filename, const char* entryPoint, const char* target, ID3DBlob** ppCode);

    static bool LoadWICImage(std::wstring filename, DirectX::ScratchImage& outImage, bool genMipsIfNecessary = false);
    
    static bool LoadDDSImage(std::wstring filename, DirectX::ScratchImage& outImage, bool genMipsIfNecessary = false);
    
    static bool LoadTgaImage(std::wstring filename, DirectX::ScratchImage& outImage, bool genMipsIfNecessary = false);
    
    static HRESULT GenerateMips(DirectX::ScratchImage& inImage, DirectX::ScratchImage& outImage);

    void ConfigureDebugLayer(ID3D12Device* pDevice)
    {
        ID3D12InfoQueue* pInfoQueue = nullptr;
        if (SUCCEEDED(pDevice->QueryInterface(IID_PPV_ARGS(&pInfoQueue))))
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
            NewFilter.DenyList.NumSeverities = 5;
            NewFilter.DenyList.pSeverityList = Severities;
            NewFilter.DenyList.NumIDs = 0;// _countof(DenyIds);
            NewFilter.DenyList.pIDList = nullptr;// DenyIds;

            pInfoQueue->PushStorageFilter(&NewFilter);
            pInfoQueue->Release();
        }
    }

    bool CompileShader(const char* filename, const char* entryPoint, const char* target, ID3DBlob** ppCode)
    {
        ID3DBlob* pError = nullptr;
        UINT flags1 = 0;

#if _DEBUG
        flags1 |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        flags1 |= D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES;

        wchar_t pWide[MAX_PATH];
        MultiByteToWideChar(CP_UTF8, 0, filename, -1, pWide, static_cast<int>(strlen(filename) + 1));

        ShaderInclude include;
        HRESULT hr = D3DCompileFromFile(pWide, nullptr, &include, entryPoint, target, flags1, 0, ppCode, &pError);
        if (FAILED(hr))
        {
            if (pError)
            {
                LuzAssert_Msg(0, (char*)pError->GetBufferPointer());

            }
            else
            {
                LUZASSERT(0);
            }

            return false;
        }

        SAFE_RELEASE(pError);

        return true;
    }

    bool LoadWICImage(std::wstring filename, DirectX::ScratchImage& outImage, bool genMipsIfNecessary /*= false*/)
    {
        DirectX::TexMetadata texMetaData;
        DirectX::ScratchImage scratchImage;
        HRESULT hr = DirectX::LoadFromWICFile(filename.c_str(), DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &texMetaData, scratchImage);
        if (FAILED(hr))
        {
            return false;
        }

        hr = DirectX::GenerateMipMaps(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), (DWORD)DirectX::TEX_FILTER_FLAGS::TEX_FILTER_DEFAULT, 0, outImage);
        if (FAILED(hr))
        {
            return false;
        }

        return true;
    }

    bool LoadDDSImage(std::wstring filename, DirectX::ScratchImage& outImage, bool genMipsIfNecessary /*= false*/)
    {
        DirectX::TexMetadata texMetaData;
        DirectX::ScratchImage scratchImage;
        HRESULT hr = DirectX::LoadFromDDSFile(filename.c_str(), DirectX::DDS_FLAGS_NONE, &texMetaData, scratchImage);
        if (FAILED(hr))
        {
            return false;
        }

        size_t mipLevels = texMetaData.mipLevels;
        if (mipLevels == 1 && genMipsIfNecessary)
        {
            hr = GenerateMips(scratchImage, outImage);
            if (FAILED(hr))
            {
                return false;
            }
        }
        else
        {
            outImage = std::move(scratchImage);
        }

        return true;
    }

    bool LoadTgaImage(std::wstring filename, DirectX::ScratchImage& outImage, bool genMipsIfNecessary /*= false*/)
    {
        DirectX::TexMetadata texMetaData;
        DirectX::ScratchImage scratchImage;
        HRESULT hr = DirectX::LoadFromTGAFile(filename.c_str(), &texMetaData, scratchImage);
        if (FAILED(hr))
        {
            return false;
        }

        size_t mipLevels = texMetaData.mipLevels;
        if (mipLevels == 1 && genMipsIfNecessary)
        {
            hr = GenerateMips(scratchImage, outImage);
            if (FAILED(hr))
            {
                return false;
            }
        }
        else
        {
            outImage = std::move(scratchImage);
        }

        return true;
    }

    HRESULT GenerateMips(DirectX::ScratchImage& inImage, DirectX::ScratchImage& outImage)
    {
        return DirectX::GenerateMipMaps(inImage.GetImages(), inImage.GetImageCount(), inImage.GetMetadata(), (DWORD)DirectX::TEX_FILTER_FLAGS::TEX_FILTER_DEFAULT, 0, outImage);
    }
}

namespace Graphics
{
    Device s_device;
    
    SwapChainContext s_swapChain;

    CommandQueue s_commandQueues[GFX_COMMAND_QUEUE_TYPE_NUM_TYPES];

    CommandContextPool s_commandContextPools[GFX_COMMAND_QUEUE_TYPE_NUM_TYPES];

    ShaderCollection s_shaderCollection;
    
    PipelineCollection s_pipelineCollection;

    RenderTargetCollection s_renderTargetCollection;

    DepthStencilCollection s_depthStencilCollection;

    VertexBufferCollection s_vertexBufferCollection;
    
    IndexBufferCollection s_indexBufferCollection;
    
    ConstantBufferCollection s_constantBufferCollection;
    
    TextureCollection s_textureCollection;

    CommandListCollection s_commandListCollection;

    static const uint32_t s_nDescriptorTypeBits = 3;

    static const uint32_t s_nCommandQueueTypeBits = 2;

    using HandleCoder = ppg::integer_coder<GpuResourceHandle>;

	static void WaitOnFence(ID3D12Fence* pFence, UINT64 signal)
	{
		// if the current fence value is still less than "fenceValue", then we know the GPU has not finished executing
		// the command queue since it has not reached the "commandQueue->Signal(fence, fenceValue)" command
		UINT64 fenceValue = pFence->GetCompletedValue();

		if (fenceValue < signal)
		{
			HANDLE eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			LUZASSERT(eventHandle);

			HRESULT hr = pFence->SetEventOnCompletion(signal, eventHandle);
			if (SUCCEEDED(hr))
			{
				// We will wait until the fence has triggered the event that it's current value has reached "fenceValue". once it's value
				// has reached "fenceValue", we know the command queue has finished executing
				WaitForSingleObject(eventHandle, INFINITE);
			}

			BOOL bSuccess = CloseHandle(eventHandle);
            LUZASSERT(bSuccess);
		}
	}

	static std::atomic_uint pAllocatorRefCounts[CommandContextPool::Capacity];

	static CommandContext AllocateCommandContext(const CommandQueueType eQueueType)
    {
		// Any thread can come and grab a context.. we don't care
		// if we grab a context that is currently in flight on the gpu
		// because we know it will finish. We only really care if we
		// grab a context that is in use by another cpu thread. So we
		// check via bit vector for availability. 1 means available.

        CommandContextPool& pool = s_commandContextPools[eQueueType];
        CommandQueue& commandQueue = s_commandQueues[eQueueType];
        
		uint64_t usage = pool.Usage.load();
		uint64_t firstBitSet;

		do
		{
			// Find and unset right most bit
			firstBitSet = usage & (~usage + 1);
			if (firstBitSet == 0ULL)
			{
				// if nothing is available then
				// change our expected value to 
				// prompt a reload on the cas op
				usage = ~usage;
			}
		} while (!pool.Usage.compare_exchange_strong(usage, usage & (usage - 1)));

		uint64_t index = static_cast<uint64_t>(log(firstBitSet) / log(2));
		LUZASSERT(static_cast<uint16_t>(index) < CommandContextPool::Capacity);

		auto rc = pAllocatorRefCounts[index].fetch_add(1) + 1;
		LUZASSERT(rc == 1);

		uint64_t lastExecution = pool.pCommandContextExecutions[index];
        uint64_t completed = commandQueue.pFence->GetCompletedValue();

        bool wait = !(lastExecution < completed || (lastExecution == 0 && completed == 0));
        if (wait)
        {
            // We may not actually wait here which is fine
            WaitOnFence(commandQueue.pFence, lastExecution);
        }

        HRESULT hr = pool.ppCommandAllocators[index]->Reset();
        LUZASSERT(SUCCEEDED(hr));

        CommandContext ctx;
        ctx.pCommandAllocator = pool.ppCommandAllocators[index];
        ctx.pDescriptorHeap = pool.ppDescriptorHeaps[index];
		ctx.Index = static_cast<uint32_t>(index);
		ctx.eQueueType = eQueueType;
        return ctx;
    }

	static void FreeCommandContext(const uint64_t iContext, const uint64_t execution, const CommandQueueType eQueueType)
	{
		CommandContextPool& pool = s_commandContextPools[eQueueType];
		pool.pCommandContextExecutions[iContext] = execution;

		uint64_t usage = pool.Usage.load();
		uint64_t updated = 1ULL << iContext;
		while (!pool.Usage.compare_exchange_strong(usage, usage | updated));

		auto rc = pAllocatorRefCounts[iContext].fetch_sub(1) - 1;
		LUZASSERT(rc == 0);
	}

	//static uint64_t ExecuteCommandList(ID3D12GraphicsCommandList* pGraphicsCommandList, const CommandQueueType& eQueueType)
    //{
    //    HRESULT hr = pGraphicsCommandList->Close();
    //    LUZASSERT(SUCCEEDED(hr));
	//
    //    // TODO: Use main queue?
    //    CommandQueue& commandQueue = s_commandQueues[eQueueType];
    //    ID3D12CommandQueue* pCommandQueue = commandQueue.pCommandQueue;
	//
    //    ID3D12CommandList* ppCommandLists[] = { pGraphicsCommandList };
    //    pCommandQueue->ExecuteCommandLists(1, ppCommandLists);
	//
    //    return commandQueue.Executions.fetch_add(1ULL) + 1ULL;
    //}

	static uint64_t ExecuteCommandList(ID3D12GraphicsCommandList* pGraphicsCommandList, const CommandQueueType eQueueType)
	{
		HRESULT hr = pGraphicsCommandList->Close();
		LUZASSERT(SUCCEEDED(hr));

		// TODO: Use main queue?
		CommandQueue& commandQueue = s_commandQueues[eQueueType];
		ID3D12CommandQueue* pCommandQueue = commandQueue.pCommandQueue;

		ID3D12CommandList* ppCommandLists[] = { pGraphicsCommandList };
		pCommandQueue->ExecuteCommandLists(1, ppCommandLists);

		return commandQueue.Executions.fetch_add(1ULL) + 1ULL;
}

	static void SignalQueue(const CommandQueueType& eQueueType, uint64_t signal, bool wait)
    { 
        CommandQueue& commandQueue = s_commandQueues[eQueueType];
        HRESULT hr = commandQueue.pCommandQueue->Signal(commandQueue.pFence, signal);
        LUZASSERT(SUCCEEDED(hr));

        if (wait)
        {
            WaitOnFence(commandQueue.pFence, signal);
        }
    }

	static Descriptor AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE eType, const u32 nDescriptors)
	{
		return s_descriptorAllocatorCollection[eType].Allocate(nDescriptors);
	}

	static ShaderHandle CreateShader(const char* filename, const char* entryPoint, const char* target)
    {
        wchar_t pWide[MAX_PATH];
        MultiByteToWideChar(CP_UTF8, 0, filename, -1, pWide, static_cast<int>(strlen(filename) + 1));
            
        LPCWSTR extension = PathFindExtensionW(pWide);

        if (_wcsicmp(extension, L".cso") == 0)
        {
            auto handle = s_shaderCollection.AllocateHandle();
            if (handle != GPU_RESOURCE_HANDLE_INVALID)
            {
                Shader& shader = s_shaderCollection.GetData(handle);
                HRESULT hr = D3DReadFileToBlob(pWide, &shader.pShader);
                LUZASSERT(SUCCEEDED(hr));

                if (shader.pShader)
                {
                    shader.ByteCode.BytecodeLength = shader.pShader->GetBufferSize();
                    shader.ByteCode.pShaderBytecode = shader.pShader->GetBufferPointer();
                }
            }

            return handle;
        }
        else if (_wcsicmp(extension, L".hlsl") == 0)
        {
            auto handle = s_shaderCollection.AllocateHandle();
            if (handle != GPU_RESOURCE_HANDLE_INVALID)
            {
                Shader& shader = s_shaderCollection.GetData(handle);
                Internal::CompileShader(filename, entryPoint, target, &shader.pShader);
                if (shader.pShader)
                {
                    shader.ByteCode.BytecodeLength = shader.pShader->GetBufferSize();
                    shader.ByteCode.pShaderBytecode = shader.pShader->GetBufferPointer();
                }
            }

            return handle;
        }
        else
        {
            LUZASSERT(0);
            return GPU_RESOURCE_HANDLE_INVALID;
        }
    }

    bool Initialize(const Platform::WindowHandle hWindow, u32 numBackBuffers)
    {
#ifdef DX_DEBUG
        LUZASSERT(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&s_device.pDebug))));
        s_device.pDebug->EnableDebugLayer();
#endif

        auto pPlatformWindow = &Platform::s_pWindows[hWindow];
        auto handle = pPlatformWindow->hwnd;

        RECT rect;
        bool success = ::GetClientRect(handle, &rect);
        LUZASSERT(success);

		auto width = rect.right - rect.left;
        auto height = rect.bottom - rect.top;
        auto fullScreen = pPlatformWindow->bFullScreen;

        // Create Factory
        HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&s_device.pFactory4));
        if (FAILED(hr)) return false;

        // Create device
        bool adapterFound = false;
        int adapterIdx = 0;

        while (s_device.pFactory4->EnumAdapters1(adapterIdx, &s_device.pAdapter1) != DXGI_ERROR_NOT_FOUND)
        {
            DXGI_ADAPTER_DESC1 desc;
            s_device.pAdapter1->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                ++adapterIdx;
                continue;
            }

            hr = D3D12CreateDevice(s_device.pAdapter1, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&s_device.pDevice));
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

        // Create Command Queues
        D3D12_COMMAND_LIST_TYPE pCommandListTypes[GFX_COMMAND_QUEUE_TYPE_NUM_TYPES] =
        {
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            D3D12_COMMAND_LIST_TYPE_COPY,
            D3D12_COMMAND_LIST_TYPE_COMPUTE
        };

        for (uint32_t i = 0; i < GFX_COMMAND_QUEUE_TYPE_NUM_TYPES; ++i)
        {
            D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
            ZeroMemory(&commandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
            commandQueueDesc.Type = pCommandListTypes[i];
            commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
            commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

            s_device.pDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&s_commandQueues[i].pCommandQueue));
            if (FAILED(hr)) return false;

            hr = s_device.pDevice->CreateFence(0ULL, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&s_commandQueues[i].pFence));
            if (FAILED(hr)) return false;
        }

        // Initialize Descriptor heaps
        DescriptorAllocator* pHeapAllocator = nullptr;
        pHeapAllocator = &s_descriptorAllocatorCollection[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV];
        pHeapAllocator->m_type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        pHeapAllocator->m_flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;// D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        pHeapAllocator->m_descriptorHandleIncrementSize = s_device.pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        pHeapAllocator = &s_descriptorAllocatorCollection[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER];
        pHeapAllocator->m_type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        pHeapAllocator->m_flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;// D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        pHeapAllocator->m_descriptorHandleIncrementSize = s_device.pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

        pHeapAllocator = &s_descriptorAllocatorCollection[D3D12_DESCRIPTOR_HEAP_TYPE_RTV];
        pHeapAllocator->m_type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        pHeapAllocator->m_flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        pHeapAllocator->m_descriptorHandleIncrementSize = s_device.pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        pHeapAllocator = &s_descriptorAllocatorCollection[D3D12_DESCRIPTOR_HEAP_TYPE_DSV];
        pHeapAllocator->m_type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        pHeapAllocator->m_flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        pHeapAllocator->m_descriptorHandleIncrementSize = s_device.pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

        // Create swap chain
        s_swapChain.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        s_swapChain.BufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        s_swapChain.RenderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        s_swapChain.Usage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        s_swapChain.Width = width;
        s_swapChain.Height = height;
        s_swapChain.Handle = handle;
        s_swapChain.NumBuffers = (std::min)(numBackBuffers, s_swapChain.MaxBuffers);
        s_swapChain.FrameIndex = 0;
        s_swapChain.Frames = 0;
        s_swapChain.FullScreen = fullScreen;

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
        swapChainDesc.BufferCount = numBackBuffers;
        swapChainDesc.Width = width;
        swapChainDesc.Height = height;
        swapChainDesc.Format = s_swapChain.BufferFormat;
        swapChainDesc.BufferUsage = s_swapChain.Usage;
        swapChainDesc.SwapEffect = s_swapChain.SwapEffect;
        swapChainDesc.SampleDesc.Count = 1;

        IDXGISwapChain1* pTempSwapChain;
        hr = s_device.pFactory4->CreateSwapChainForHwnd(s_commandQueues[GFX_COMMAND_QUEUE_TYPE_DRAW].pCommandQueue, handle, &swapChainDesc, nullptr, nullptr, &pTempSwapChain);
        if (FAILED(hr))
        {   
            ErrorDescription(hr);
            return false;
        }

        s_swapChain.pSwapChain3 = static_cast<IDXGISwapChain3*>(pTempSwapChain);
        s_swapChain.FrameIndex = s_swapChain.pSwapChain3->GetCurrentBackBufferIndex();
        LUZASSERT(s_swapChain.FrameIndex == 0);

        D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
        ZeroMemory(&heapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        heapDesc.NumDescriptors = numBackBuffers;

        hr = s_device.pDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&s_swapChain.pRenderTargetDescriptorHeap));
        if (FAILED(hr))
        {
            return false;
        }

        // Create Render Target Views for back buffers
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = s_swapChain.pRenderTargetDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        UINT descriptorIncrementSize = GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        for (u32 i = 0; i < s_swapChain.NumBuffers; ++i)
        {
            s_swapChain.RenderTargetViewHandles[i] = 
                CD3DX12_CPU_DESCRIPTOR_HANDLE(cpuHandle, static_cast<INT>(i), descriptorIncrementSize);

            ID3D12Resource* pResource = nullptr;
            s_swapChain.pSwapChain3->GetBuffer(i, IID_PPV_ARGS(&pResource));
            
            s_device.pDevice->CreateRenderTargetView(pResource, nullptr, s_swapChain.RenderTargetViewHandles[i]);
        }

        // Create Depth Stencil View
        D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
        depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
        depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
        depthOptimizedClearValue.DepthStencil.Stencil = 0;

        hr = s_device.pDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthOptimizedClearValue,
            IID_PPV_ARGS(&s_swapChain.pDepthStencilResource));
        if (FAILED(hr)) return false;

        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        heapDesc.NumDescriptors = 1;
        hr = s_device.pDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&s_swapChain.pDepthStencilDescriptorHeap));
        if (FAILED(hr))
        {
            return false;
        }

        s_swapChain.DepthStencilViewHandle = s_swapChain.pDepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        s_device.pDevice->CreateDepthStencilView(s_swapChain.pDepthStencilResource, nullptr, s_swapChain.DepthStencilViewHandle);

        // Initialize CommandContextPool
        D3D12_DESCRIPTOR_HEAP_DESC desc;
        ZeroMemory(&desc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        desc.NumDescriptors = CommandContextPool::DescriptorHeapCapacity;

        for (unsigned int i = 0; i < GFX_COMMAND_QUEUE_TYPE_NUM_TYPES; ++i)
        {
            CommandContextPool& pool = s_commandContextPools[i];

            for (unsigned int j = 0; j < CommandContextPool::Capacity; ++j)
            {
                pool.pCommandContextExecutions[j] = 0;

                HRESULT hr = s_device.pDevice->CreateCommandAllocator(pCommandListTypes[i], IID_PPV_ARGS(&pool.ppCommandAllocators[j]));
                LUZASSERT(SUCCEEDED(hr));

                hr = s_device.pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&pool.ppDescriptorHeaps[j]));
                LUZASSERT(SUCCEEDED(hr));
            }
        }

        return true;
    }

    void Shutdown()
    {
        Flush();

        for (u32 i = 0; i < PIPELINE_MAX; ++i)
        {
            SAFE_RELEASE(s_pipelineCollection.GetData(static_cast<PipelineStateHandle>(i)).pPipelineState);
            SAFE_RELEASE(s_pipelineCollection.GetData(static_cast<PipelineStateHandle>(i)).pSignature);
        }

        for (u32 i = 0; i < SHADER_MAX; ++i)
        {
            SAFE_RELEASE(s_shaderCollection.GetData(static_cast<ShaderHandle>(i)).pShader);
        }

        for (u32 i = 0; i < RENDER_TARGET_MAX; ++i)
        {
            SAFE_RELEASE(s_renderTargetCollection.GetData(static_cast<RenderTargetHandle>(i)).pResource);
        }

        for (u32 i = 0; i < DEPTH_STENCIL_MAX; ++i)
        {
            SAFE_RELEASE(s_depthStencilCollection.GetData(static_cast<DepthStencilHandle>(i)).pResource);
        }

        for (u32 i = 0; i < VERTEX_BUFFER_MAX; ++i)
        {
            SAFE_RELEASE(s_vertexBufferCollection.GetData(static_cast<VertexBufferHandle>(i)).pResource);
        }

        for (u32 i = 0; i < INDEX_BUFFER_MAX; ++i)
        {
            SAFE_RELEASE(s_indexBufferCollection.GetData(static_cast<IndexBufferHandle>(i)).pResource);
        }

        for (u32 i = 0; i < CONSTANT_BUFFER_MAX; ++i)
        {
            SAFE_RELEASE(s_constantBufferCollection.GetData(static_cast<ConstantBufferHandle>(i)).pResource);
        }

        for (u32 i = 0; i < TEXTURE_MAX; ++i)
        {
            SAFE_RELEASE(s_textureCollection.GetData(static_cast<TextureHandle>(i)).pResource);
        }

        for (u32 i = 0; i < COMMAND_LIST_MAX; ++i)
        {
            ID3D12GraphicsCommandList* pGraphicsCommandList = s_commandListCollection.GetData(static_cast<CommandStreamHandle>(i)).pGraphicsCommandList;
            if (pGraphicsCommandList)pGraphicsCommandList->ClearState(0);
            SAFE_RELEASE(s_commandListCollection.GetData(static_cast<CommandStreamHandle>(i)).pGraphicsCommandList);
        }

        for (u32 i = 0; i < GFX_COMMAND_QUEUE_TYPE_NUM_TYPES; ++i)
        {
            for (u32 j = 0; j < CommandContextPool::Capacity; ++j)
            {
                s_commandContextPools[i].ppCommandAllocators[j]->Reset();
                SAFE_RELEASE(s_commandContextPools[i].ppCommandAllocators[j]);
                SAFE_RELEASE(s_commandContextPools[i].ppDescriptorHeaps[j]);
            }
        }

        for (u32 i = 0; i < s_swapChain.NumBuffers; ++i)
        {
            ID3D12Resource* pResource = nullptr;
            HRESULT hr = s_swapChain.pSwapChain3->GetBuffer(i, IID_PPV_ARGS(&pResource));
            LUZASSERT(SUCCEEDED(hr));
            SAFE_RELEASE(pResource);
        }

        SAFE_RELEASE(s_swapChain.pGraphicsCommandList);
        SAFE_RELEASE(s_swapChain.pDepthStencilResource);
        SAFE_RELEASE(s_swapChain.pDepthStencilDescriptorHeap);
        SAFE_RELEASE(s_swapChain.pRenderTargetDescriptorHeap);
        SAFE_RELEASE(s_swapChain.pSwapChain);
        SAFE_RELEASE(s_swapChain.pSwapChain1);
        SAFE_RELEASE(s_swapChain.pSwapChain2);
        SAFE_RELEASE(s_swapChain.pSwapChain3);

        for (u32 i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
        {
            for (auto& heap : s_descriptorAllocatorCollection[i].m_descriptorHeaps)
            {
                SAFE_RELEASE(heap.pHeap);
            }
            s_descriptorAllocatorCollection[i].m_descriptorHeaps.clear();
        }

        for (u32 i = 0; i < GFX_COMMAND_QUEUE_TYPE_NUM_TYPES; ++i)
        {
            SAFE_RELEASE(s_commandQueues[i].pCommandQueue);
            SAFE_RELEASE(s_commandQueues[i].pFence);
        }

        SAFE_RELEASE(s_device.pAdapter);
        SAFE_RELEASE(s_device.pAdapter1);
        SAFE_RELEASE(s_device.pAdapter2);
        SAFE_RELEASE(s_device.pAdapter3);

        SAFE_RELEASE(s_device.pFactory);
        SAFE_RELEASE(s_device.pFactory1);
        SAFE_RELEASE(s_device.pFactory2);
        SAFE_RELEASE(s_device.pFactory3);
        SAFE_RELEASE(s_device.pFactory4);
        
#ifdef DX_DEBUG
        HRESULT hr = s_device.pDevice->QueryInterface(IID_PPV_ARGS(&s_device.pDebugDevice1));
        LUZASSERT(SUCCEEDED(hr));
        s_device.pDebugDevice1->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);

        SAFE_RELEASE(s_device.pDebugDevice1);
        SAFE_RELEASE(s_device.pDebugDevice);
        SAFE_RELEASE(s_device.pDebug);
#endif

        SAFE_RELEASE(s_device.pDevice);
        SAFE_RELEASE(s_device.pDevice1);
    }

    ShaderHandle CreateVertexShader(const char* filename) { return CreateShader(filename, "main", "vs_5_1"); }
    
    ShaderHandle CreateHullShader(const char* filename) { return CreateShader(filename, "main", "hs_5_1"); }
    
    ShaderHandle CreateDomainShader(const char* filename) { return CreateShader(filename, "main", "ds_5_1"); }
    
    ShaderHandle CreateGeometryShader(const char* filename) { return CreateShader(filename, "main", "gs_5_1"); }
    
    ShaderHandle CreatePixelShader(const char* filename) { return CreateShader(filename, "main", "ps_5_1"); }
    
    PipelineStateHandle CreateGraphicsPipelineState(const PipelineDesc& desc)
    {
        PipelineStateHandle handle = s_pipelineCollection.AllocateHandle();
        if (handle != GPU_RESOURCE_HANDLE_INVALID)
        {
            Pipeline& pipeline = s_pipelineCollection.GetData(handle);

            D3D12_GRAPHICS_PIPELINE_STATE_DESC pso;
            ZeroMemory(&pso, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

            // Signature

            std::vector<std::vector<CD3DX12_DESCRIPTOR_RANGE1>> paramRanges;    // Just here to keep ranges in scope until finalizing the root signature
            std::vector<CD3DX12_ROOT_PARAMETER1> params;

            auto& signature = desc.Signature;
            u32 numParams = signature.NumParams();
            params.reserve((size_t)numParams);

            for (u32 i = 0; i < numParams; ++i)
            {
                params.emplace_back();
                auto& param = signature.GetParam(i);
                auto paramType = param.GetType();

                switch (paramType)
                {
                case Parameter::Type::PARAMETER_TYPE_CONSTANTS:
                    params.back().InitAsConstants(param.GetConstantNum32BitValues(), param.GetConstantRegister(), param.GetConstantRegisterSpace(), GetVisibility(param.GetVisibility()));
                    break;
                case Parameter::Type::PARAMETER_TYPE_CONSTANT_VIEW:
                    params.back().InitAsConstantBufferView(param.GetDescriptorRegister(), param.GetDescriptorRegisterSpace(), GetDescriptorFlags(param.GetDataFlags()), GetVisibility(param.GetVisibility()));
                    break;
                case Parameter::Type::PARAMETER_TYPE_SHADER_VIEW:
                    params.back().InitAsShaderResourceView(param.GetDescriptorRegister(), param.GetDescriptorRegisterSpace(), GetDescriptorFlags(param.GetDataFlags()), GetVisibility(param.GetVisibility()));
                    break;
                case Parameter::Type::PARAMETER_TYPE_COMPUTE_VIEW:
                    params.back().InitAsUnorderedAccessView(param.GetDescriptorRegister(), param.GetDescriptorRegisterSpace(), GetDescriptorFlags(param.GetDataFlags()), GetVisibility(param.GetVisibility()));
                    break;
                case Parameter::Type::PARAMETER_TYPE_DESCRIPTOR_TABLE:
                {
                    paramRanges.emplace_back();
                    auto& ranges = paramRanges.back();

                    u32 numRanges = param.NumDescriptorTableRanges();
                    ranges.reserve((size_t)numRanges);

                    for (u32 j = 0; j < numRanges; ++j)
                    {
                        auto& range = param.GetDescriptorTableRange(j);

                        UINT numDescriptors = static_cast<UINT>(range.NumDescriptors);
                        UINT baseRegister = static_cast<UINT>(range.BaseRegister);
                        UINT registerSpace = static_cast<UINT>(range.RegisterSpace);
                        D3D12_DESCRIPTOR_RANGE_FLAGS flags =
                            (range.NumDescriptors == -1)
                            ? D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE
                            : D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
                        
                        switch (range.Kind)
                        {
                        case DescriptorTable::Range::Type::DESCRIPTOR_TABLE_RANGE_TYPE_CONSTANT_VIEW:
                            ranges.emplace_back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, numDescriptors, baseRegister, registerSpace, flags, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
                            break;
                        case DescriptorTable::Range::Type::DESCRIPTOR_TABLE_RANGE_TYPE_SHADER_VIEW:
                            ranges.emplace_back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, numDescriptors, baseRegister, registerSpace, flags, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
                            break;
                        case DescriptorTable::Range::Type::DESCRIPTOR_TABLE_RANGE_TYPE_COMPUTE_VIEW:
                            ranges.emplace_back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, numDescriptors, baseRegister, registerSpace, flags, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
                            break;
                        case DescriptorTable::Range::Type::DESCRIPTOR_TABLE_RANGE_TYPE_SAMPLER:
                            ranges.emplace_back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, numDescriptors, baseRegister, registerSpace, flags, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
                            break;
                        }
                    }

                    params.back().InitAsDescriptorTable((UINT)ranges.size(), ranges.data(), GetVisibility(param.GetVisibility()));
                    break;
                }
                }
            }

            u32 numSamplers = signature.NumSamplers();

            std::vector<CD3DX12_STATIC_SAMPLER_DESC> samplers;
            samplers.reserve((u32)numSamplers);

            for (u32 i = 0; i < numSamplers; ++i)
            {
                samplers.emplace_back();
                auto& sampler = signature.GetSampler(i);

                samplers
                    .back()
                    .Init(sampler.Register
                        , GetFilterType(sampler.Filter)
                        , GetAddressType(sampler.AddressU)
                        , GetAddressType(sampler.AddressV)
                        , GetAddressType(sampler.AddressW)
                        , (FLOAT)sampler.MipLodBias
                        , (UINT)sampler.MaxAnisotropy
                        , GetComparisonType(sampler.Comparison)
                        , GetBorderColorType(sampler.BorderColor)
                        , (FLOAT)sampler.MinLod
                        , (FLOAT)sampler.MaxLod
                        , GetVisibility(sampler.Visibility));
            }

            CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rsd;
            rsd.Init_1_1((UINT)params.size(), params.data(), (UINT)samplers.size(), samplers.data(), GetSignatureFlags(signature.GetFlags()));

            ID3DBlob* pBlob;
            ID3DBlob* pErrorBlob;
            HRESULT hr = D3D12SerializeVersionedRootSignature(&rsd, &pBlob, &pErrorBlob);
            if (FAILED(hr))
            {
                OutputDebugStringA((LPCSTR)pErrorBlob->GetBufferPointer());
                SAFE_RELEASE(pErrorBlob);
            }

            hr = s_device.pDevice->CreateRootSignature(0, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_PPV_ARGS(&pipeline.pSignature));
            LUZASSERT(SUCCEEDED(hr));

            pso.pRootSignature = pipeline.pSignature;

            const char* pSignatureName = desc.Signature.GetName();
            wchar_t pWide[MAX_PATH];
            MultiByteToWideChar(CP_UTF8, 0, pSignatureName, -1, pWide, static_cast<int>(strlen(pSignatureName) + 1));
            pso.pRootSignature->SetName(pWide);

            // Shaders
            LUZASSERT(desc.VertexShaderHandle != GPU_RESOURCE_HANDLE_INVALID);
            pso.VS = s_shaderCollection.GetData(desc.VertexShaderHandle).ByteCode;
            if (desc.HullShaderHandle != GPU_RESOURCE_HANDLE_INVALID) pso.HS = s_shaderCollection.GetData(desc.HullShaderHandle).ByteCode;
            if (desc.DomainShaderHandle != GPU_RESOURCE_HANDLE_INVALID) pso.DS = s_shaderCollection.GetData(desc.DomainShaderHandle).ByteCode;
            if (desc.GeometryShaderHandle != GPU_RESOURCE_HANDLE_INVALID) pso.GS = s_shaderCollection.GetData(desc.GeometryShaderHandle).ByteCode;
            if (desc.PixelShaderHandle != GPU_RESOURCE_HANDLE_INVALID) pso.PS = s_shaderCollection.GetData(desc.PixelShaderHandle).ByteCode;

            // BlendState

            pso.BlendState.AlphaToCoverageEnable = desc.Blend.AlphaToCoverageEnable;
            pso.BlendState.IndependentBlendEnable = desc.Blend.IndependentBlendEnable;

            for (u32 i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
            {
                auto& rt = pso.BlendState.RenderTarget[i];
                auto& bs = desc.Blend.BlendStates[i];
                rt.BlendEnable = bs.BlendEnable;
                rt.LogicOpEnable = bs.LogicOpEnable;
                rt.SrcBlend = GetD3D12Blend(bs.SrcBlend);
                rt.DestBlend = GetD3D12Blend(bs.DestBlend);
                rt.BlendOp = GetD3D12BlendOp(bs.BlendOp);
                rt.SrcBlendAlpha = GetD3D12Blend(bs.SrcBlendAlpha);
                rt.DestBlendAlpha = GetD3D12Blend(bs.DestBlendAlpha);
                rt.BlendOpAlpha = GetD3D12BlendOp(bs.BlendOpAlpha);
                rt.LogicOp = GetD3D12LogicOp(bs.LogicOp);
                rt.RenderTargetWriteMask = GetD3D12ColorWriteEnable(bs.RenderTargetWriteMask);
            }

            // Sample Mask
            pso.SampleMask = desc.SampleMask;

            // RasterizerState
            pso.RasterizerState.FillMode = GetD3D12FillMode(desc.Rasterizer.Fill);
            pso.RasterizerState.CullMode = GetD3D12CullMode(desc.Rasterizer.Cull);
            pso.RasterizerState.FrontCounterClockwise = desc.Rasterizer.FrontCounterClockwise;
            pso.RasterizerState.DepthBias = desc.Rasterizer.DepthBias;
            pso.RasterizerState.DepthBiasClamp = desc.Rasterizer.DepthBiasClamp;
            pso.RasterizerState.SlopeScaledDepthBias = desc.Rasterizer.SlopeScaledDepthBias;
            pso.RasterizerState.DepthClipEnable = desc.Rasterizer.DepthClipEnable;
            pso.RasterizerState.MultisampleEnable = desc.Rasterizer.MsaaEnable;
            pso.RasterizerState.AntialiasedLineEnable = desc.Rasterizer.AntialiasedLineEnable;
            pso.RasterizerState.ForcedSampleCount = desc.Rasterizer.ForcedSampleCount;
            pso.RasterizerState.ConservativeRaster = GetD3D12RasterizationMode(desc.Rasterizer.RasterizationMode);

            // DepthStencilState
            pso.DepthStencilState.DepthEnable = desc.DepthStencil.DepthEnable;
            pso.DepthStencilState.DepthWriteMask = GetD3D12DepthWriteMask(desc.DepthStencil.WriteMask);
            pso.DepthStencilState.DepthFunc = GetComparisonType(desc.DepthStencil.Comparison);
            pso.DepthStencilState.StencilEnable = desc.DepthStencil.StencilEnable;
            pso.DepthStencilState.StencilReadMask = desc.DepthStencil.StencilReadMask;
            pso.DepthStencilState.StencilWriteMask = desc.DepthStencil.StencilWriteMask;
            pso.DepthStencilState.FrontFace.StencilFailOp = GetD3D12StencilOp(desc.DepthStencil.FrontFace.StencilFailOp);
            pso.DepthStencilState.FrontFace.StencilDepthFailOp = GetD3D12StencilOp(desc.DepthStencil.FrontFace.StencilDepthFailOp);
            pso.DepthStencilState.FrontFace.StencilPassOp = GetD3D12StencilOp(desc.DepthStencil.FrontFace.StencilPassOp);
            pso.DepthStencilState.FrontFace.StencilFunc = GetComparisonType(desc.DepthStencil.FrontFace.Comparison);
            pso.DepthStencilState.BackFace.StencilFailOp = GetD3D12StencilOp(desc.DepthStencil.BackFace.StencilFailOp);
            pso.DepthStencilState.BackFace.StencilDepthFailOp = GetD3D12StencilOp(desc.DepthStencil.BackFace.StencilDepthFailOp);
            pso.DepthStencilState.BackFace.StencilPassOp = GetD3D12StencilOp(desc.DepthStencil.BackFace.StencilPassOp);
            pso.DepthStencilState.BackFace.StencilFunc = GetComparisonType(desc.DepthStencil.BackFace.Comparison);

            // InputLayout
            const InputLayoutDesc& il = desc.InputLayout;
            u32 numElements = il.NumElements();
            auto pElements = il.GetElements();

            std::vector<D3D12_INPUT_ELEMENT_DESC> elements;
            elements.resize(static_cast<size_t>(numElements));

            for (u32 i = 0; i < numElements; i++)
            {
                auto& e = pElements[i];
                auto& desc = elements[i];
                desc.SemanticName = e.SemanticName;
                desc.SemanticIndex = e.SemanticIndex;
                desc.Format = GetDxgiFormat(e.Format);
                desc.InputSlot = e.InputSlot;
                desc.AlignedByteOffset = e.ByteOffset;
                desc.InputSlotClass = GetD3D12InputClassification(e.ElementType);
                desc.InstanceDataStepRate = e.InstanceRate;
            }

            pso.InputLayout.NumElements = numElements;
            pso.InputLayout.pInputElementDescs = elements.data();

            // IBStripCutValue

            // Topology
            LUZASSERT(desc.Topology != PrimitiveTopologyType::GFX_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED);
            pso.PrimitiveTopologyType = GetD3D12PrimitiveTopoglogyType(desc.Topology);

            // RenderTargets and DepthStencil
            if (desc.UseSwapChain)
            {
                pso.NumRenderTargets = 1;
                pso.RTVFormats[0] = s_swapChain.RenderTargetFormat;
                pso.DSVFormat = s_swapChain.pDepthStencilResource->GetDesc().Format;
            }
            else
            {
                pso.NumRenderTargets = desc.NumRenderTargets;
                for (u32 i = 0; i < desc.NumRenderTargets; ++i)
                {
                    pso.RTVFormats[i] = s_textureCollection.GetData(desc.pRenderTargets[i]).pResource->GetDesc().Format;
                }

                pso.DSVFormat = s_depthStencilCollection.GetData(desc.DsHandle).DsvDesc.Format;
            }

            // Samplers
            pso.SampleDesc.Quality = desc.SampleQuality;
            pso.SampleDesc.Count = desc.SampleCount;

            // NodeMask
            pso.NodeMask = desc.NodeMask;

            // Cached PSO
            pso.CachedPSO.CachedBlobSizeInBytes = 0;
            pso.CachedPSO.pCachedBlob = nullptr;

            // Flags
            pso.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

            hr = s_device.pDevice->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipeline.pPipelineState));
            LUZASSERT(SUCCEEDED(hr));
        }

        return handle;
    }

    RenderTargetHandle CreateRenderTarget(const RenderTargetDesc& desc)
    {
        RenderTargetHandle handle = s_renderTargetCollection.AllocateHandle();
        if (handle != GPU_RESOURCE_HANDLE_INVALID)
        {
            RenderTarget& rt = s_renderTargetCollection.GetData(handle);

            DXGI_FORMAT format = GetDxgiFormat(desc.Format);

            D3D12_CLEAR_VALUE clearValue = {};
            clearValue.Format = format;
            if (desc.pColor)
            {
                clearValue.Color[0] = desc.pColor[0];
                clearValue.Color[1] = desc.pColor[1];
                clearValue.Color[2] = desc.pColor[2];
                clearValue.Color[3] = desc.pColor[3];
            }
            else
            {
                clearValue.Color[0] = 0.0f;
                clearValue.Color[1] = 0.0f;
                clearValue.Color[2] = 0.0f;
                clearValue.Color[3] = 0.0f;
            }


            HRESULT hr = s_device.pDevice->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Tex2D(format, desc.Width, desc.Height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                &clearValue,
                IID_PPV_ARGS(&rt.pResource));
            LUZASSERT(SUCCEEDED(hr));

            rt.CpuHandle = AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1).CpuHandle;
        }

        return handle;
    }

    DepthStencilHandle CreateDepthStencil(const DepthStencilDesc& desc) 
    {
        DepthStencilHandle handle = s_depthStencilCollection.AllocateHandle(GFX_DESCRIPTOR_HANDLE_TYPE_DSV, s_nDescriptorTypeBits);
        if (handle != GPU_RESOURCE_HANDLE_INVALID)
        {
            DepthStencil& ds = s_depthStencilCollection.GetData(handle);

            bool createSrv = (desc.Flags & GFX_DEPTH_STENCIL_FLAG_SHADER_RESOURCE);
            
            // Create Depth Stencil View
            DXGI_FORMAT format = (createSrv) ? DXGI_FORMAT_R24G8_TYPELESS : GetDxgiFormat(desc.Format);
            
            D3D12_CLEAR_VALUE clearValue = {};
            clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            clearValue.DepthStencil.Depth = 1.0f;
            clearValue.DepthStencil.Stencil = 0;

            HRESULT hr = s_device.pDevice->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Tex2D(format, desc.Width, desc.Height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
                D3D12_RESOURCE_STATE_DEPTH_WRITE,
                &clearValue,
                IID_PPV_ARGS(&ds.pResource));
            LUZASSERT(SUCCEEDED(hr));

            ds.DsvHandle = AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);

            ds.DsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            ds.DsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            ds.DsvDesc.Texture2D.MipSlice = 0;
            ds.DsvDesc.Flags = D3D12_DSV_FLAG_NONE;

            s_device.pDevice->CreateDepthStencilView(ds.pResource, &ds.DsvDesc, ds.DsvHandle.CpuHandle);

            // Handle SRV creation
            if (createSrv)
            {
                ds.SrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                ds.SrvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; // TODO: Maybe a method that converts from depth writable format to depth readable format. How many options are there really?
                ds.SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                ds.SrvDesc.Texture2D.MipLevels = 1;
                ds.SrvDesc.Texture2D.MostDetailedMip = 0;
                ds.SrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
                ds.SrvDesc.Texture2D.PlaneSlice = 0;

                ds.SrvHandle = AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);

                s_device.pDevice->CreateShaderResourceView(ds.pResource, &ds.SrvDesc, ds.SrvHandle.CpuHandle);
            }
        }

        return handle;
    }

    VertexBufferHandle CreateVertexBuffer(const BufferDesc& desc)
    {
        VertexBufferHandle handle = s_vertexBufferCollection.AllocateHandle();
        if (handle != GPU_RESOURCE_HANDLE_INVALID)
        {
            VertexBuffer& vb = s_vertexBufferCollection.GetData(handle);

            // No lock required https://msdn.microsoft.com/en-us/library/windows/desktop/dn899178(v=vs.85).aspx
            HRESULT hr = s_device.pDevice->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), 
                D3D12_HEAP_FLAG_NONE, 
                &CD3DX12_RESOURCE_DESC::Buffer(desc.SizeInBytes, D3D12_RESOURCE_FLAG_NONE, desc.Alignment), 
                D3D12_RESOURCE_STATE_COPY_DEST, 
                nullptr, 
                IID_PPV_ARGS(&vb.pResource));
            LUZASSERT(SUCCEEDED(hr));

            if (desc.pData)
            {
                ID3D12Resource* pUploadBuffer;
                hr = s_device.pDevice->CreateCommittedResource(
                    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                    D3D12_HEAP_FLAG_NONE,
                    &CD3DX12_RESOURCE_DESC::Buffer(desc.SizeInBytes, D3D12_RESOURCE_FLAG_NONE, desc.Alignment),
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    nullptr,
                    IID_PPV_ARGS(&pUploadBuffer));
                LUZASSERT(SUCCEEDED(hr));

                D3D12_SUBRESOURCE_DATA subresource = {};
                subresource.pData = reinterpret_cast<BYTE*>(desc.pData);
                subresource.RowPitch = desc.SizeInBytes;
                subresource.SlicePitch = desc.SizeInBytes;

                // TODO: Create Command List on the fly?
                ID3D12GraphicsCommandList* pGraphicsCommandList;

                CommandContext ctx = AllocateCommandContext(GFX_COMMAND_QUEUE_TYPE_DRAW);
                hr = s_device.pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, ctx.pCommandAllocator, nullptr, IID_PPV_ARGS(&pGraphicsCommandList));
                LUZASSERT(SUCCEEDED(hr));

                UpdateSubresources(pGraphicsCommandList, vb.pResource, pUploadBuffer, 0, 0, 1, &subresource);
                pGraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(vb.pResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
                
                uint64_t execution = ExecuteCommandList(pGraphicsCommandList, GFX_COMMAND_QUEUE_TYPE_DRAW);
				FreeCommandContext(ctx.Index, execution, GFX_COMMAND_QUEUE_TYPE_DRAW);
                SignalQueue(GFX_COMMAND_QUEUE_TYPE_DRAW, execution, true);

                SAFE_RELEASE(pUploadBuffer);
                SAFE_RELEASE(pGraphicsCommandList);
            }

            vb.View.BufferLocation = vb.pResource->GetGPUVirtualAddress();
            vb.View.SizeInBytes = static_cast<UINT>(desc.SizeInBytes);
            vb.View.StrideInBytes = static_cast<UINT>(desc.StrideInBytes);
        }

        return handle;
    }

    IndexBufferHandle CreateIndexBuffer(const BufferDesc& desc)
    {
        IndexBufferHandle handle = s_indexBufferCollection.AllocateHandle();
        if (handle != GPU_RESOURCE_HANDLE_INVALID)
        {
            IndexBuffer& ib = s_indexBufferCollection.GetData(handle);

            // No lock required https://msdn.microsoft.com/en-us/library/windows/desktop/dn899178(v=vs.85).aspx
            HRESULT hr = s_device.pDevice->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(desc.SizeInBytes, D3D12_RESOURCE_FLAG_NONE, desc.Alignment),
                D3D12_RESOURCE_STATE_COPY_DEST,
                nullptr,
                IID_PPV_ARGS(&ib.pResource));
            LUZASSERT(SUCCEEDED(hr));

            if (desc.pData)
            {
                ID3D12Resource* pUploadBuffer;
                hr = s_device.pDevice->CreateCommittedResource(
                    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                    D3D12_HEAP_FLAG_NONE,
                    &CD3DX12_RESOURCE_DESC::Buffer(desc.SizeInBytes, D3D12_RESOURCE_FLAG_NONE, desc.Alignment),
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    nullptr,
                    IID_PPV_ARGS(&pUploadBuffer));
                LUZASSERT(SUCCEEDED(hr));

                D3D12_SUBRESOURCE_DATA subresource = {};
                subresource.pData = reinterpret_cast<BYTE*>(desc.pData);
                subresource.RowPitch = desc.SizeInBytes;
                subresource.SlicePitch = desc.SizeInBytes;

                // TODO: Create Command List on the fly?
                ID3D12GraphicsCommandList* pGraphicsCommandList;

                CommandContext ctx = AllocateCommandContext(GFX_COMMAND_QUEUE_TYPE_DRAW);
                hr = s_device.pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, ctx.pCommandAllocator, nullptr, IID_PPV_ARGS(&pGraphicsCommandList));
                LUZASSERT(SUCCEEDED(hr));

                UpdateSubresources(pGraphicsCommandList, ib.pResource, pUploadBuffer, 0, 0, 1, &subresource);
                pGraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(ib.pResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER));

				uint64_t execution = ExecuteCommandList(pGraphicsCommandList, GFX_COMMAND_QUEUE_TYPE_DRAW);
				FreeCommandContext(ctx.Index, execution, GFX_COMMAND_QUEUE_TYPE_DRAW);
				SignalQueue(GFX_COMMAND_QUEUE_TYPE_DRAW, execution, true);

                SAFE_RELEASE(pUploadBuffer);
                SAFE_RELEASE(pGraphicsCommandList);
            }

            DXGI_FORMAT format;
            switch (desc.StrideInBytes)
            {
            case 1: format = DXGI_FORMAT_R8_UINT; break;
            case 2: format = DXGI_FORMAT_R16_UINT; break;
            case 4: format = DXGI_FORMAT_R32_UINT; break;
            default: LUZASSERT(false);
            }

            ib.View.BufferLocation = ib.pResource->GetGPUVirtualAddress();
            ib.View.SizeInBytes = static_cast<UINT>(desc.SizeInBytes);
            ib.View.Format = format;
            ib.NumIndices = static_cast<u32>(desc.SizeInBytes / desc.StrideInBytes);
        }

        return handle;
    }

    ConstantBufferHandle CreateConstantBuffer(const ConstantBufferDesc& desc)
    {
        ConstantBufferHandle handle = s_constantBufferCollection.AllocateHandle(GFX_DESCRIPTOR_HANDLE_TYPE_CBV, s_nDescriptorTypeBits);
        if (handle != GPU_RESOURCE_HANDLE_INVALID)
        {
            ConstantBuffer& cb = s_constantBufferCollection.GetData(handle);

            HRESULT hr = s_device.pDevice->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(desc.SizeInBytes, D3D12_RESOURCE_FLAG_NONE, desc.Alignment),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&cb.pResource));
            LUZASSERT(SUCCEEDED(hr));

            cb.GpuVirtualAddress = cb.pResource->GetGPUVirtualAddress();

            if (desc.AllocHeap)
            {
                cb.CbvHandle = AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);

                D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
                ZeroMemory(&cbvDesc, sizeof(D3D12_CONSTANT_BUFFER_VIEW_DESC));
                cbvDesc.BufferLocation = cb.GpuVirtualAddress;
                cbvDesc.SizeInBytes = static_cast<UINT>(desc.SizeInBytes);
                s_device.pDevice->CreateConstantBufferView(&cbvDesc, cb.CbvHandle.CpuHandle);
            }

            if (desc.pData)
            {
                u32* gpuAddress = nullptr;
                hr = cb.pResource->Map(0, &CD3DX12_RANGE(0, 0), reinterpret_cast<void**>(&gpuAddress));
                LUZASSERT(SUCCEEDED(hr));
                memcpy(gpuAddress, desc.pData, static_cast<size_t>(desc.SizeInBytes));
                cb.pResource->Unmap(0, &CD3DX12_RANGE(0, 0));
            }
        }

        return handle;
    }

    TextureHandle CreateTexture(const TextureDesc& desc)
    {
        TextureHandle handle = s_textureCollection.AllocateHandle(GFX_DESCRIPTOR_HANDLE_TYPE_SRV, s_nDescriptorTypeBits);
        if (handle != GPU_RESOURCE_HANDLE_INVALID)
        {
            Texture& tex = s_textureCollection.GetData(handle);

            DXGI_FORMAT format = GetDxgiFormat(desc.Format);

            D3D12_CLEAR_VALUE clearValue = {};
            clearValue.Format = format;
            if (desc.pColor)
            {
                clearValue.Color[0] = desc.pColor[0];
                clearValue.Color[1] = desc.pColor[1];
                clearValue.Color[2] = desc.pColor[2];
                clearValue.Color[3] = desc.pColor[3];
            }
            else
            {
                clearValue.Color[0] = 0.0f;
                clearValue.Color[1] = 0.0f;
                clearValue.Color[2] = 0.0f;
                clearValue.Color[3] = 0.0f;
            }
            
            D3D12_RESOURCE_FLAGS flags = GetD3D12ResourceFlags(desc.Flags);

            if (desc.Dimension == GFX_RESOURCE_DIMENSION_TEXTURE1D)
            {
                HRESULT hr = s_device.pDevice->CreateCommittedResource(
                    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                    D3D12_HEAP_FLAG_NONE,
                    &CD3DX12_RESOURCE_DESC::Tex1D(format, desc.Width, desc.Depth, desc.MipLevels, flags),
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    &clearValue,
                    IID_PPV_ARGS(&tex.pResource));
                LUZASSERT(SUCCEEDED(hr));
            }
            else if (desc.Dimension == GFX_RESOURCE_DIMENSION_TEXTURE2D)
            {
                HRESULT hr = s_device.pDevice->CreateCommittedResource(
                    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                    D3D12_HEAP_FLAG_NONE,
                    &CD3DX12_RESOURCE_DESC::Tex2D(format, desc.Width, desc.Height, desc.Depth, desc.MipLevels, desc.SampleCount, desc.SampleQuality, flags),
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    &clearValue,
                    IID_PPV_ARGS(&tex.pResource));
                LUZASSERT(SUCCEEDED(hr));
            }
            else if (desc.Dimension == GFX_RESOURCE_DIMENSION_TEXTURE3D)
            {
                HRESULT hr = s_device.pDevice->CreateCommittedResource(
                    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                    D3D12_HEAP_FLAG_NONE,
                    &CD3DX12_RESOURCE_DESC::Tex3D(format, desc.Width, desc.Height, desc.Depth, desc.MipLevels, flags),
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    &clearValue,
                    IID_PPV_ARGS(&tex.pResource));
                LUZASSERT(SUCCEEDED(hr));
            }
            else LUZASSERT(false);

            //if (desc.pData)
            //{
            //    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
            //    HRESULT hr = DirectX::LoadDDSTextureFromMemoryEx(s_device.pDevice, static_cast<uint8_t*>(desc.pData), desc.SizeInBytes, 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_FLAGS::DDS_LOADER_MIP_AUTOGEN, &tex.pResource, subresources);
            //    LUZASSERT(SUCCEEDED(hr));

            //    UINT subresourceSize = static_cast<UINT>(subresources.size());
            //    const UINT64 uploadBufferSize = GetRequiredIntermediateSize(tex.pResource, 0, subresourceSize);
            //
            //    ID3D12Resource* pUploadBuffer;
            //    hr = s_device.pDevice->CreateCommittedResource(
            //        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            //        D3D12_HEAP_FLAG_NONE,
            //        &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
            //        D3D12_RESOURCE_STATE_GENERIC_READ,
            //        nullptr,
            //        IID_PPV_ARGS(&pUploadBuffer));
            //    LUZASSERT(SUCCEEDED(hr));

            //    ID3D12GraphicsCommandList* pCommandList = nullptr;
            //    UpdateSubresources(pCommandList, tex.pResource, pUploadBuffer, 0, 0, subresourceSize, subresources.data());
            //    pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(tex.pResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

            //    ID3D12CommandQueue* pQueue = nullptr;
            //    pQueue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(&pCommandList));

            //    // TODO: need to wait here
            //    SAFE_RELEASE(pUploadBuffer);

            //    Dx12::DescriptorHandle heap = Dx12::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
            //    tex.SrvHandle.CpuHandle = heap.CpuHandle();
            //    tex.SrvHandle.GpuHandle = heap.GpuHandle();

            //    // Handle render target view

            //    // Handle unordered access view
            //}

            if ((desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) == 0)
            {
                tex.SrvHandle = AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
                D3D12_SHADER_RESOURCE_VIEW_DESC srv = GetD3D12ShaderResourceViewDesc(desc);
                s_device.pDevice->CreateShaderResourceView(tex.pResource, &srv, tex.SrvHandle.CpuHandle);
            }

            if (desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
            {
                if (desc.Depth > 1)
                {
                    tex.RtvHandle = AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, desc.Depth * desc.MipLevels);

                    for (uint16_t mip = 0; mip < desc.MipLevels; ++mip)
                    {
                        for (uint16_t arr = 0; arr < desc.Depth; ++arr)
                        {
                            D3D12_RENDER_TARGET_VIEW_DESC rtv;
                            ZeroMemory(&rtv, sizeof(D3D12_RENDER_TARGET_VIEW_DESC));
                            rtv.Format = format;
                            rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
                            rtv.Texture2DArray.MipSlice = mip;
                            rtv.Texture2DArray.FirstArraySlice = arr;
                            rtv.Texture2DArray.ArraySize = 1;
                            rtv.Texture2DArray.PlaneSlice = 0;
                            s_device.pDevice->CreateRenderTargetView(tex.pResource, &rtv, CD3DX12_CPU_DESCRIPTOR_HANDLE(tex.RtvHandle.CpuHandle, desc.Depth * static_cast<UINT>(mip) + static_cast<UINT>(arr), GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)));
                        }
                    }
                }
                else
                {
                    tex.RtvHandle = AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, desc.Depth);
                    D3D12_RENDER_TARGET_VIEW_DESC rtv = GetD3D12RenderTargetViewDesc(desc);
                    s_device.pDevice->CreateRenderTargetView(tex.pResource, &rtv, tex.RtvHandle.CpuHandle);
                }
            }
        }

        return handle;
    }

    TextureHandle CreateTexture(const TextureFileDesc& desc) 
    {
		static std::atomic_bool s_coInitialized = false;
		if (!s_coInitialized)
		{
			HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
			if (SUCCEEDED(hr))
			{
				s_coInitialized = true;
			}
			else
			{
				LUZASSERT(0);
			}
		}

        wchar_t pWide[MAX_PATH];
        MultiByteToWideChar(CP_UTF8, 0, desc.Filename, -1, pWide, static_cast<int>(strlen(desc.Filename) + 1));

		LPCWSTR extension = PathFindExtensionW(pWide);
		DirectX::ScratchImage image;
		bool bImageLoaded = false;

		if (_wcsicmp(L".dds", extension) == 0)
		{
			bImageLoaded = Internal::LoadDDSImage(pWide, image, desc.GenMips);
		}
		else if (_wcsicmp(L".tga", extension) == 0)
		{
			bImageLoaded = Internal::LoadTgaImage(pWide, image, desc.GenMips);
		}
		else
		{
			bImageLoaded = Internal::LoadWICImage(pWide, image, desc.GenMips);
		}

		LUZASSERT(bImageLoaded);

		if (!bImageLoaded) return GPU_RESOURCE_HANDLE_INVALID;

		const DirectX::TexMetadata& imageMetadata = image.GetMetadata();

        TextureHandle handle = s_textureCollection.AllocateHandle(GFX_DESCRIPTOR_HANDLE_TYPE_SRV, s_nDescriptorTypeBits);
        if (handle != GPU_RESOURCE_HANDLE_INVALID)
        {
            Texture& tex = s_textureCollection.GetData(handle);
            HRESULT hr = DirectX::CreateTexture(s_device.pDevice, imageMetadata, &tex.pResource);
            LUZASSERT(SUCCEEDED(hr));

            tex.pResource->SetName(pWide);

            std::vector<D3D12_SUBRESOURCE_DATA> subresources;
            hr = PrepareUpload(s_device.pDevice, image.GetImages(), image.GetImageCount(), imageMetadata, subresources);
            LUZASSERT(SUCCEEDED(hr));

            UINT subresourceSize = static_cast<UINT>(subresources.size());
            const UINT64 uploadBufferSize = GetRequiredIntermediateSize(tex.pResource, 0, subresourceSize);

            ID3D12Resource* pUploadBuffer;
            hr = s_device.pDevice->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&pUploadBuffer));
            LUZASSERT(SUCCEEDED(hr));

            // TODO: Create Command List on the fly?
            ID3D12GraphicsCommandList* pGraphicsCommandList;

            CommandContext ctx = AllocateCommandContext(GFX_COMMAND_QUEUE_TYPE_DRAW);
            hr = s_device.pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, ctx.pCommandAllocator, nullptr, IID_PPV_ARGS(&pGraphicsCommandList));
            LUZASSERT(SUCCEEDED(hr));

            UpdateSubresources(pGraphicsCommandList, tex.pResource, pUploadBuffer, 0, 0, subresourceSize, subresources.data());
            pGraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(tex.pResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
            
			uint64_t execution = ExecuteCommandList(pGraphicsCommandList, GFX_COMMAND_QUEUE_TYPE_DRAW);
			FreeCommandContext(ctx.Index, execution, GFX_COMMAND_QUEUE_TYPE_DRAW);
            SignalQueue(GFX_COMMAND_QUEUE_TYPE_DRAW, execution, true);

            // create srv
            tex.SrvHandle = AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
            D3D12_SHADER_RESOURCE_VIEW_DESC srv = GetD3D12ShaderResourceViewDesc(imageMetadata);
            s_device.pDevice->CreateShaderResourceView(tex.pResource, &srv, tex.SrvHandle.CpuHandle);

            SAFE_RELEASE(pUploadBuffer);
            SAFE_RELEASE(pGraphicsCommandList);
        }

        return handle;
    }

    void ReleaseShader(const ShaderHandle handle)
    {
        Shader& shader = s_shaderCollection.GetData(handle);
        shader.ByteCode.BytecodeLength = 0;
        shader.ByteCode.pShaderBytecode = nullptr;
        SAFE_RELEASE(shader.pShader);
        s_shaderCollection.FreeHandle(handle);
    }

    void ReleasePipeline(const PipelineStateHandle handle)
    {
        Pipeline& pipeline = s_pipelineCollection.GetData(handle);
        SAFE_RELEASE(pipeline.pPipelineState);
        SAFE_RELEASE(pipeline.pSignature);
        s_pipelineCollection.FreeHandle(handle);
    }

    void ReleaseRenderTarget(const RenderTargetHandle handle)
    {
        RenderTarget& rt = s_renderTargetCollection.GetData(handle);
        rt.CpuHandle.ptr = 0;

        for (u32 i = 0; i < GFX_COMMAND_QUEUE_TYPE_NUM_TYPES; ++i)
            WaitOnFence(s_commandQueues->pFence, rt.Execution[i]);
        SAFE_RELEASE(rt.pResource);

        s_renderTargetCollection.FreeHandle(handle);
    }

    void ReleaseDepthStencil(const DepthStencilHandle handle)
    {
        DepthStencil& ds = s_depthStencilCollection.GetData(handle);
        ds.DsvHandle.CpuHandle.ptr = 0;
        ds.DsvHandle.GpuHandle.ptr = 0;
        ds.SrvHandle.CpuHandle.ptr = 0;
        ds.SrvHandle.GpuHandle.ptr = 0;

        for (u32 i = 0; i < GFX_COMMAND_QUEUE_TYPE_NUM_TYPES; ++i)
            WaitOnFence(s_commandQueues->pFence, ds.Execution[i]);
        SAFE_RELEASE(ds.pResource);

        s_depthStencilCollection.FreeHandle(handle);
    }

    void ReleaseVertexBuffer(const VertexBufferHandle handle)
    {
        VertexBuffer& vb = s_vertexBufferCollection.GetData(handle);
        vb.View.BufferLocation = 0;
        vb.View.SizeInBytes = 0;
        vb.View.StrideInBytes = 0;

        for (u32 i = 0; i < GFX_COMMAND_QUEUE_TYPE_NUM_TYPES; ++i)
            WaitOnFence(s_commandQueues->pFence, vb.Execution[i]);
        SAFE_RELEASE(vb.pResource);

        s_vertexBufferCollection.FreeHandle(handle);
    }

    void ReleaseIndexBuffer(const IndexBufferHandle handle)
    {
        IndexBuffer& ib = s_indexBufferCollection.GetData(handle);
        ib.View.BufferLocation = 0;
        ib.View.SizeInBytes = 0;
        ib.View.Format = DXGI_FORMAT_UNKNOWN;

        for (u32 i = 0; i < GFX_COMMAND_QUEUE_TYPE_NUM_TYPES; ++i)
           WaitOnFence(s_commandQueues->pFence, ib.Execution[i]);
        SAFE_RELEASE(ib.pResource);

        s_indexBufferCollection.FreeHandle(handle);
    }

    void ReleaseConstantBuffer(const ConstantBufferHandle handle)
    {
        ConstantBuffer& cb = s_constantBufferCollection.GetData(handle);
        cb.CbvHandle.CpuHandle.ptr = 0;
        cb.CbvHandle.GpuHandle.ptr = 0;

        for (u32 i = 0; i < GFX_COMMAND_QUEUE_TYPE_NUM_TYPES; ++i)
            WaitOnFence(s_commandQueues->pFence, cb.Execution[i]);
        SAFE_RELEASE(cb.pResource);

        s_constantBufferCollection.FreeHandle(handle);
    }

    void ReleaseTexture(const TextureHandle handle)
    {
        Texture& tx = s_textureCollection.GetData(handle);
        tx.RtvHandle.CpuHandle.ptr = 0;
        tx.RtvHandle.GpuHandle.ptr = 0;
        tx.SrvHandle.CpuHandle.ptr = 0;
        tx.SrvHandle.GpuHandle.ptr = 0;

        for (u32 i = 0; i < GFX_COMMAND_QUEUE_TYPE_NUM_TYPES; ++i)
            WaitOnFence(s_commandQueues->pFence, tx.Execution[i]);
        SAFE_RELEASE(tx.pResource);

        s_textureCollection.FreeHandle(handle);
    }

    void UpdateConstantBuffer(const void* pData, const u64 sizeInBytes, const ConstantBufferHandle handle)
    {
        LUZASSERT(handle != GPU_RESOURCE_HANDLE_INVALID);
        ConstantBuffer& cb = s_constantBufferCollection.GetData(handle);
        
        u32* gpuAddress = nullptr;
        HRESULT hr = cb.pResource->Map(0, &CD3DX12_RANGE(0, 0), reinterpret_cast<void**>(&gpuAddress));
        CHECK_DEVICE_REMOVED(hr);
        memcpy(gpuAddress, pData, static_cast<size_t>(sizeInBytes));
    }

    void SubmitCommandStream(CommandStream* pCommandStream, bool wait)
    {
        CommandStreamHandle handle = pCommandStream->GetHandle();

        CommandQueueType eQueueType = HandleCoder::extract<CommandQueueType>(handle, s_nCommandQueueTypeBits);
        CommandQueue& cq = s_commandQueues[eQueueType];
        CommandList& cl = s_commandListCollection.GetData(handle);

        uint64_t execution = ExecuteCommandList(cl.pGraphicsCommandList, cl.eType);
		FreeCommandContext(cl.iAllocator, execution, cl.eType);
        SignalQueue(cl.eType, execution, wait);
    }

    void Present()
    {
        CommandContext ctx = AllocateCommandContext(GFX_COMMAND_QUEUE_TYPE_DRAW);

        if (!s_swapChain.pGraphicsCommandList)
        {
            HRESULT hr = s_device.pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, ctx.pCommandAllocator, nullptr, IID_PPV_ARGS(&s_swapChain.pGraphicsCommandList));
            LUZASSERT(SUCCEEDED(hr));
        
            hr = s_swapChain.pGraphicsCommandList->SetName(L"SwapChain Graphics Command List");
            LUZASSERT(SUCCEEDED(hr));
        }
        else
        {
            HRESULT hr = s_swapChain.pGraphicsCommandList->Reset(ctx.pCommandAllocator, nullptr);
            LUZASSERT(SUCCEEDED(hr));
        }
        

        ID3D12Resource* pResource = nullptr;
        HRESULT hr = s_swapChain.pSwapChain3->GetBuffer(s_swapChain.FrameIndex, IID_PPV_ARGS(&pResource));
        LUZASSERT(SUCCEEDED(hr));

        s_swapChain.pGraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pResource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

        uint64_t execution = ExecuteCommandList(s_swapChain.pGraphicsCommandList, ctx.eQueueType);
		FreeCommandContext(ctx.Index, execution, ctx.eQueueType);

        hr = s_swapChain.pSwapChain3->Present(1, 0);
        CHECK_DEVICE_REMOVED(hr);

        s_swapChain.FrameIndex = s_swapChain.pSwapChain3->GetCurrentBackBufferIndex();
        s_swapChain.Frames += 1;

        SignalQueue(ctx.eQueueType, execution, true);
    }

    void Flush()
    {
        for (unsigned int i = 0; i < GFX_COMMAND_QUEUE_TYPE_NUM_TYPES; ++i)
        {
            uint64_t execution = s_commandQueues[i].Executions.fetch_add(1ULL) + 1ULL;
            SignalQueue((CommandQueueType)i, execution, true);
        }
    }

    void CreateCommandStream(const CommandStreamDesc& desc, CommandStream* pCommandStream)
    {
        CommandStreamHandle handle = s_commandListCollection.AllocateHandle(desc.QueueType, s_nCommandQueueTypeBits);
        //switch (desc.QueueType)
        //{
        //case GFX_COMMAND_QUEUE_TYPE_DRAW: handle = CreateGraphicsCommandList(desc.PipelineHandle); break;
        //case GFX_COMMAND_QUEUE_TYPE_COPY: LUZASSERT(false); break;
        //case GFX_COMMAND_QUEUE_TYPE_COMPUTE: LUZASSERT(false); break;
        //default: LUZASSERT(false); break;
        //}

        LUZASSERT(handle != GPU_RESOURCE_HANDLE_INVALID);
        pCommandStream = new(reinterpret_cast<void*>(pCommandStream)) CommandStream(handle);
    }

    void ResetCommandStream(CommandStream* pCommandStream, const PipelineStateHandle pipelineHandle)
    {
		LUZASSERT(pCommandStream);
		
		CommandStreamHandle handle = pCommandStream->GetHandle();
		LUZASSERT(handle != GPU_RESOURCE_HANDLE_INVALID);		

		ID3D12PipelineState * pPipelineState = nullptr;
		ID3D12RootSignature* pSignature = nullptr;
		if (pipelineHandle != GPU_RESOURCE_HANDLE_INVALID)
		{
			Pipeline& pipe = s_pipelineCollection.GetData(pipelineHandle);
			pPipelineState = pipe.pPipelineState;
			pSignature = pipe.pSignature;
		}

        CommandQueueType eQueueType = HandleCoder::extract<CommandQueueType>(handle, s_nCommandQueueTypeBits);

		CommandContext ctx = AllocateCommandContext(eQueueType);
		LUZASSERT(ctx.pCommandAllocator);
		LUZASSERT(ctx.pDescriptorHeap);

		CommandList& cl = s_commandListCollection.GetData(handle);
		if (cl.pGraphicsCommandList)
		{
            HRESULT hr = cl.pGraphicsCommandList->Reset(ctx.pCommandAllocator, pPipelineState);
			LUZASSERT(SUCCEEDED(hr));
		}
		else
		{
            HRESULT hr = s_device.pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, ctx.pCommandAllocator, pPipelineState, IID_PPV_ARGS(&cl.pGraphicsCommandList));
			LUZASSERT(SUCCEEDED(hr));
		}

		if (pSignature)
		{
			cl.pGraphicsCommandList->SetGraphicsRootSignature(pSignature);
		}

		cl.pDescriptorHeap = ctx.pDescriptorHeap;
		cl.iAllocator = ctx.Index;
		cl.eType = GFX_COMMAND_QUEUE_TYPE_DRAW;
    }

    void ReleaseCommandStream(CommandStream* pCommandStream)
    {
        CommandList& cl = s_commandListCollection.GetData(pCommandStream->GetHandle());
        if (cl.pGraphicsCommandList) cl.pGraphicsCommandList->ClearState(nullptr);
        SAFE_RELEASE(cl.pGraphicsCommandList);

        s_commandListCollection.FreeHandle(pCommandStream->GetHandle());
    }
}

#endif

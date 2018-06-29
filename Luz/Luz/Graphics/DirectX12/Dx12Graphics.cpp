#include "stdafx.h"
#include "Graphics.h"

#ifdef DX12
#include "Dx12GraphicsTypes.h"

#include "ResourceCollection.h"
#include "DescriptorHeap.h"
#include "CommandContext.h"
#include "MSPlatformWindow.h"

#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

//#define DX_DEBUG
#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }

#define PIPELINE_MAX 32
#define SHADER_MAX 32
#define RENDER_TARGET_MAX 32
#define DEPTH_STENCIL_MAX 1024
#define VERTEX_BUFFER_MAX 1024
#define INDEX_BUFFER_MAX 1024
#define CONSTANT_BUFFER_MAX 1024
#define TEXTURE_MAX 1024
#define COMMAND_LIST_MAX 32

namespace Internal
{
    static void EnableDebugLayer(ID3D12Debug** ppDebug);
    
    static void ConfigureDebugLayer(ID3D12Device* pDevice);

    static bool CompileShader(const char* filename, const char* entryPoint, const char* target, ID3DBlob** ppCode);

    static bool LoadWICImage(std::wstring filename, DirectX::ScratchImage& outImage, bool genMipsIfNecessary = false);
    
    static bool LoadDDSImage(std::wstring filename, DirectX::ScratchImage& outImage, bool genMipsIfNecessary = false);
    
    static bool LoadTgaImage(std::wstring filename, DirectX::ScratchImage& outImage, bool genMipsIfNecessary = false);
    
    static HRESULT GenerateMips(DirectX::ScratchImage& inImage, DirectX::ScratchImage& outImage);

    void EnableDebugLayer(ID3D12Debug** ppDebug)
    {
        HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(ppDebug));
        LUZASSERT(SUCCEEDED(hr));
        (*ppDebug)->EnableDebugLayer();
    }

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
        std::wstring ws;
        ws.resize(strlen(filename));

        MultiByteToWideChar(CP_UTF8, 0, filename, -1, ws.data(), static_cast<int>(ws.size()));

        ID3DBlob* pError;
        UINT flags1 = 0;

#if _DEBUG
        flags1 |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        flags1 |= D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES;

        HRESULT hr = D3DCompileFromFile(ws.c_str(), nullptr, nullptr, entryPoint, target, flags1, 0, ppCode, &pError);
        if (FAILED(hr))
        {
            OutputDebugStringA((char*)pError->GetBufferPointer());
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
    static Device s_device;
    static SwapChainContext s_swapChain;

    static ID3D12CommandQueue* s_pGraphicsQueue = nullptr;
    static ID3D12Debug* s_pDebug = nullptr;
    static ID3D12DebugDevice* s_pDebugDevice = nullptr;

    typedef TCollection<PipelineStateHandle, Pipeline, PIPELINE_MAX> PipelineCollection;
    typedef TCollection<ShaderHandle, Shader, SHADER_MAX> ShaderCollection;

    typedef TCollection<RenderTargetHandle, RenderTarget, RENDER_TARGET_MAX> RenderTargetCollection;
    typedef TCollection<DepthStencilHandle, DepthStencil, DEPTH_STENCIL_MAX> DepthStencilCollection;
    
    typedef TCollection<VertexBufferHandle, VertexBuffer, VERTEX_BUFFER_MAX> VertexBufferCollection;
    typedef TCollection<IndexBufferHandle, IndexBuffer, INDEX_BUFFER_MAX> IndexBufferCollection;
    typedef TCollection<ConstantBufferHandle, ConstantBuffer, CONSTANT_BUFFER_MAX> ConstantBufferCollection;
    typedef TCollection<TextureHandle, Texture, TEXTURE_MAX> TextureCollection;
    
    typedef TCollection<CommandStreamHandle, CommandList, COMMAND_LIST_MAX> CommandListCollection;

    static ShaderCollection s_shaderCollection;
    
    static PipelineCollection s_pipelineCollection;

    static RenderTargetCollection s_renderTargetCollection;

    static DepthStencilCollection s_depthStencilCollection;

    static VertexBufferCollection s_vertexBufferCollection;
    
    static IndexBufferCollection s_indexBufferCollection;
    
    static ConstantBufferCollection s_constantBufferCollection;
    
    static TextureCollection s_textureCollection;

    static CommandListCollection s_commandListCollection;

    ShaderHandle CreateShader(const char* filename, const char* entryPoint, const char* target);

    bool Initialize(Window* pWindow, u32 numBackBuffers)
    {
#ifdef DX_DEBUG
        Internal::EnableDebugLayer(&s_pDebug);
#endif

        auto pPlatformWindow = dynamic_cast<MS::PlatformWindow*>(pWindow);
        auto handle = pPlatformWindow->Handle();
        auto width = pPlatformWindow->Width();
        auto height = pPlatformWindow->Height();
        auto fullScreen = pPlatformWindow->FullScreen();

        // Create Factory
        HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&s_device.pFactory4));
        if (FAILED(hr)) return false;

        // Create device
        IDXGIAdapter1* pAdapter;
        bool adapterFound = false;
        int adapterIdx = 0;

        while (s_device.pFactory4->EnumAdapters1(adapterIdx, &pAdapter) != DXGI_ERROR_NOT_FOUND)
        {
            DXGI_ADAPTER_DESC1 desc;
            pAdapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                ++adapterIdx;
                continue;
            }

            hr = D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&s_device.pDevice));
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

        // Create main graphics queue
        D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
        ZeroMemory(&commandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
        commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

        s_device.pDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&s_pGraphicsQueue));
        if (FAILED(hr)) return false;

        // Initialize Descriptor heaps
        Dx12::DescriptorHeapAllocator::Initialize(s_device.pDevice);

        // Create swap chain
        s_swapChain.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        s_swapChain.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        s_swapChain.Usage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        s_swapChain.Width = width;
        s_swapChain.Height = height;
        s_swapChain.Handle = handle;
        s_swapChain.NumBuffers = numBackBuffers;
        s_swapChain.FrameIndex = 0;
        s_swapChain.FullScreen = fullScreen;

        DXGI_MODE_DESC backBufferDesc;
        ZeroMemory(&backBufferDesc, sizeof(DXGI_MODE_DESC));
        backBufferDesc.Width = width;
        backBufferDesc.Height = height;
        backBufferDesc.Format = s_swapChain.Format;

        DXGI_SAMPLE_DESC sampleDesc;
        ZeroMemory(&sampleDesc, sizeof(DXGI_SAMPLE_DESC));
        sampleDesc.Count = 1;

        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
        swapChainDesc.BufferCount = numBackBuffers;
        swapChainDesc.BufferDesc = backBufferDesc;
        swapChainDesc.BufferUsage = s_swapChain.Usage;
        swapChainDesc.SwapEffect = s_swapChain.SwapEffect;
        swapChainDesc.OutputWindow = s_swapChain.Handle;
        swapChainDesc.SampleDesc = sampleDesc;
        swapChainDesc.Windowed = !s_swapChain.FullScreen;

        IDXGISwapChain* pTempSwapChain;
        hr = s_device.pFactory4->CreateSwapChain(s_pGraphicsQueue, &swapChainDesc, &pTempSwapChain);
        if (FAILED(hr))
        {
            return false;
        }

        s_swapChain.pSwapChain3 = static_cast<IDXGISwapChain3*>(pTempSwapChain);
        s_swapChain.FrameIndex = s_swapChain.pSwapChain3->GetCurrentBackBufferIndex();

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
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = s_swapChain.pRenderTargetDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        UINT descriptorHandleSize = s_device.pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        for (u32 i = 0; i < s_swapChain.NumBuffers; ++i)
        {
            ID3D12Resource* pSwapChainBuffer = nullptr;
            s_swapChain.pSwapChain3->GetBuffer(i, IID_PPV_ARGS(&pSwapChainBuffer));
            s_swapChain.RenderTargetViewHandles[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHandle, static_cast<INT>(i), descriptorHandleSize);
            s_device.pDevice->CreateRenderTargetView(pSwapChainBuffer, nullptr, s_swapChain.RenderTargetViewHandles[i]);
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

        Dx12::CommandAllocatorPool::Initialize(s_device.pDevice);

        return true;
    }

    void Shutdown()
    {
        // wait for the gpu to finish all frames
        Dx12::CommandAllocatorPool::Destroy();
        Dx12::DescriptorHeapAllocator::Destroy();

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
            SAFE_RELEASE(s_commandListCollection.GetData(static_cast<CommandStreamHandle>(i)).pCommandList);
        }

        SAFE_RELEASE(s_pGraphicsQueue);

        SAFE_RELEASE(s_swapChain.pRenderTargetDescriptorHeap);
        SAFE_RELEASE(s_swapChain.pDepthStencilDescriptorHeap);
        SAFE_RELEASE(s_swapChain.pDepthStencilResource);
        SAFE_RELEASE(s_swapChain.pSwapChain3);
        SAFE_RELEASE(s_device.pFactory4);

#ifdef DX_DEBUG
        s_device.pDevice->QueryInterface(IID_PPV_ARGS(&s_pDebugDevice));
        if (s_pDebugDevice)
        {
            s_pDebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
        }

        SAFE_RELEASE(s_pDebug);
        SAFE_RELEASE(s_pDebugDevice);
#endif

        SAFE_RELEASE(s_device.pDevice);
    }

    ShaderHandle CreateShader(const char* filename, const char* entryPoint, const char* target)
    {
        auto handle = s_shaderCollection.AllocateHandle();
        if (handle != GpuResourceHandle::GPU_RESOURCE_HANDLE_INVALID)
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
                        ranges.emplace_back();
                        auto& range = param.GetDescriptorTableRange(j);
                        auto rangeType = range.Kind;

                        switch (rangeType)
                        {
                        case DescriptorTable::Range::Type::DESCRIPTOR_TABLE_RANGE_TYPE_CONSTANT_VIEW:
                            ranges.back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, range.NumDescriptors, range.BaseRegister, range.RegisterSpace, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
                            break;
                        case DescriptorTable::Range::Type::DESCRIPTOR_TABLE_RANGE_TYPE_SHADER_VIEW:
                            ranges.back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, range.NumDescriptors, range.BaseRegister, range.RegisterSpace, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
                            break;
                        case DescriptorTable::Range::Type::DESCRIPTOR_TABLE_RANGE_TYPE_COMPUTE_VIEW:
                            ranges.back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, range.NumDescriptors, range.BaseRegister, range.RegisterSpace, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
                            break;
                        case DescriptorTable::Range::Type::DESCRIPTOR_TABLE_RANGE_TYPE_SAMPLER:
                            ranges.back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, range.NumDescriptors, range.BaseRegister, range.RegisterSpace, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
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
            LUZASSERT(SUCCEEDED(hr));

            hr = s_device.pDevice->CreateRootSignature(0, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_PPV_ARGS(&pipeline.pSignature));
            LUZASSERT(SUCCEEDED(hr));

            pso.pRootSignature = pipeline.pSignature;
            
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
            pso.PrimitiveTopologyType = GetD3D12PrimitiveTopoglogyType(desc.Topology);

            // RenderTargets and DepthStencil
            if (desc.UseSwapChain)
            {
                DXGI_SWAP_CHAIN_DESC scd;
                hr = s_swapChain.pSwapChain3->GetDesc(&scd);
                LUZASSERT(SUCCEEDED(hr));

                pso.NumRenderTargets = 1;
                pso.RTVFormats[0] = scd.BufferDesc.Format;
                pso.DSVFormat = s_swapChain.pDepthStencilResource->GetDesc().Format;
            }
            else
            {
                pso.NumRenderTargets = desc.NumRenderTargets;
                for (u32 i = 0; i < desc.NumRenderTargets; ++i)
                {
                    pso.RTVFormats[i] = s_textureCollection.GetData(desc.pRenderTargets[i]).pResource->GetDesc().Format;
                }

                pso.DSVFormat = s_textureCollection.GetData(desc.DsHandle).pResource->GetDesc().Format;
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
            clearValue.Color[0] = desc.pColor[0];
            clearValue.Color[1] = desc.pColor[0];
            clearValue.Color[2] = desc.pColor[0];
            clearValue.Color[3] = desc.pColor[0];

            HRESULT hr = s_device.pDevice->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Tex2D(format, desc.Width, desc.Height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                &clearValue,
                IID_PPV_ARGS(&rt.pResource));
            LUZASSERT(SUCCEEDED(hr));

            Dx12::DescriptorHandle heap = Dx12::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1);
            rt.CpuHandle = heap.CpuHandle();
            rt.GpuHandle = heap.GpuHandle();
        }

        return handle;
    }

    DepthStencilHandle CreateDepthStencil(const DepthStencilDesc& desc) 
    {
        DepthStencilHandle handle = s_depthStencilCollection.AllocateHandle();
        if (handle != GPU_RESOURCE_HANDLE_INVALID)
        {
            DepthStencil& ds = s_depthStencilCollection.GetData(handle);

            // Create Depth Stencil View
            DXGI_FORMAT format = GetDxgiFormat(desc.Format);
            D3D12_CLEAR_VALUE clearValue = {};
            clearValue.Format = format;
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

            Dx12::DescriptorHandle heap = Dx12::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
            ds.DepthStencilViewHandle.CpuHandle = heap.CpuHandle();
            ds.DepthStencilViewHandle.GpuHandle = heap.GpuHandle();

            s_device.pDevice->CreateDepthStencilView(ds.pResource, nullptr, ds.DepthStencilViewHandle.CpuHandle);

            // Handle SRV creation
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
                union
                {
                    ID3D12CommandList* pCommandList;
                    ID3D12GraphicsCommandList* pGraphicsCommandList;
                };

                Dx12::CommandAllocator* pAlloc = Dx12::CommandAllocatorPool::Allocate(D3D12_COMMAND_LIST_TYPE_DIRECT);
                hr = s_device.pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pAlloc->Allocator(), nullptr, IID_PPV_ARGS(&pGraphicsCommandList));
                LUZASSERT(SUCCEEDED(hr));

                UpdateSubresources(pGraphicsCommandList, vb.pResource, pUploadBuffer, 0, 0, 1, &subresource);
                pGraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(vb.pResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
                hr = pGraphicsCommandList->Close();
                LUZASSERT(SUCCEEDED(hr));

                // TODO: Use main queue?
                ID3D12CommandQueue* pCommandQueue = s_pGraphicsQueue;
                pCommandQueue->ExecuteCommandLists(1, &pCommandList);

                // TODO: Use existing alloc to wait but make better
                auto& fence = pAlloc->GetFence();
                fence.IncrementSignal();
                hr = pCommandQueue->Signal(fence.Ptr(), fence.Signal());
                LUZASSERT(SUCCEEDED(hr));
                fence.Wait();

                vb.View.BufferLocation = vb.pResource->GetGPUVirtualAddress();
                vb.View.SizeInBytes = static_cast<UINT>(desc.SizeInBytes);
                vb.View.StrideInBytes =static_cast<UINT>(desc.StrideInBytes);

                SAFE_RELEASE(pUploadBuffer);
            }
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
                union
                {
                    ID3D12CommandList* pCommandList;
                    ID3D12GraphicsCommandList* pGraphicsCommandList;
                };

                Dx12::CommandAllocator* pAlloc = Dx12::CommandAllocatorPool::Allocate(D3D12_COMMAND_LIST_TYPE_DIRECT);
                hr = s_device.pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pAlloc->Allocator(), nullptr, IID_PPV_ARGS(&pGraphicsCommandList));
                LUZASSERT(SUCCEEDED(hr));

                UpdateSubresources(pGraphicsCommandList, ib.pResource, pUploadBuffer, 0, 0, 1, &subresource);
                pGraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(ib.pResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER));
                hr = pGraphicsCommandList->Close();
                LUZASSERT(SUCCEEDED(hr));

                // TODO: COmmand queue
                ID3D12CommandQueue* pCommandQueue = s_pGraphicsQueue;
                pCommandQueue->ExecuteCommandLists(1, &pCommandList);

                // TODO: Use existing alloc to wait but make better
                auto& fence = pAlloc->GetFence();
                fence.IncrementSignal();
                hr = pCommandQueue->Signal(fence.Ptr(), fence.Signal());
                LUZASSERT(SUCCEEDED(hr));
                fence.Wait();

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

                SAFE_RELEASE(pUploadBuffer);
            }
        }

        return handle;
    }

    ConstantBufferHandle CreateConstantBuffer(const ConstantBufferDesc& desc)
    {
        ConstantBufferHandle handle = s_constantBufferCollection.AllocateHandle();
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

            if (desc.pData)
            {
                u32* gpuAddress = nullptr;
                hr = cb.pResource->Map(0, &CD3DX12_RANGE(0, 0), reinterpret_cast<void**>(&gpuAddress));
                LUZASSERT(SUCCEEDED(hr));
                memcpy(gpuAddress, desc.pData, static_cast<size_t>(desc.SizeInBytes));
                cb.pResource->Unmap(0, &CD3DX12_RANGE(0, 0));

                cb.GpuVirtualAddress = cb.pResource->GetGPUVirtualAddress();

                if (desc.AllocHeap)
                {
                    Dx12::DescriptorHandle dh = Dx12::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
                    cb.CpuHandle = dh.CpuHandle();
                    cb.GpuHandle = dh.GpuHandle();

                    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
                    ZeroMemory(&cbvDesc, sizeof(D3D12_CONSTANT_BUFFER_VIEW_DESC));
                    cbvDesc.BufferLocation = cb.GpuVirtualAddress;
                    cbvDesc.SizeInBytes = static_cast<UINT>(desc.SizeInBytes);
                    s_device.pDevice->CreateConstantBufferView(&cbvDesc, cb.CpuHandle);
                }
            }
        }

        return handle;
    }

    TextureHandle CreateTexture(const TextureDesc& desc)
    {
        TextureHandle handle = s_textureCollection.AllocateHandle();
        if (handle != GPU_RESOURCE_HANDLE_INVALID)
        {
            Texture& tex = s_textureCollection.GetData(handle);

            DXGI_FORMAT format = GetDxgiFormat(desc.Format);

            D3D12_CLEAR_VALUE clearValue = {};
            clearValue.Format = format;
            clearValue.Color[0] = desc.pColor[0];
            clearValue.Color[1] = desc.pColor[0];
            clearValue.Color[2] = desc.pColor[0];
            clearValue.Color[3] = desc.pColor[0];

            if (desc.Dimension == GFX_RESOURCE_DIMENSION_TEXTURE1D)
            {
                HRESULT hr = s_device.pDevice->CreateCommittedResource(
                    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                    D3D12_HEAP_FLAG_NONE,
                    &CD3DX12_RESOURCE_DESC::Tex1D(format, desc.Width, desc.ArraySize, desc.MipLevels, D3D12_RESOURCE_FLAG_NONE),
                    D3D12_RESOURCE_STATE_COPY_DEST,
                    &clearValue,
                    IID_PPV_ARGS(&tex.pResource));
                LUZASSERT(SUCCEEDED(hr));
            }
            else if (desc.Dimension == GFX_RESOURCE_DIMENSION_TEXTURE2D)
            {
                HRESULT hr = s_device.pDevice->CreateCommittedResource(
                    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                    D3D12_HEAP_FLAG_NONE,
                    &CD3DX12_RESOURCE_DESC::Tex2D(format, desc.Width, desc.Height, desc.ArraySize, desc.MipLevels, desc.SampleCount, desc.SampleQuality, D3D12_RESOURCE_FLAG_NONE),
                    D3D12_RESOURCE_STATE_COPY_DEST,
                    &clearValue,
                    IID_PPV_ARGS(&tex.pResource));
                LUZASSERT(SUCCEEDED(hr));
            }
            else if (desc.Dimension == GFX_RESOURCE_DIMENSION_TEXTURE3D)
            {
                HRESULT hr = s_device.pDevice->CreateCommittedResource(
                    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                    D3D12_HEAP_FLAG_NONE,
                    &CD3DX12_RESOURCE_DESC::Tex3D(format, desc.Width, desc.Height, desc.Depth, desc.MipLevels, D3D12_RESOURCE_FLAG_NONE),
                    D3D12_RESOURCE_STATE_COPY_DEST,
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
        }

        return handle;
    }

    TextureHandle CreateTexture(const TextureFileDesc& desc) 
    {
        TextureHandle handle = s_textureCollection.AllocateHandle();
        if (handle != GPU_RESOURCE_HANDLE_INVALID)
        {
            static std::atomic_bool s_coInitialized = false;
            if (!s_coInitialized)
            {
                CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
                s_coInitialized = true;
            }

            std::wstring ws;
            ws.resize(strlen(desc.Filename));

            MultiByteToWideChar(CP_UTF8, 0, desc.Filename, -1, ws.data(), static_cast<int>(ws.size()));

            LPCWSTR extension = PathFindExtension(ws.c_str());

            DirectX::ScratchImage image;
            if (_wcsicmp(L".dds", extension) == 0)
            {
                Internal::LoadDDSImage(ws, image, desc.GenMips);
            }
            else if (_wcsicmp(L".tga", extension) == 0)
            {
                Internal::LoadTgaImage(ws, image, desc.GenMips);
            }
            else
            {
                Internal::LoadWICImage(ws, image, desc.GenMips);
            }

            Texture& tex = s_textureCollection.GetData(handle);
            HRESULT hr = DirectX::CreateTexture(s_device.pDevice, image.GetMetadata(), &tex.pResource);
            LUZASSERT(SUCCEEDED(hr));

            tex.pResource->SetName(ws.c_str());

            std::vector<D3D12_SUBRESOURCE_DATA> subresources;
            hr = PrepareUpload(s_device.pDevice, image.GetImages(), image.GetImageCount(), image.GetMetadata(), subresources);
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
            union
            {
                ID3D12CommandList* pCommandList;
                ID3D12GraphicsCommandList* pGraphicsCommandList;
            };

            Dx12::CommandAllocator* pAlloc = Dx12::CommandAllocatorPool::Allocate(D3D12_COMMAND_LIST_TYPE_DIRECT);
            hr = s_device.pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pAlloc->Allocator(), nullptr, IID_PPV_ARGS(&pGraphicsCommandList));
            LUZASSERT(SUCCEEDED(hr));

            UpdateSubresources(pGraphicsCommandList, tex.pResource, pUploadBuffer, 0, 0, subresourceSize, subresources.data());
            pGraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(tex.pResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
            hr = pGraphicsCommandList->Close();
            LUZASSERT(SUCCEEDED(hr));
            
            ID3D12CommandQueue* pCommandQueue = s_pGraphicsQueue;
            pCommandQueue->ExecuteCommandLists(1, &pCommandList);

            // TODO: Use existing alloc to wait but make better
            auto& fence = pAlloc->GetFence();
            fence.IncrementSignal();
            hr = pCommandQueue->Signal(fence.Ptr(), fence.Signal());
            LUZASSERT(SUCCEEDED(hr));
            fence.Wait();

            Dx12::DescriptorHandle heap = Dx12::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
            tex.SrvHandle.CpuHandle = heap.CpuHandle();
            tex.SrvHandle.GpuHandle = heap.GpuHandle();

            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
            ZeroMemory(&srvDesc, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Format = image.GetMetadata().format;
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = static_cast<UINT>(image.GetMetadata().mipLevels);

            s_device.pDevice->CreateShaderResourceView(tex.pResource, &srvDesc, tex.SrvHandle.CpuHandle);
            // TODO: RTV, UAV?

            // TODO: need to wait here
            SAFE_RELEASE(pUploadBuffer);
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
        RenderTarget rt = s_renderTargetCollection.GetData(handle);
        rt.CpuHandle.ptr = 0;
        rt.GpuHandle.ptr = 0;
        SAFE_RELEASE(rt.pResource);
        s_renderTargetCollection.FreeHandle(handle);
    }

    void ReleaseDepthStencil(const DepthStencilHandle handle)
    {
        DepthStencil ds = s_depthStencilCollection.GetData(handle);
        ds.DepthStencilViewHandle.CpuHandle.ptr = 0;
        ds.DepthStencilViewHandle.GpuHandle.ptr = 0;
        ds.ShaderResourceViewHandle.CpuHandle.ptr = 0;
        ds.ShaderResourceViewHandle.GpuHandle.ptr = 0;
        SAFE_RELEASE(ds.pResource);
        s_depthStencilCollection.FreeHandle(handle);
    }

    void ReleaseVertexBuffer(const VertexBufferHandle handle)
    {
        VertexBuffer vb = s_vertexBufferCollection.GetData(handle);
        vb.View.BufferLocation = 0;
        vb.View.SizeInBytes = 0;
        vb.View.StrideInBytes = 0;
        SAFE_RELEASE(vb.pResource);
        s_vertexBufferCollection.FreeHandle(handle);
    }

    void ReleaseIndexBuffer(const IndexBufferHandle handle)
    {
        IndexBuffer ib = s_indexBufferCollection.GetData(handle);
        ib.View.BufferLocation = 0;
        ib.View.SizeInBytes = 0;
        ib.View.Format = DXGI_FORMAT_UNKNOWN;
        SAFE_RELEASE(ib.pResource);
        s_indexBufferCollection.FreeHandle(handle);
    }

    void ReleaseConstantBuffer(const ConstantBufferHandle handle)
    {
        ConstantBuffer cb = s_constantBufferCollection.GetData(handle);
        cb.CpuHandle.ptr = 0;
        cb.GpuHandle.ptr = 0;
        SAFE_RELEASE(cb.pResource);
        s_constantBufferCollection.FreeHandle(handle);
    }

    void ReleaseTexture(const TextureHandle handle)
    {
        Texture tex = s_textureCollection.GetData(handle);
        tex.RtvHandle.CpuHandle.ptr = 0;
        tex.RtvHandle.GpuHandle.ptr = 0;
        tex.SrvHandle.CpuHandle.ptr = 0;
        tex.SrvHandle.GpuHandle.ptr = 0;
        SAFE_RELEASE(tex.pResource);
        s_textureCollection.FreeHandle(handle);
    }

    void UpdateConstantBuffer(const void* pData, const u64 sizeInBytes, const ConstantBufferHandle handle)
    {
        LUZASSERT(handle != GPU_RESOURCE_HANDLE_INVALID);
        ConstantBuffer& cb = s_constantBufferCollection.GetData(handle);
        
        u32* gpuAddress = nullptr;
        HRESULT hr = cb.pResource->Map(0, &CD3DX12_RANGE(0, 0), reinterpret_cast<void**>(&gpuAddress));
        LUZASSERT(SUCCEEDED(hr));
        memcpy(gpuAddress, pData, static_cast<size_t>(sizeInBytes));
    }

    void SubmitCommandStream(const CommandStream* pCommandStream, bool wait)
    {
        CommandList& cl = s_commandListCollection.GetData(pCommandStream->GetHandle());
        
        HRESULT hr = cl.pGraphicsCommandList->Close();
        LUZASSERT(SUCCEEDED(hr));

        cl.pCommandQueue->ExecuteCommandLists(1, &cl.pCommandList);

        Dx12::CommandAllocator* pAlloc = reinterpret_cast<Dx12::CommandAllocator*>(cl.pAlloc);
        LUZASSERT(pAlloc);

        auto& fence = pAlloc->GetFence();
        fence.IncrementSignal();
        hr = cl.pCommandQueue->Signal(fence.Ptr(), fence.Signal());
        LUZASSERT(SUCCEEDED(hr));

        if (wait)
        {
            fence.Wait();
        }
    }

    void Present()
    {
        union
        {
            ID3D12CommandList* pCommandList;
            ID3D12GraphicsCommandList* pGraphicsCommandList;
        };

        Dx12::CommandAllocator* pAlloc = Dx12::CommandAllocatorPool::Allocate(D3D12_COMMAND_LIST_TYPE_DIRECT);
        HRESULT hr = s_device.pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pAlloc->Allocator(), nullptr, IID_PPV_ARGS(&pGraphicsCommandList));
        LUZASSERT(SUCCEEDED(hr));

        ID3D12Resource* pResource = nullptr;
        hr = s_swapChain.pSwapChain3->GetBuffer(s_swapChain.FrameIndex, IID_PPV_ARGS(&pResource));
        LUZASSERT(SUCCEEDED(hr));

        pGraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pResource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
        hr = pGraphicsCommandList->Close();
        LUZASSERT(SUCCEEDED(hr));

        ID3D12CommandQueue* pCommandQueue = s_pGraphicsQueue;
        pCommandQueue->ExecuteCommandLists(1, &pCommandList);

        // TODO: Use existing alloc to wait but make better
        auto& fence = pAlloc->GetFence();
        fence.IncrementSignal();
        hr = pCommandQueue->Signal(fence.Ptr(), fence.Signal());
        LUZASSERT(SUCCEEDED(hr));
        fence.Wait();

        hr = s_swapChain.pSwapChain3->Present(0, 0);
        if (FAILED(hr))
        {
            hr = s_device.pDevice->GetDeviceRemovedReason();
            LUZASSERT(false);
        }

        s_swapChain.FrameIndex = s_swapChain.pSwapChain3->GetCurrentBackBufferIndex();
    }

    void CreateCommandStream(const CommandStreamDesc& desc, CommandStream* pCommandStream)
    {
        if (desc.QueueType == GFX_COMMAND_QUEUE_TYPE_MAIN)
        {
            CommandStreamHandle handle = s_commandListCollection.AllocateHandle();
            LUZASSERT(handle != GPU_RESOURCE_HANDLE_INVALID);

            Dx12::CommandAllocator* pAllocator = Dx12::CommandAllocatorPool::Allocate(D3D12_COMMAND_LIST_TYPE_DIRECT);

            CommandList& cl = s_commandListCollection.GetData(handle);
            cl.pCommandQueue = s_pGraphicsQueue;
            cl.pAlloc = reinterpret_cast<void*>(pAllocator);
            
            ID3D12PipelineState* pPipelineState = nullptr;
            if (desc.PipelineHandle != GPU_RESOURCE_HANDLE_INVALID)
            {
                pPipelineState = s_pipelineCollection.GetData(desc.PipelineHandle).pPipelineState;
            }
            HRESULT hr = s_device.pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, reinterpret_cast<Dx12::CommandAllocator*>(cl.pAlloc)->Allocator(), pPipelineState, IID_PPV_ARGS(&cl.pGraphicsCommandList));
            LUZASSERT(SUCCEEDED(hr));

            pCommandStream = new(reinterpret_cast<void*>(pCommandStream)) CommandStream(handle);
        }
        else
        {
            LUZASSERT(false);
        }
    }

    void ReleaseCommandStream(CommandStream* pCommandStream)
    {
        CommandList& cl = s_commandListCollection.GetData(pCommandStream->GetHandle());
        ZeroMemory(&cl, sizeof(CommandList));
        s_commandListCollection.FreeHandle(pCommandStream->GetHandle());
    }

    CommandStream::CommandStream()
        : CommandStream((CommandStreamHandle)-1)
    {

    }

    CommandStream::CommandStream(const CommandStreamHandle handle)
        : m_handle(handle)
    {

    }

    CommandStream::~CommandStream()
    {
        ReleaseCommandStream(this);
    }

    void CommandStream::Reset(const PipelineStateHandle handle)
    {
        Pipeline& pso = s_pipelineCollection.GetData(handle);
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        LUZASSERT(cl.pCommandQueue);
        LUZASSERT(cl.pGraphicsCommandList);
        LUZASSERT(cl.pAlloc);

        Dx12::CommandAllocator* pAllocator = Dx12::CommandAllocatorPool::Allocate(D3D12_COMMAND_LIST_TYPE_DIRECT);
        LUZASSERT(pAllocator->GetFence().InUse());
        cl.pAlloc = reinterpret_cast<void*>(pAllocator);

        ID3D12PipelineState* pPipelineState = nullptr;
        if (handle != GPU_RESOURCE_HANDLE_INVALID)
        {
            pPipelineState = s_pipelineCollection.GetData(handle).pPipelineState;
        }

        HRESULT hr = cl.pGraphicsCommandList->Reset(reinterpret_cast<Dx12::CommandAllocator*>(cl.pAlloc)->Allocator(), pPipelineState);
        LUZASSERT(SUCCEEDED(hr));

        cl.pGraphicsCommandList->SetGraphicsRootSignature(pso.pSignature);
    }

    void CommandStream::SetPipeline(const PipelineStateHandle handle)
    {
        Pipeline& pso = s_pipelineCollection.GetData(handle);
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        
        cl.pGraphicsCommandList->SetPipelineState(pso.pPipelineState);
        cl.pGraphicsCommandList->SetGraphicsRootSignature(pso.pSignature);
    }

    void CommandStream::SetRenderTargets(const u32 numRenderTargets, const RenderTargetHandle* pRtHandles, const DepthStencilHandle dsHandle)
    {
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        RenderTarget& rt = s_renderTargetCollection.GetData(pRtHandles[0]);
        DepthStencil& ds = s_depthStencilCollection.GetData(dsHandle);

        cl.pGraphicsCommandList->OMSetRenderTargets(numRenderTargets, &rt.CpuHandle, TRUE, &ds.DepthStencilViewHandle.CpuHandle);
    }

    void CommandStream::SetRenderTargets()
    {
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        
        ID3D12Resource* pResource = nullptr;
        HRESULT hr = s_swapChain.pSwapChain3->GetBuffer(s_swapChain.FrameIndex, IID_PPV_ARGS(&pResource));
        LUZASSERT(SUCCEEDED(hr));

        cl.pGraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pResource, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
        cl.pGraphicsCommandList->OMSetRenderTargets(1, &s_swapChain.RenderTargetViewHandles[s_swapChain.FrameIndex], FALSE, &s_swapChain.DepthStencilViewHandle);
    }

    void CommandStream::SetViewport(const Viewport& viewport)
    {
        D3D12_VIEWPORT vp;
        ZeroMemory(&vp, sizeof(D3D12_VIEWPORT));
        vp.TopLeftX = static_cast<FLOAT>(viewport.TopLeftX);
        vp.TopLeftY = static_cast<FLOAT>(viewport.TopLeftY);
        vp.Width = static_cast<FLOAT>(viewport.Width);
        vp.Height = static_cast<FLOAT>(viewport.Height);
        vp.MinDepth = static_cast<FLOAT>(viewport.MinDepth);
        vp.MaxDepth = static_cast<FLOAT>(viewport.MaxDepth);

        CommandList& cl = s_commandListCollection.GetData(m_handle);
        cl.pGraphicsCommandList->RSSetViewports(1, &vp);
    }

    void CommandStream::SetScissorRect(const Rect& rect)
    {
        D3D12_RECT scissor;
        ZeroMemory(&scissor, sizeof(D3D12_RECT));
        scissor.left = static_cast<LONG>(rect.Left);
        scissor.top = static_cast<LONG>(rect.Top);
        scissor.right = static_cast<LONG>(rect.Right);
        scissor.bottom = static_cast<LONG>(rect.Bottom);

        CommandList& cl = s_commandListCollection.GetData(m_handle);
        cl.pGraphicsCommandList->RSSetScissorRects(1, &scissor);
    }

    void CommandStream::ClearRenderTarget(const float* pColor, const RenderTargetHandle handle)
    {
        LUZASSERT(handle != GPU_RESOURCE_HANDLE_INVALID);
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        RenderTarget& rt = s_renderTargetCollection.GetData(handle);
        
        cl.pGraphicsCommandList->ClearRenderTargetView(rt.CpuHandle, pColor, 0, nullptr);
    }

    void CommandStream::ClearRenderTarget(const float* pColor)
    {
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        
        cl.pGraphicsCommandList->ClearRenderTargetView(s_swapChain.RenderTargetViewHandles[s_swapChain.FrameIndex], pColor, 0, nullptr);
    }

    void CommandStream::ClearDepthStencil(const float depth, const u8 stencil, const DepthStencilHandle handle)
    {
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        DepthStencil& ds = s_depthStencilCollection.GetData(handle);

        cl.pGraphicsCommandList->ClearDepthStencilView(ds.DepthStencilViewHandle.CpuHandle, D3D12_CLEAR_FLAG_DEPTH, depth, stencil, 0, nullptr);
    }

    void CommandStream::ClearDepthStencil(const float depth, const u8 stencil)
    {
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        cl.pGraphicsCommandList->ClearDepthStencilView(s_swapChain.DepthStencilViewHandle, D3D12_CLEAR_FLAG_DEPTH, depth, stencil, 0, nullptr);
    }

    void CommandStream::SetPrimitiveTopology(const PrimitiveSubtopology topology)
    {
        CommandList& cl = s_commandListCollection.GetData(m_handle);

        cl.pGraphicsCommandList->IASetPrimitiveTopology(GetD3D12PrimitiveTogopology(topology));
    }

    void CommandStream::SetVertexBuffer(const VertexBufferHandle handle)
    {
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        VertexBuffer& vb = s_vertexBufferCollection.GetData(handle);
        
        cl.pGraphicsCommandList->IASetVertexBuffers(0, 1, &vb.View);
    }

    void CommandStream::SetIndexBuffer(const IndexBufferHandle handle)
    {
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        IndexBuffer& ib = s_indexBufferCollection.GetData(handle);

        cl.pGraphicsCommandList->IASetIndexBuffer(&ib.View);
    }

    void CommandStream::SetConstantBuffer(const u32 param, const ConstantBufferHandle handle)
    {
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        ConstantBuffer cb = s_constantBufferCollection.GetData(handle);
        
        cl.pGraphicsCommandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(param), cb.GpuVirtualAddress);
    }

    void CommandStream::SetTexture(const u32 param, const TextureHandle handle)
    {
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        Texture tx = s_textureCollection.GetData(handle);

        cl.pGraphicsCommandList->SetComputeRootShaderResourceView(static_cast<UINT>(param), tx.GpuVirtualAddress);
    }

    void CommandStream::SetDescriptorTable(const u32 param, const ConstantBufferHandle baseHandle)
    {
        ConstantBuffer& cb = s_constantBufferCollection.GetData(baseHandle);

        ID3D12DescriptorHeap* pHeap = Dx12::GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, cb.CpuHandle, cb.GpuHandle);

        CommandList& cl = s_commandListCollection.GetData(m_handle);

        cl.pGraphicsCommandList->SetDescriptorHeaps(1, &pHeap);
        cl.pGraphicsCommandList->SetGraphicsRootDescriptorTable(static_cast<UINT>(param), cb.GpuHandle);
    }

    void CommandStream::DrawInstanceIndexed(const IndexBufferHandle handle, const u32 instanceCount /*= 1*/, const u32 startIndex /*= 0*/, const i32 baseVertexLocation /*= 0*/, const u32 startInstanceLocation /*= 0*/)
    {
        CommandList& cl = s_commandListCollection.GetData(m_handle);
        IndexBuffer& ib = s_indexBufferCollection.GetData(handle);

        cl.pGraphicsCommandList->DrawIndexedInstanced(ib.NumIndices, instanceCount, startIndex, baseVertexLocation, startInstanceLocation);
    }
}

#endif

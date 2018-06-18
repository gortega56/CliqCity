#include "stdafx.h"
#include "Graphics.h"

#ifdef DX12
#include "Dx12GraphicsTypes.h"
#include "ResourceCollection.h"
#include "DescriptorHeap.h"
#include "CommandContext.h"
#include "MSPlatformWindow.h"

#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }

namespace Internal
{
    bool CompileShader(const char* filename, const char* entryPoint, const char* target, ID3DBlob** ppCode);

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
}

namespace Graphics
{
    static Device s_device;
    static SwapChainContext s_swapChain;

    static ID3D12CommandQueue* s_pGraphicsQueue = nullptr;
    static ID3D12Debug* s_pDebug = nullptr;
    static ID3D12DebugDevice* s_pDebugDevice = nullptr;

    typedef ResourceCollection<Shader, 32> ShaderCollection;
    typedef ResourceCollection<Pipeline, 16> PipelineCollection;
    typedef ResourceCollection<DepthStencil, 1024> DepthStencilCollection;
    typedef ResourceCollection<Texture, 1024> TextureCollection;
    
    static ShaderCollection s_shaderCollection;
    static PipelineCollection s_pipelineCollection;
    static TextureCollection s_textureCollection;
    static DepthStencilCollection s_depthStencilCollection;

    static void EnableDebugLayer();
    static void ConfigureDebugLayer();

    ShaderHandle CreateShader(const char* filename, const char* entryPoint, const char* target);

    bool Initialize(Window* pWindow, u32 numBackBuffers)
    {
#ifdef _DEBUG
        EnableDebugLayer();
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
        Dx12::DescriptorHeapAllocator::Initialize();

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
        HRESULT hr = s_device.pFactory4->CreateSwapChain(s_pGraphicsQueue, &swapChainDesc, &pTempSwapChain);
        if (FAILED(hr))
        {
            return false;
        }

        s_swapChain.pSwapChain3 = static_cast<IDXGISwapChain3*>(pTempSwapChain);

        // Create Render Target Views for back buffers
        for (u32 i = 0; i < numBackBuffers; ++i)
        {
            s_swapChain.RenderTargetViewHandles[i] = Dx12::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV).CpuHandle;
        }

        // Create Depth Stencil View
        D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
        depthOptimizedClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
        depthOptimizedClearValue.DepthStencil.Stencil = 0;

        hr = s_device.pDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D24_UNORM_S8_UINT, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthOptimizedClearValue,
            IID_PPV_ARGS(&s_swapChain.pDepthStencilResource));
        if (FAILED(hr)) return false;

        s_swapChain.DepthStencilViewHandle = Dx12::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1).CpuHandle();

        Dx12::CommandAllocatorPool::Initialize();

        return true;
    }

    void Shutdown()
    {
        // wait for the gpu to finish all frames
        Dx12::CommandAllocatorPool::Destroy();
        Dx12::DescriptorHeapAllocator::Destroy();

#ifdef _DEBUG
        s_device.pDevice->QueryInterface(IID_PPV_ARGS(&s_pDebugDevice));
        if (s_pDebugDevice)
        {
            s_pDebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
        }

        SAFE_RELEASE(s_pDebug);
        SAFE_RELEASE(s_pDebugDevice);
#endif
        SAFE_RELEASE(s_device.pDevice);
        SAFE_RELEASE(s_swapChain.pSwapChain3);

    }

    ShaderHandle CreateShader(const char* filename, const char* entryPoint, const char* target)
    {
        auto handle = s_shaderCollection.AllocateHandle();
        if (handle != GpuResourceHandle::GPU_RESOURCE_HANDLE_INVALID)
        {
            auto shader = s_shaderCollection.GetResource(handle);
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
        Pipeline& pipeline = s_pipelineCollection.GetResource(handle);

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
        pso.VS = s_shaderCollection.GetResource(desc.VertexShaderHandle).ByteCode;
        if (desc.HullShaderHandle != GPU_RESOURCE_HANDLE_INVALID) pso.HS = s_shaderCollection.GetResource(desc.HullShaderHandle).ByteCode;
        if (desc.DomainShaderHandle != GPU_RESOURCE_HANDLE_INVALID) pso.DS = s_shaderCollection.GetResource(desc.DomainShaderHandle).ByteCode;
        if (desc.GeometryShaderHandle != GPU_RESOURCE_HANDLE_INVALID) pso.GS = s_shaderCollection.GetResource(desc.GeometryShaderHandle).ByteCode;
        if (desc.PixelShaderHandle != GPU_RESOURCE_HANDLE_INVALID) pso.PS = s_shaderCollection.GetResource(desc.PixelShaderHandle).ByteCode;

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
        pso.NumRenderTargets = desc.NumRenderTargets;
        for (u32 i = 0; i < desc.NumRenderTargets; ++i)
        {
            pso.RTVFormats[i] = s_textureCollection.GetResource(desc.pRenderTargets[i]).pResource->GetDesc().Format;
        }

        pso.DSVFormat = s_textureCollection.GetResource(desc.DsHandle).pResource->GetDesc().Format;

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

        return handle;
    }

    DepthStencilHandle CreateDepthStencil(const DepthStencilDesc& desc) 
    {
        
    }

    VertexBufferHandle CreateVertexBuffer(const BufferDesc& desc)
    {

    }

    IndexBufferHandle CreateIndexBuffer(const BufferDesc& desc)
    {

    }

    ConstantBufferHandle CreateConstantBuffer(const BufferDesc& desc)
    {

    }

    TextureHandle CreateTexture(const TextureDesc& desc)
    {

    }

    void EnableDebugLayer()
    {
        HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&s_pDebug));
        if (SUCCEEDED(hr))
        {
            s_pDebug->EnableDebugLayer();
        }
    }

    void ConfigureDebugLayer()
    {
        ID3D12InfoQueue* pInfoQueue = nullptr;
        if (SUCCEEDED(s_device.pDevice->QueryInterface(IID_PPV_ARGS(&pInfoQueue))))
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
}

#endif

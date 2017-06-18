#include "stdafx.h"
#include "Material.h"
#include "RootSignature.h"
#include "CommandContext.h"
#include "DescriptorHeap.h"
#include "Resource\Texture.h"
#include "dx12_renderer.h"
#include "Resource\ResourceManager.h"
#include "CommandQueue.h"
#include "CommandContext.h"

using namespace Dx12;
using namespace Material;



//std::shared_ptr<const Immutable> Builder::ToImmutable()
//{
//    std::sort(m_srvParams.begin(), m_srvParams.end(), [](const SrvParam& p0, const SrvParam& p1) { return p0.Index() < p1.Index(); });
//
//    u32 numSrvResources = 0;
//    u32 numSrvParams = (int)m_srvParams.size();
//    for (int i = 0; i < numSrvParams; ++i)
//    {
//        auto& srvPreload = m_srvParams[i].PreloadData();
//        std::sort(srvPreload.begin(), srvPreload.end(), [](const SrvPreloadData& p0, const SrvPreloadData& p1) { return p0.Offset() < p1.Offset(); });
//        numSrvResources += (u32)srvPreload.size();
//    }
//    
//    volatile u32 numSrvLoading = numSrvResources;
//
//    std::vector<PixelBuffer> pixelBuffers(numSrvResources);
//
//    ResourceManager rm;
//    for (int paramIndex = 0; paramIndex < numSrvParams; ++paramIndex)
//    {
//        auto& srvPreloadData = m_srvParams[paramIndex].PreloadData();
//        for (int srvIndex = 0, srvCount = (u32)srvPreloadData.size(); srvIndex < srvCount; ++srvIndex)
//        {
//            auto& srvData = srvPreloadData[srvIndex];
//            auto& pixelBuffer = pixelBuffers[paramIndex * numSrvParams + srvIndex];
//            rm.LoadResource<Texture2D>(srvData.Filename(), [&pixelBuffer, &numSrvLoading](std::shared_ptr<const Texture2D> pTexture)
//            {
//                if (!pixelBuffer.InitializeTexture2D(nullptr, pTexture)) __debugbreak();
//                InterlockedDecrement(&numSrvLoading);
//            });
//        }
//    }
//
//    std::sort(m_cbvParams.begin(), m_cbvParams.end(), [](const CbvParam& p0, const CbvParam& p1) { return p0.Index() < p1.Index(); });
//
//    u32 numCbvResources = (int)m_cbvParams.size();
//    std::vector<UploadBuffer> constantBuffers(numCbvResources);
//
//    for (int i = 0; i < numCbvResources; ++i)
//    {
//        auto& cbvParam = m_cbvParams[i];
//        auto& constantBuffer = constantBuffers[i];
//        constantBuffer.SetBufferSize(cbvParam.BufferSize());
//        constantBuffer.SetElementSize(cbvParam.ElementSize());
//        constantBuffer.SetNumElements(cbvParam.NumElements());
//        if (!constantBuffers[i].Initialize(nullptr, cbvParam.Data())) __debugbreak();
//    }
//
//    while (numSrvLoading > 0) {}
//
//    auto pDescriptorHeap = std::make_shared<DescriptorHeap>(numSrvResources);
//    if (!pDescriptorHeap->InitializeMixed(nullptr, L"MaterialName")) __debugbreak();
//
//    for (int i = 0, count = (int)pixelBuffers.size(); i < count; ++i)
//    {
//        pDescriptorHeap->CreateShaderResourceViews(nullptr, &pixelBuffers[i], i);
//    }
//}

Immutable::Immutable(std::shared_ptr<const RootSignature> rootSignature) : m_rootSignature(rootSignature)
{

}

Immutable::Immutable() : Immutable(nullptr)
{

}

void Immutable::SetRootConstantBufferView(std::shared_ptr<const UploadBuffer> uploadBuffer, u32 paramIndex, u32 heapOffset)
{
    LUZASSERT(m_rootSignature->m_rootParameters[paramIndex].ParameterType == D3D12_ROOT_PARAMETER_TYPE_CBV);
    m_gpuResources.push_back(uploadBuffer);
    m_resourceParams.emplace_back(paramIndex, (u32)m_gpuResources.size() - 1, heapOffset, MATERIAL_PARAM_TYPE_CBV);
}

void Immutable::SetRootDescriptorTable(std::shared_ptr<const DescriptorHeap> descriptorHeap, std::vector<std::shared_ptr<const GpuResource>> gpuResources, u32 paramIndex, u32 heapOffset)
{
    LUZASSERT(m_rootSignature->m_rootParameters[paramIndex].ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE);
    LUZASSERT(m_rootSignature->m_rootParameters[paramIndex].DescriptorTable.pDescriptorRanges[heapOffset].RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

    std::copy(gpuResources.begin(), gpuResources.end(), std::back_inserter(m_gpuResources));
    m_descriptorHeaps.push_back(descriptorHeap);
    m_resourceParams.emplace_back(paramIndex, (u32)m_descriptorHeaps.size() - 1, heapOffset, MATERIAL_PARAM_TYPE_TABLE);
}

void Immutable::Prepare(GraphicsCommandContext* pGraphicsContext)
{
    std::vector<ID3D12DescriptorHeap*> descriptorHeaps;
    descriptorHeaps.reserve(m_descriptorHeaps.size());

    for (auto& descriptorHeap : m_descriptorHeaps)
    {
        descriptorHeaps.push_back(descriptorHeap->Native());
    }

    pGraphicsContext->CommandList()->SetDescriptorHeaps((UINT)descriptorHeaps.size(), descriptorHeaps.data());


    for (auto& resParam : m_resourceParams)
    {
        auto rootParam = m_rootSignature->m_rootParameters[resParam.Index];

        if (rootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_CBV)
        {
            pGraphicsContext->SetGraphicsRootConstantBufferView(std::static_pointer_cast<const UploadBuffer>(m_gpuResources[resParam.HeapIndex]).get(), resParam.Index);
        }
        else if (rootParam.ParameterType == D3D12_DESCRIPTOR_RANGE_TYPE_SRV)
        {
            pGraphicsContext->SetGraphicsRootDescriptorTable(m_descriptorHeaps[resParam.HeapIndex].get(), resParam.Index, resParam.HeapOffset);
        }
    }

}

//bool Material::Prepare(GraphicsCommandContext* pCtx)
//{
//
//    return true;
//}
//
//bool Material::Finalize()
//{
// 
//}
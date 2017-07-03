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
#include "GpuResource.h"

using namespace Dx12;
using namespace Material;

static void GetParamIndices(std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters, D3D12_ROOT_PARAMETER_TYPE paramType, std::vector<int> outIndices)
{
    outIndices.clear();
    for (int i = 0, count = (int)rootParameters.size(); i < count; ++i)
    {
        if (rootParameters[i].ParameterType == paramType)
        {
            outIndices.push_back(i);
        }
    }
}

Builder::Builder(std::shared_ptr<const RootSignature> pRootSignature) : m_rootSignature(pRootSignature)
{

}

void Builder::SetRootConstantBufferView(u32 paramIndex, u32 resourceOffset, u32 bufferSize, u32 elementSize, u32 numElements, void* data)
{
    LUZASSERT(m_rootSignature->m_rootParameters[paramIndex].ParameterType == D3D12_ROOT_PARAMETER_TYPE_CBV);

    auto pResource = std::make_shared<UploadBuffer>();
    pResource->SetBufferSize(bufferSize);
    pResource->SetElementSize(elementSize);
    pResource->SetNumElements(numElements);
    m_buildParams.push_back(std::make_shared<CbvBuildParam>(paramIndex, 0, resourceOffset, pResource, data));
}

/*
    // Heeap 1
    SetDescriptorTableEntry(0, 0, 0, "texture1.dds");
    SetDescriptorTableEntry(0, 1, 0, "texture2.dds");
    
    // Heap 2
    SetDescriptorTableEntry(1, 0, 0, "texture3.dds");

*/

void Builder::SetDescriptorTableEntry(u32 paramIndex, u32 rangeIndex, u32 resourceOffset, std::wstring filename)
{
    LUZASSERT(m_rootSignature->m_rootParameters[paramIndex].ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE);
    LUZASSERT(m_rootSignature->m_rootParameters[paramIndex].DescriptorTable.pDescriptorRanges[rangeIndex].RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

    auto pResource = std::make_shared<PixelBuffer>();
    m_buildParams.push_back(std::make_shared<SrvBuildParam>(paramIndex, rangeIndex, resourceOffset, pResource, filename));
}

void Builder::BuildRootConstantBufferViews(std::shared_ptr<const RootSignature> pRootSignature, std::vector<std::shared_ptr<BuildParam>> buildParams, std::shared_ptr<Immutable> out)
{
    for (int i = 0, count = (int)buildParams.size(); i < count; ++i)
    {
        auto& param = std::static_pointer_cast<CbvBuildParam>(buildParams[i]);

        if (pRootSignature->m_rootParameters[param->m_paramIndex].ParameterType != D3D12_ROOT_PARAMETER_TYPE_CBV) continue;

        auto& uploadBuffer = std::static_pointer_cast<UploadBuffer>(param->m_gpuResource);
        if (uploadBuffer->Initialize(param->m_data))
        {
            __debugbreak();
        }

        out->SetRootConstantBufferView(uploadBuffer, param->m_paramIndex, param->m_resourceOffset);
    }
}

void Builder::BuildShaderResourceViewDescriptorTable(std::shared_ptr<const RootSignature> pRootSignature, std::vector<std::shared_ptr<BuildParam>> buildParams, std::shared_ptr<Immutable> out)
{
    u32 numSrv = 0;
    volatile u32 numSrvLoading = 0;

    ResourceManager resMgr;

    for (int i = 0, count = (int)buildParams.size(); i < count; ++i)
    {
        auto& param = std::static_pointer_cast<SrvBuildParam>(buildParams[i]);

        if (pRootSignature->m_rootParameters[param->m_paramIndex].ParameterType != D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) continue;
        if (pRootSignature->m_rootParameters[param->m_paramIndex].DescriptorTable.pDescriptorRanges[param->m_rangeIndex].RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SRV) continue;

        numSrv += 1;
        InterlockedIncrement(&numSrvLoading);
        
        resMgr.LoadResource<Texture2D>(param->m_filename, [&param, &numSrvLoading](std::shared_ptr<const Texture2D> pTexture)
        {
            auto pPixelBuffer = std::static_pointer_cast<PixelBuffer>(param->m_gpuResource);
            if (!pPixelBuffer->InitializeTexture2D(pTexture))
            {
                __debugbreak();
            }

            InterlockedDecrement(&numSrvLoading);
        });
    }

    if (numSrv == 0)
    {
        return;
    }

    while (numSrvLoading != 0) {}

    DescriptorHandle srvHandleStart = Dx12::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, numSrv);

    for (int i = 0, count = (int)buildParams.size(); i < count; ++i)
    {
        auto& param = std::static_pointer_cast<SrvBuildParam>(buildParams[i]);
        if (pRootSignature->m_rootParameters[param->m_paramIndex].ParameterType != D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) continue;
        if (pRootSignature->m_rootParameters[param->m_paramIndex].DescriptorTable.pDescriptorRanges[param->m_rangeIndex].RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SRV) continue;

        std::static_pointer_cast<PixelBuffer>(param->m_gpuResource)->CreateShaderResourceView(srvHandleStart, param->m_resourceOffset);
      //  out->SetRootDescriptorTable(pSrvHeap, param->m_paramIndex, param->m_rangeIndex, param->m_resourceOffset);
    }
}

std::shared_ptr<const Immutable> Builder::ToImmutable()
{
    std::sort(m_buildParams.begin(), m_buildParams.end(), [](const std::shared_ptr<BuildParam>& p0, const std::shared_ptr<BuildParam>& p1) 
    { 
        if (p0->m_paramIndex != p1->m_paramIndex) return p0->m_paramIndex < p1->m_paramIndex;
        if (p0->m_rangeIndex != p1->m_rangeIndex) return p0->m_rangeIndex < p1->m_rangeIndex;
        return p0->m_resourceOffset < p1->m_resourceOffset;
    });

    auto pMaterial = std::make_shared<Immutable>(m_rootSignature);
    BuildRootConstantBufferViews(m_rootSignature, m_buildParams, pMaterial);
    BuildShaderResourceViewDescriptorTable(m_rootSignature, m_buildParams, pMaterial);

    return pMaterial;
}

Immutable::Immutable(std::shared_ptr<const RootSignature> rootSignature) : m_rootSignature(rootSignature)
{

}

Immutable::Immutable() : Immutable(nullptr)
{

}

void Immutable::SetRootConstantBufferView(std::shared_ptr<const UploadBuffer> uploadBuffer, u32 paramIndex, u32 resourceOffset /*= 0*/)
{
    LUZASSERT(m_rootSignature->m_rootParameters[paramIndex].ParameterType == D3D12_ROOT_PARAMETER_TYPE_CBV);
    LUZASSERT(std::find_if(m_gpuResources.begin(), m_gpuResources.end(), [uploadBuffer](const std::shared_ptr<const GpuResource>& pResource) { return pResource == uploadBuffer; }) == m_gpuResources.end());
    
    m_gpuResources.push_back(uploadBuffer);
    m_resourceParams.emplace_back(paramIndex, (u32)m_gpuResources.size() - 1, resourceOffset);
}

void Immutable::SetRootDescriptorTable(const DescriptorHandle& descriptorHandle, u32 paramIndex)
{
    LUZASSERT(m_rootSignature->m_rootParameters[paramIndex].ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE);
    LUZASSERT(std::find_if(m_resourceParams.begin(), m_resourceParams.end(), [paramIndex](const ResourceParam& resourceParam) { return resourceParam.ParamIndex == paramIndex; }) == m_resourceParams.end());
    LUZASSERT(std::find_if(m_descriptorHandles.begin(), m_descriptorHandles.end(), [descriptorHandle](const DescriptorHandle& handle) { return descriptorHandle == handle; }) == m_descriptorHandles.end());

    m_descriptorHandles.push_back(descriptorHandle);
    u32 handleIndex = (u32)m_descriptorHandles.size() - 1;

    m_resourceParams.emplace_back(paramIndex, handleIndex, descriptorHandle.Offset());
}

void Immutable::SetShaderResourceViewTableEntry(std::shared_ptr<const PixelBuffer> pPixelBuffer, u32 paramIndex, u32 rangeIndex /*= 0*/, u32 resourceOffset /*= 0*/)
{
    LUZASSERT(m_rootSignature->m_rootParameters[paramIndex].ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE);
    LUZASSERT(m_rootSignature->m_rootParameters[paramIndex].DescriptorTable.pDescriptorRanges[rangeIndex].RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
    LUZASSERT(std::find_if(m_gpuResources.begin(), m_gpuResources.end(), [pPixelBuffer](const std::shared_ptr<const GpuResource>& pResource) { return pResource == pPixelBuffer; }) == m_gpuResources.end());

    auto srvHandle = pPixelBuffer->ShaderResourceViewHandle();
    auto srvHandleIter = std::find_if(m_descriptorHandles.begin(), m_descriptorHandles.end(), [&srvHandle](const DescriptorHandle& handle) { return srvHandle.Type() == handle.Type() && srvHandle.Index() == handle.Index(); });
    if (srvHandleIter == m_descriptorHandles.end())
    {
        SetRootDescriptorTable(srvHandle, paramIndex);
    }

    auto paramIter = std::find_if(m_resourceParams.begin(), m_resourceParams.end(), [paramIndex](const ResourceParam& resourceParam) { return resourceParam.ParamIndex == paramIndex; });
    LUZASSERT(paramIter != m_resourceParams.end());

    m_gpuResources.push_back(pPixelBuffer);
    u32 resourceIndex = (u32)m_gpuResources.size() - 1;

    paramIter->RangeParams.emplace_back(rangeIndex, resourceIndex, resourceOffset);
}

void Immutable::UpdateConstantBufferView(u32 paramIndex, void* data)
{
    LUZASSERT(m_rootSignature->m_rootParameters[paramIndex].ParameterType == D3D12_ROOT_PARAMETER_TYPE_CBV);

    auto iter = std::find_if(m_resourceParams.begin(), m_resourceParams.end(), [paramIndex](const ResourceParam& resourceParam) { return resourceParam.ParamIndex == paramIndex; });
    if (iter == m_resourceParams.end())
    {
        return;
    }

    auto pResource = std::static_pointer_cast<const UploadBuffer>(m_gpuResources[iter->ResourceIndex]);
    pResource->Map(data);
    pResource->Unmap();
}

void Immutable::Prepare(GraphicsCommandContext* pGraphicsContext)
{
    std::vector<ID3D12DescriptorHeap*> descriptorHeaps;
    Dx12::GetDescriptorHeaps(m_descriptorHandles, descriptorHeaps);

    pGraphicsContext->CommandList()->SetDescriptorHeaps((UINT)descriptorHeaps.size(), descriptorHeaps.data());


    for (auto& resParam : m_resourceParams)
    {
        auto rootParam = m_rootSignature->m_rootParameters[resParam.ParamIndex];

        if (rootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_CBV)
        {
            pGraphicsContext->SetGraphicsRootConstantBufferView(std::static_pointer_cast<const UploadBuffer>(m_gpuResources[resParam.ResourceIndex]).get(), resParam.ParamIndex);
        }
        else if (rootParam.ParameterType == D3D12_DESCRIPTOR_RANGE_TYPE_SRV)
        {
            pGraphicsContext->SetGraphicsRootDescriptorTable(m_descriptorHandles[resParam.ResourceIndex], resParam.ParamIndex);
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
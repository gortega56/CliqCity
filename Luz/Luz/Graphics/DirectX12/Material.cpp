#include "stdafx.h"
#include "Material.h"
#include "RootSignature.h"
#include "CommandContext.h"
#include "DescriptorHeap.h"
#include "GpuResource.h"
#include "Resource\ResourceManager.h"

using namespace dx12;

Material::Material() : m_rootSignature(nullptr)
{

}

Material::~Material()
{

}

void Material::SetTexture2D(const ParamID& paramID, std::shared_ptr<const Texture2D> pTexture)
{
    auto gpuResource = std::make_shared<PixelBuffer>();
    
    // TODO: Initialize buffer with texture data

    SetPixelBuffer(paramID, gpuResource);
}

bool Material::Prepare(GraphicsCommandContext* pCtx)
{
    pCtx->CommandList()->SetGraphicsRootSignature(m_rootSignature->Signature());

    for (auto& kvp : m_descriptorMap)
    {
        pCtx->CommandList()->SetGraphicsRootDescriptorTable((UINT)kvp.first, kvp.second->Native()->GetGPUDescriptorHandleForHeapStart());
    }

    for (auto& kvp : m_pixelBufferMap)
    {
        // TODO: ShaderResourceView
       // pCtx->CommandList()->SetComputeRootDescriptorTable((UINT)kvp.first, kvp.second->sh);
    }

    for (auto& kvp : m_constantBufferMap)
    {
        pCtx->CommandList()->SetGraphicsRootConstantBufferView((UINT)kvp.first, kvp.second->RootConstantBufferView());
    }

    return true;
}
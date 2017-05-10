#include "stdafx.h"
#include "Material.h"
#include "RootSignature.h"
#include "CommandContext.h"
#include "DescriptorHeap.h"
#include "Resource\Texture.h"
#include "dx12_renderer.h"
#include "Resource\ResourceManager.h"

using namespace Dx12;

Material::Material(std::shared_ptr<const Renderer> pRenderer) : m_renderer(pRenderer), m_rootSignature(nullptr)
{

}

Material::~Material()
{

}

void Material::SetTexture2D(const ParamID& paramID, std::shared_ptr<const Texture2D> pTexture)
{
    if (auto renderer = m_renderer.lock())
    {
        auto gpuResource = std::make_shared<PixelBuffer>();

        if (gpuResource->Initialize(renderer->GetCommandQueue(), renderer->GetContext(), (u32)pTexture->Width(), pTexture->Height(), pTexture->GetDXGI(), (void*)pTexture->Data()))
        {
            SetPixelBuffer(paramID, gpuResource);
        }
    }
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
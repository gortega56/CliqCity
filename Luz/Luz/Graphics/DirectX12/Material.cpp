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

bool Material::Finalize()
{
    for (auto& kvp : m_pixelBufferMap)
    {
        auto& paramID = kvp.first;

    }
}

void Material::AppendParam(const ParamKey& key, const Param& param)
{
    // find all root params (unique param index)
    // find all table ranges (same param index, unique table offset)
    // init all root params, descriptor heaps, and resource views
}

Material::Builder& Material::Builder::AddRootConstantBufferView(const u32 paramIndex, const u64 bufferSize, const u32 elementSize, const u32 numElements, void* data)
{
    Param& param = AppendParam();
    param.Index = paramIndex;
    param.Type = ParamType::ROOT_CBV;
    param.AsRootCBV.BufferSize = bufferSize;
    param.AsRootCBV.ElementSize = elementSize;
    param.AsRootCBV.NumElements = numElements;
    param.AsRootCBV.Data = data;
    return *this;
}

Material::Builder& Material::Builder::AddTexture2D(std::string filename, u32 paramIndex)
{
    Param& param = AppendParam();
    param.Index = paramIndex;
    param.Type = ParamType::SRV;
    param.AsTexture.Filename = filename;
    param.AsTexture.Type = TextureType::TEXTURE_2D;
    return *this;
}

Material::Param& Material::Builder::AppendParam()
{
    m_params.emplace_back();
    return m_params.back();
}

std::shared_ptr<const Material> Material::Builder::Finalize(std::shared_ptr<const Renderer> pRenderer)
{
    auto& material = std::make_shared<Material>(pRenderer);
    auto& mutex = std::make_shared<std::mutex>();

    ResourceManager rm;
    for (auto& texture : m_textures)
    {
        std::wstring filename = std::wstring(texture.Filename.begin(), texture.Filename.end());
        rm.LoadResource<Texture2D>(filename, [&mutex, &material](std::shared_ptr<const Texture2D> pTexture)
        {
            std::lock_guard<std::mutex> lock(*mutex);
            material->SetTexture2D((ParamID)1, pTexture);
        });
    }

    auto& pCtx = pRenderer->GetContext();
    for (auto& rootCbv : m_rootCbvs)
    {
        material->SetConstantBuffer()
    }
}
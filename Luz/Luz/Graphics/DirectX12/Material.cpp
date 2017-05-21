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
//
//RootCbvParam::RootCbvParam()
//{
//
//}
//
//RootCbvParam::~RootCbvParam()
//{
//
//}
//
//void RootCbvParam::Finalize(std::shared_ptr<const CommandQueue> pQueue, std::shared_ptr<GraphicsCommandContext> pCtx)
//{
//    UploadBuffer gpuResource;
//    if (gpuResource.Initialize(pCtx, m_data))
//    {
//
//    }
//}
//
//Texture2dParam::Texture2dParam(std::string filename) : m_filename(filename) {}
//Texture2dParam::~Texture2dParam() {}
//
//void Texture2dParam::Finalize(std::shared_ptr<const CommandQueue> pQueue, std::shared_ptr<GraphicsCommandContext> pCtx)
//{
//    ResourceManager rm;
//    rm.LoadResource<Texture2D>(std::wstring(m_filename.begin(), m_filename.end()), [pQueue, pCtx](std::shared_ptr<const Texture2D> pTex)
//    {
//        PixelBuffer gpuResource;
//        if (gpuResource.Initialize(pQueue, pCtx, pTex->Width(), pTex->Height(), pTex->GetDXGI(), (void*)pTex->Data()))
//        {
//            auto& pDev = pCtx->GetDevice();
//
//        }
//    });
//}

Material::Material(std::shared_ptr<const Renderer> pRenderer) : m_renderer(pRenderer), m_rootSignature(nullptr)
{

}

Material::~Material()
{

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
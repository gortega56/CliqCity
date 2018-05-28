#include "MeshApplication.h"
#include "Engine.h"
#include "Resource\ResourceManager.h"
#include "Mesh.h"
#include "DirectX12\GpuResource.h"
#include <functional>
#include <memory>
#include "DirectX12\CommandContext.h"
#include "Resource\Texture.h"
#include "Resource\Fbx.h"
#include "Resource\ObjResource.h"
#include "Resource\MtlResource.h"
#include "Platform\Window.h"
#include "Graphics.h"
#include "Console.h"
#include "Platform\Input.h"
#include <codecvt>
#include <iostream>
#include <sstream>

#define DIFF_PATH0 L".\\Assets\\BrickDiff.dds"
#define DIFF_PATH1 L".\\Assets\\RockPileDiff.dds"

#define NORM_PATH0 L".\\Assets\\BrickNorm.dds"
#define NORM_PATH1 L".\\Assets\\RockPileNorm.dds"

#define FBX_PATH0 ".\\Assets\\/*Prof_Animated*/.fbx"
#define FBX_PATH1 ".\\Assets\\cube.FBX"

#define SPONZA_OBJ_PATH ".\\Assets\\sponza_obj\\sponza.obj"
#define OBJ_PATH ".\\Assets\\cube.obj"

#define SPONZA_MTL_PATH ".\\Assets\\sponza_obj\\sponza.mtl"
#define SPONZA_TEX_PATH ".\\Assets\\sponza_textures\\"

using namespace Luz;
using namespace gmath;
static float g_scale = 5.0f;
Console g_console;

static int FindOrPushBackTextureName(std::vector<std::string>& textureNames, std::string textureName);

int FindOrPushBackTextureName(std::vector<std::string>& textureNames, std::string textureName)
{
    auto iter = std::find(textureNames.begin(), textureNames.end(), textureName);
    if (iter == textureNames.end())
    {
        textureNames.push_back(textureName);
        return (int)textureNames.size() - 1;
    }

    return (int)(iter - textureNames.begin());
}

MeshApplication::MeshApplication()
{
    m_renderableIndex = -1;
}


MeshApplication::~MeshApplication()
{
}

bool MeshApplication::Initialize()
{
    Console::Initialize(&g_console);

    m_window = Window::Create("Mesh Application", 1600, 900, false);

    if (!Graphics::Initialize(m_window.get(), 3))
    {
        return false;
    }

    auto loadingObj = Resource::Async<Resource::Obj>::Load(SPONZA_OBJ_PATH);
    auto loadingMtl = Resource::Async<Resource::Mtl>::Load(SPONZA_MTL_PATH);

    m_cameraController = CameraController(m_engine->OS()->GetInput());

    PerspectiveCamera* pCamera = m_cameraController.GetCamera();
    pCamera->GetTransform()->SetPosition(0.0f, 0.0f, -15.0f);
    pCamera->SetFieldOfView(3.14f * 0.5f);
    pCamera->SetAspectRatio(m_window->AspectRatio());
    pCamera->SetNear(0.1f);
    pCamera->SetFar(3000.0f);

    m_cbvData.view = pCamera->GetView().transpose();
    m_cbvData.proj = pCamera->GetProjection().transpose();

    //rm.LoadResource<Resource::Fbx>(FBX_PATH1, [weakRenderable](std::shared_ptr<const Resource::Fbx> pFbx)
    //{
    //    if (!pFbx) return;

    //    if (auto sharedRenderable = weakRenderable.lock())
    //    {
    //        Mesh<Vertex, u32> mesh;
    //        pFbx->WriteVertices<Vertex>(mesh.Vertices(), [](Vertex& vertex, const Resource::Fbx::Vertex& fbx)
    //        {
    //            vertex.Position = *reinterpret_cast<const float3*>(&fbx.Position[0]);
    //            vertex.Normal = *reinterpret_cast<const float3*>(&fbx.Normal[0]);
    //            vertex.UV = *reinterpret_cast<const float2*>(&fbx.UV[0]);
    //        });

    //        mesh.SetIndices(pFbx->GetIndices());
    //        mesh.GenerateTangents();

    //        sharedRenderable->LoadMesh(&mesh);
    //        sharedRenderable->m_isRenderable.store(true);
    //    }
    //});

    if (!m_vs.InitializeVS(L"VertexShader.hlsl"))
    {
        return false;
    }

    if (!m_ps.InitializePS(L"PixelShader.hlsl"))
    {
        return false;
    }

    InputLayout inputLayout;
    inputLayout
        .AppendFloat4("TANGENT")
        .AppendPosition3F()
        .AppendNormal3F()
        .AppendUV3()
        .Finalize();

    auto range = Dx12::DescriptorTable(2).AppendRangeCBV(26, 1).AppendRangeSRV(36, 0);

    m_rs = std::make_shared<Dx12::RootSignature>(2);
    m_rs->AllowInputLayout()
        .DenyHS()
        .DenyDS()
        .DenyGS()
        .AppendRootCBV(0)
        .AppendRootDescriptorTable(range, D3D12_SHADER_VISIBILITY_PIXEL)
        .AppendAnisotropicWrapSampler(0);

    if (!m_rs->Finalize())
    {
        return false;
    }

    m_pipeline.SetInputLayout(&inputLayout);
    m_pipeline.SetRootSignature(m_rs.get());
    m_pipeline.SetVS(&m_vs);
    m_pipeline.SetPS(&m_ps);
    m_pipeline.SetTriangleTopology();
    m_pipeline.SetSampleCount(1);
    m_pipeline.SetSampleQuality(0);
    m_pipeline.SetSampleMask(0xffffffff);
    m_pipeline.SetDepthStencilState(&DepthStencilState());
    m_pipeline.SetRasterizerState(&RasterizerState()); 
    m_pipeline.SetBlendState(&BlendState());
    m_pipeline.SetRenderTargets();
    
    if (!m_pipeline.Finalize())
    {
        return false;
    }

    //MaterialBuilder matBuilder0(m_rs);
    //matBuilder0.SetRootConstantBufferView(0, 0, sizeof(ConstantBufferData), sizeof(ConstantBufferData), 1, &m_cbvData0);
    //matBuilder0.SetDescriptorTableEntry(1, 0, 0, DIFF_PATH0);
    //matBuilder0.SetDescriptorTableEntry(1, 0, 1, NORM_PATH0);
    //m_material0 = matBuilder0.ToImmutable();
    //

    //MaterialBuilder mb1(m_rs);
    //mb1.SetRootConstantBufferView(0, 0, sizeof(ConstantBufferData), sizeof(ConstantBufferData), 1, &m_cbvData1);
    //mb1.SetDescriptorTableEntry(1, 0, 0, DIFF_PATH1);
    //mb1.SetDescriptorTableEntry(1, 0, 1, NORM_PATH1);
    //m_material1 = mb1.ToImmutable();

    std::vector<Graphics::Mesh<Vertex, u32>> meshes;
    std::vector<std::string> materials;

    std::shared_ptr<const Resource::Obj> pObj = loadingObj.Get();
    std::shared_ptr<const Resource::Mtl> pMtl = loadingMtl.Get();

    if (pObj)
    {
        for (u32 i = 0, numMaterials = pObj->GetNumMaterials(); i < numMaterials; ++i)
        {
            materials.push_back(pObj->GetMaterialName(i));
        }

        meshes.resize(static_cast<size_t>(pObj->GetNumMeshes()));
        pObj->CreateVertices<Vertex, u32>([&meshes](const u32 index, const Vertex* pVertices, u32 numVertices, const u32* pIndices, const u32 numIndices)
        {
            meshes[index].SetVertices(pVertices, numVertices);
            meshes[index].SetIndices(pIndices, numIndices);
        });

        m_renderables.resize(meshes.size());
        for (size_t i = 0; i < meshes.size(); ++i)
        {
            m_materialIndices.push_back(pObj->GetMeshDesc(i).MaterialIndex);
            m_renderables[i] = std::make_shared<Renderable>();
            m_renderables[i]->LoadMesh(&meshes[i]);
            m_renderables[i]->m_isRenderable.store(true);
        }
    }

    std::vector<std::string> textureNames;
    std::string texturePath = SPONZA_TEX_PATH;

    if (pMtl)
    {
        MaterialBuilder builder(m_rs);
        builder.SetRootConstantBufferView(0, 0, sizeof(ConstantBufferData), sizeof(ConstantBufferData), 1, &m_cbvData);

        for (auto& name : materials)
        {
            if (auto pMat = pMtl->GetMaterial(name))
            {
                m_materialConstants.emplace_back();

                auto& mc = m_materialConstants.back();
                mc.SpecularExponent = pMat->SpecularExponent;
                mc.Transparency = pMat->Transparency;
                mc.OpticalDensity = pMat->OpticalDensity;
                mc.Dissolve = pMat->Dissolve;
                mc.Specular = float3(pMat->Specular);
                mc.TransmissionFilter = float3(pMat->TransmissionFilter);
                mc.Ambient = float3(pMat->Ambient);
                mc.Diffuse = float3(pMat->Diffuse);
                mc.Emissive = float3(pMat->Emissive);
                
                if (pMat->DiffuseTextureName.size() != 0)
                {
                    mc.TextureIndices[0] = FindOrPushBackTextureName(textureNames, texturePath + pMat->DiffuseTextureName);
                }

                if (pMat->NormalTextureName.size() != 0)
                {
                    mc.TextureIndices[1] = FindOrPushBackTextureName(textureNames, texturePath + pMat->NormalTextureName);
                }

                if (pMat->DissolveTextureName.size() != 0)
                {
                    mc.TextureIndices[2]= FindOrPushBackTextureName(textureNames, texturePath + pMat->DissolveTextureName);
                }
            }
        }

        for (int i = 0; i < (int)m_materialConstants.size(); ++i)
        {
            builder.SetDescriptorTableEntry(1, 0, i, (u32)sizeof(PhongMaterial), (u32)sizeof(PhongMaterial), 1, &m_materialConstants[i]);
        }

        typedef std::codecvt_utf8<wchar_t> convert_type;
        std::wstring_convert<convert_type, wchar_t> converter;

        for (int i = 0; i < (int)textureNames.size(); ++i)
        {
            builder.SetDescriptorTableEntry(1, 1, i, converter.from_bytes(textureNames[i]));
        }

        m_material = builder.ToImmutable();
    }

    std::stringstream ss;

    for (u32 i = 0, count = pObj->GetNumMeshes(); i < count; ++i)
    {
        auto md = pObj->GetMeshDesc(i);
        
        
        ss << md.Name << std::endl;
        ss << md.MaterialName << std::endl;
        auto pMat = pMtl->GetMaterial(md.MaterialName);
        if (pMat)
        {
            ss << "Diffuse: " << pMat->DiffuseTextureName << std::endl;
            ss << "Normal: " << pMat->NormalTextureName << std::endl;
            ss << "Dissolve" << pMat->DissolveTextureName << std::endl;
        }
        else
        {
            ss << "No material" << std::endl;
        }

        ss << std::endl;
    }

    std::cout << ss.str();
    return true;
}

int MeshApplication::Shutdown()
{
    Graphics::Shutdown();

    return 0;
}

void MeshApplication::Update(double dt)
{
    static int frame = 0;

    auto pCtx = Dx12::GraphicsCommandContext::Create();
    pCtx->Reset(&m_pipeline);

    pCtx->SetRenderContext();
    pCtx->ClearRenderContext();
    pCtx->SetViewport();

    pCtx->SetRootSignature(m_rs.get());
    m_material->UpdateConstantBufferView(0, &m_cbvData);
    m_material->Prepare(pCtx.get());

    if (m_renderableIndex != -1)
    {
        m_renderables[m_renderableIndex]->Prepare(pCtx.get());
        m_renderables[m_renderableIndex]->DrawIndexedInstanced(pCtx.get());
    }
    else
    {
        for (int i = 0, count = (int)m_renderables.size(); i < count; ++i)
        {
            if (m_materialIndices[i] == -1) continue;
            m_renderables[i]->Prepare(pCtx.get());
            m_renderables[i]->DrawIndexedInstanced(pCtx.get());
        }
    }
    
    
    pCtx->Present();
    frame++;

    auto pInput = m_engine->OS()->GetInput();
    if (pInput)
    {
        if (pInput->GetKeyUp(KEYCODE_UP))
        {
            m_renderableIndex += 1;
            if (m_renderableIndex >= (i32)m_renderables.size())
            {
                m_renderableIndex = -1;
            }

            std::cout << "RenderableIndex: " << m_renderableIndex << std::endl;
        }
    }
}

void MeshApplication::FixedUpdate(double dt)
{
    m_cameraController.Update(dt);
    m_cbvData.view = m_cameraController.GetCamera()->GetView().transpose();

    

}
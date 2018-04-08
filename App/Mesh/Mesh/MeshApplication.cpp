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

MeshApplication::MeshApplication()
{
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

    auto range = Dx12::DescriptorTable(1).AppendRangeSRV(2, 0);

    m_rs = std::make_shared<Dx12::RootSignature>(2);
    m_rs->AllowInputLayout()
        .DenyHS()
        .DenyDS()
        .DenyGS()
        .AppendRootCBV(0)
        .AppendRootCBV(1)
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

    

    std::vector<Mesh<Vertex, u32>> meshes;
    std::vector<std::string> materials;
    if (auto pObj = loadingObj.Get())
    {
        pObj->Export(meshes, materials, m_materialIndices);
    
        m_renderables.resize(meshes.size());
        for (size_t i = 0; i < meshes.size(); ++i)
        {
            m_renderables[i] = std::make_shared<Renderable>();
            m_renderables[i]->LoadMesh(&meshes[i]);
            m_renderables[i]->m_isRenderable.store(true);
        }
    }

    auto pMtl = loadingMtl.Get();
    if (pMtl)
    {
        std::string texturePath = SPONZA_TEX_PATH;
        typedef std::codecvt_utf8<wchar_t> convert_type;
        std::wstring_convert<convert_type, wchar_t> converter;

        std::vector<MaterialBuilder> builders;
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

                std::string diffuseTexture = texturePath + pMat->DiffuseTextureName;
                std::string normalTexture = texturePath + pMat->NormalTextureName;

                builders.emplace_back(m_rs);
                
                auto& builder = builders.back();
                builder.SetRootConstantBufferView(0, 0, sizeof(ConstantBufferData), sizeof(ConstantBufferData), 1, &m_cbvData);
                builder.SetRootConstantBufferView(1, 0, sizeof(PhongMaterial), sizeof(PhongMaterial), 1, &mc);
                builder.SetDescriptorTableEntry(2, 0, 0, converter.from_bytes(diffuseTexture));
                builder.SetDescriptorTableEntry(2, 0, 1, converter.from_bytes(normalTexture));
            }
        }

        for (auto& builder : builders)
        {
            m_materials.push_back(builder.ToImmutable());
        }
    }
    

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

    for (int i = 0, count = (int)m_renderables.size(); i < count; ++i)
    {
        i32 mi = m_materialIndices[i];
        if (mi != -1)
        {
            m_materials[mi]->UpdateConstantBufferView(0, &m_cbvData);
            m_materials[mi]->UpdateConstantBufferView(1, &m_materialConstants[mi]);
            m_materials[mi]->Prepare(pCtx.get());
        }
        
        m_renderables[i]->Prepare(pCtx.get());
        m_renderables[i]->DrawIndexedInstanced(pCtx.get());
    }
    
    pCtx->Present();
    frame++;
}

void MeshApplication::FixedUpdate(double dt)
{
    m_cameraController.Update(dt);
    m_cbvData.view = m_cameraController.GetCamera()->GetView().transpose();
}
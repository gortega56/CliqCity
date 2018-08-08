#include "MeshApplication.h"
#include "Engine.h"
#include "Resource\ResourceManager.h"
#include "Mesh.h"
#include <functional>
#include <memory>
//#include "Resource\Fbx.h"
#include "Resource\ObjResource.h"
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
#define SPONZA_MTL_PATH ".\\Assets\\sponza_obj\\sponza.mtl"
#define SPONZA_TEX_PATH ".\\Assets\\sponza_textures\\"

using namespace Luz;
using namespace lina;
static float g_scale = 5.0f;
Console g_console;


static OrthographicCamera s_lighting = OrthographicCamera(1500.0f, 1500.0f, 0.1f, 3000.0f);
static Light s_light = Light(float3(0.8f, 0.8f, 0.8f), float3(0.0f, -0.5f, 0.0f));

static bool s_renderShadows = true;
static bool s_shadowFullScreen = false;
static float s_lightMoveSpeed = 0.2f;

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

    Resource::Obj::Desc desc;
    desc.Filename = SPONZA_OBJ_PATH;
    desc.Directory = ".\\Assets\\sponza_obj\\";
    desc.TextureDirectory = ".\\Assets\\sponza_textures\\";
    desc.InvertUVs = true;
    auto loadingObj = Resource::Async<Resource::Obj>::Load(desc);

    m_cameraController = CameraController(m_engine->OS()->GetInput());

    PerspectiveCamera* pCamera = m_cameraController.GetCamera();
    pCamera->GetTransform()->SetPosition(0.0f, 0.0f, -15.0f);
    pCamera->SetFieldOfView(3.14f * 0.5f);
    pCamera->SetAspectRatio(m_window->AspectRatio());
    pCamera->SetNear(0.1f);
    pCamera->SetFar(3000.0f);

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

    m_vs = Graphics::CreateVertexShader("VertexShader.hlsl");
    m_ps = Graphics::CreatePixelShader("PixelShader.hlsl");

    m_fs_vs = Graphics::CreateVertexShader("FS_Tri_VS.hlsl");
    m_fs_ps = Graphics::CreatePixelShader("FS_Tri_PS.hlsl");

    Graphics::PipelineDesc pd;
    pd.Signature.SetName("Opaque")
        .AllowInputLayout()
        .DenyHS()
        .DenyDS()
        .DenyGS()
        .AppendConstantView(0)
        .AppendConstantView(1)
        .AppendConstantView(2)
        .AppendDescriptorTable(Graphics::SHADER_VISIBILITY_ALL)
        .AppendDescriptorTableRange(3, 26, 3, 0, Graphics::DescriptorTable::Range::DESCRIPTOR_TABLE_RANGE_TYPE_CONSTANT_VIEW)   // Array of CBVs
        .AppendDescriptorTableRange(3, 52, 0, 0, Graphics::DescriptorTable::Range::DESCRIPTOR_TABLE_RANGE_TYPE_SHADER_VIEW)     // Array of SRVs
        .AppendDescriptorTableRange(3, 1, 0, 1, Graphics::DescriptorTable::Range::DESCRIPTOR_TABLE_RANGE_TYPE_SHADER_VIEW)
        .AppendAnisotropicWrapSampler(0)
        .AppendComparisonPointBorderSampler(1);
    pd.InputLayout.AppendFloat4("TANGENT")
        .AppendPosition3F()
        .AppendNormal3F()
        .AppendUV3();
    pd.VertexShaderHandle = m_vs;
    pd.PixelShaderHandle = m_ps;
    pd.Topology = Graphics::GFX_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pd.SampleCount = 1;
    pd.SampleQuality = 0;
    pd.SampleMask = 0xffffffff;

    pd.DepthStencil.DepthEnable = true;
    pd.DepthStencil.WriteMask = Graphics::GFX_DEPTH_WRITE_MASK_ALL;
    pd.DepthStencil.Comparison = Graphics::COMPARISON_TYPE_LESS;
    pd.DepthStencil.StencilReadMask = 0xff;
    pd.DepthStencil.StencilWriteMask = 0xff;
    pd.DepthStencil.StencilEnable = false;
    pd.DepthStencil.FrontFace.StencilFailOp = Graphics::GFX_STENCIL_OP_KEEP;
    pd.DepthStencil.FrontFace.StencilDepthFailOp = Graphics::GFX_STENCIL_OP_KEEP;
    pd.DepthStencil.FrontFace.StencilPassOp = Graphics::GFX_STENCIL_OP_KEEP;
    pd.DepthStencil.FrontFace.Comparison = Graphics::COMPARISON_TYPE_ALWAYS;
    pd.DepthStencil.BackFace.StencilFailOp = Graphics::GFX_STENCIL_OP_KEEP;
    pd.DepthStencil.BackFace.StencilDepthFailOp = Graphics::GFX_STENCIL_OP_KEEP;
    pd.DepthStencil.BackFace.StencilPassOp = Graphics::GFX_STENCIL_OP_KEEP;
    pd.DepthStencil.BackFace.Comparison = Graphics::COMPARISON_TYPE_ALWAYS;

    pd.Rasterizer.Fill = Graphics::GFX_FILL_MODE_SOLID;
    pd.Rasterizer.Cull = Graphics::GFX_CULL_MODE_BACK;
    pd.Rasterizer.FrontCounterClockwise = false;
    pd.Rasterizer.DepthBias = 0;
    pd.Rasterizer.DepthBiasClamp = 0.0f;
    pd.Rasterizer.SlopeScaledDepthBias = 0.0f;
    pd.Rasterizer.DepthClipEnable = true;
    pd.Rasterizer.MsaaEnable = false;
    pd.Rasterizer.AntialiasedLineEnable = false;
    pd.Rasterizer.RasterizationMode = Graphics::GFX_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    pd.Blend.AlphaToCoverageEnable = false;
    pd.Blend.IndependentBlendEnable = false;
    pd.Blend.BlendStates[0].BlendEnable = false;
    pd.Blend.BlendStates[0].LogicOpEnable = false;
    pd.Blend.BlendStates[0].SrcBlend = Graphics::GFX_BLEND_ONE;
    pd.Blend.BlendStates[0].DestBlend = Graphics::GFX_BLEND_ZERO;
    pd.Blend.BlendStates[0].BlendOp = Graphics::GFX_BLEND_OP_ADD;
    pd.Blend.BlendStates[0].SrcBlendAlpha = Graphics::GFX_BLEND_ONE;
    pd.Blend.BlendStates[0].DestBlendAlpha = Graphics::GFX_BLEND_ZERO;
    pd.Blend.BlendStates[0].BlendOpAlpha = Graphics::GFX_BLEND_OP_ADD;
    pd.Blend.BlendStates[0].LogicOp = Graphics::GFX_LOGIC_OP_NOOP;
    pd.Blend.BlendStates[0].RenderTargetWriteMask = Graphics::GFX_COLOR_WRITE_ENABLE_ALL;

    pd.UseSwapChain = true;

    m_opaquePipeline = Graphics::CreateGraphicsPipelineState(pd);

    std::vector<Graphics::Mesh<Vertex, u32>> meshes;
    std::vector<std::string> textureNames;

    std::shared_ptr<const Resource::Obj> pObj = loadingObj.Get();
    if (pObj)
    {
        // Get bounds
        Resource::Obj::BoundingBox boundingBox = pObj->GetSceneBounds();
        m_sceneBounds.max.x = boundingBox.MaxX;
        m_sceneBounds.max.y = boundingBox.MaxY;
        m_sceneBounds.max.z = boundingBox.MaxZ;
        m_sceneBounds.min.x = boundingBox.MinX;
        m_sceneBounds.min.y = boundingBox.MinY;
        m_sceneBounds.min.z = boundingBox.MinZ;

        float3 dim = m_sceneBounds.max - m_sceneBounds.min;

        s_lighting.SetWidth(dim.x);
        s_lighting.SetHeight(dim.y);
        s_lighting.SetFar(3000.0f);
        s_lighting.SetNear(0.1f);

        float3 lightPos = dim * -normalize(s_light.Direction);
        float3x3 lightView = float3x3::orientation_lh(s_light.Direction, float3(0.0f, 0.0f, 1.0f));
        s_lighting.GetTransform()->SetPosition(lightPos);
        s_lighting.GetTransform()->SetRotation(quaternion::create(lightView));

        m_shadowCbvData.view = s_lighting.GetView();
        m_shadowCbvData.proj = s_lighting.GetProjection();
        m_shadowCbvData.inverseView = m_shadowCbvData.view.inverse().transpose();
        m_shadowCbvData.inverseProj = m_shadowCbvData.proj.inverse().transpose();

        // Create geo
        meshes.resize(static_cast<size_t>(pObj->GetNumSurfaces()));
        pObj->CreateStructuredSurfaces<Vertex, u32>([&](const u32 index, const Resource::Obj::StructuredSurface<Vertex, u32>& desc)
        {
            meshes[index].SetVertices(desc.VerticesPtr, desc.NumVertices);
            meshes[index].SetIndices(desc.IndicesPtr, desc.NumIndices);
            m_materialIndices.push_back(desc.MaterialHandle);
        });

        // Create mats
        for (u32 i = 0, numMaterials = pObj->GetNumMaterials(); i < numMaterials; ++i)
        {
            auto md = pObj->GetMaterialDesc(i);
            m_materialConstants.emplace_back();
            auto& mc = m_materialConstants.back();
            mc.SpecularExponent = md.SpecularExponent;
            mc.Transparency = md.Transparency;
            mc.OpticalDensity = md.OpticalDensity;
            mc.Dissolve = md.Dissolve;
            mc.Specular = float3(md.Specular[0], md.Specular[1], md.Specular[2]);
            mc.TransmissionFilter = float3(md.TransmissionFilter[0], md.TransmissionFilter[1], md.TransmissionFilter[2]);
            mc.Ambient = float3(md.Ambient[0], md.Ambient[1], md.Ambient[2]);
            mc.Diffuse = float3(md.Diffuse[0], md.Diffuse[1], md.Diffuse[2]);
            mc.Emissive = float3(md.Emissive[0], md.Emissive[1], md.Emissive[2]);

            if (strlen(md.DiffuseTextureName)) mc.TextureIndices[0] = FindOrPushBackTextureName(textureNames, md.DiffuseTextureName);
            if (strlen(md.NormalTextureName)) mc.TextureIndices[1] = FindOrPushBackTextureName(textureNames, md.NormalTextureName);
            if (strlen(md.DissolveTextureName)) mc.TextureIndices[2] = FindOrPushBackTextureName(textureNames, md.DissolveTextureName);
            if (strlen(md.SpecularTextureName)) mc.TextureIndices[3] = FindOrPushBackTextureName(textureNames, md.SpecularTextureName);
        }
    }

    m_surfaces.resize(meshes.size());
    for (u32 i = 0, count = static_cast<u32>(meshes.size()); i < count; ++i)
    {
        auto& mesh = meshes[i];
        
        Graphics::BufferDesc vbd;
        vbd.Alignment = 0;
        vbd.SizeInBytes = mesh.NumVertexBytes();
        vbd.StrideInBytes = static_cast<u16>(mesh.VertexStride());
        vbd.pData = mesh.Vertices();

        Graphics::BufferDesc ibd;
        ibd.Alignment = 0;
        ibd.SizeInBytes = mesh.NumIndexBytes();
        ibd.StrideInBytes = mesh.IndexStride();
        ibd.pData = mesh.Indices();

        auto& surface = m_surfaces[i];
        surface.vb = Graphics::CreateVertexBuffer(vbd);
        surface.ib = Graphics::CreateIndexBuffer(ibd);
       // surface.isReady = true;
    }

    Graphics::ConstantBufferDesc cbd;
    cbd.Alignment = 0;
    cbd.SizeInBytes = sizeof(ConstantBufferData);
    cbd.StrideInBytes = sizeof(ConstantBufferData);
    cbd.AllocHeap = false;
    cbd.pData = &m_cbvData;
    m_viewProjectionHandle = Graphics::CreateConstantBuffer(cbd);

    m_lightViewProjHandle = Graphics::CreateConstantBuffer(cbd);

    cbd.SizeInBytes = sizeof(Light);// (sizeof(Light) + 255) & ~255;
    cbd.StrideInBytes = sizeof(Light);// (sizeof(Light) + 255) & ~255;
    cbd.AllocHeap = false;
    cbd.pData = &s_light;
    m_lightHandle = Graphics::CreateConstantBuffer(cbd);

    cbd.SizeInBytes = sizeof(PhongMaterial);
    cbd.StrideInBytes = sizeof(PhongMaterial);
    cbd.AllocHeap = true;
    for (u32 i = 0, count = static_cast<u32>(m_materialConstants.size()); i < count; ++i)
    {
        cbd.pData = &m_materialConstants[i];
        Graphics::ConstantBufferHandle cbHandle = Graphics::CreateConstantBuffer(cbd);
        if (i == 0) m_baseDescriptorHandle = cbHandle;
    }

    for (u32 i = 0, count = static_cast<u32>(textureNames.size()); i < count; ++i)
    {
        Graphics::TextureFileDesc td;
        td.Filename = textureNames[i].c_str();
        td.GenMips = true;
        Graphics::CreateTexture(td);
    }
    
    static u32 s_ib[] = { 0, 1, 2 };
    Graphics::BufferDesc ib;
    ib.Alignment = 0;
    ib.SizeInBytes = 12;
    ib.StrideInBytes = sizeof(u32);
    ib.pData = s_ib;
    m_fs_ib = Graphics::CreateIndexBuffer(ib);

    Graphics::DepthStencilDesc ds;
    ds.ClearDepth = 1.0f;
    ds.ClearStencil = 0;
    ds.Flags = Graphics::GFX_DEPTH_STENCIL_FLAG_SHADER_RESOURCE;
    ds.Format = Graphics::GFX_FORMAT_R32_TYPELESS;
    ds.Width = 2048;
    ds.Height = 2048;
    m_shadowTexture = Graphics::CreateDepthStencil(ds);

    memset(&pd.Signature, 0, sizeof(Graphics::SignatureDesc));
    pd.Signature.SetName("shadow")
        .AllowInputLayout()
        .DenyHS()
        .DenyDS()
        .DenyGS()
        .AppendConstantView(0);
    pd.VertexShaderHandle = m_vs;
    pd.PixelShaderHandle = Graphics::GPU_RESOURCE_HANDLE_INVALID;
    pd.NumRenderTargets = 0;
    pd.pRenderTargets = nullptr;
    pd.DsHandle = m_shadowTexture;
    pd.UseSwapChain = false;

    pd.Rasterizer.DepthBias = 100000;
    pd.Rasterizer.DepthBiasClamp = 0.0f;
    pd.Rasterizer.SlopeScaledDepthBias = 1.0f;

    m_shadowPipeline = Graphics::CreateGraphicsPipelineState(pd);

    memset(&pd.Signature, 0, sizeof(Graphics::SignatureDesc));
    pd.Signature.SetName("fullscreen")
        .AllowInputLayout()
        .DenyHS()
        .DenyDS()
        .DenyGS()
        .AppendDescriptorTable(Graphics::SHADER_VISIBILITY_PS)
        .AppendDescriptorTableRange(0, 1, 1, 0, Graphics::DescriptorTable::Range::DESCRIPTOR_TABLE_RANGE_TYPE_SHADER_VIEW)
        .AppendAnisotropicWrapSampler(0);
    pd.VertexShaderHandle = m_fs_vs;
    pd.PixelShaderHandle = m_fs_ps;
    pd.UseSwapChain = true;
    m_fullScreenPipeline = Graphics::CreateGraphicsPipelineState(pd);

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
    static const float clear[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
    static const Graphics::Viewport vp = { 0.0f, 0.0f, 1600.0f, 900.0f, 0.0f, 1.0f };
    static const Graphics::Rect scissor = { 0, 0, 1600, 900 };
    
    // Shadow 
    if (s_renderShadows)
    {
        static const Graphics::Viewport s_shadow_vp = { 0.0f, 0.0f, 2048.0f, 2048.0f, 0.0f, 1.0f };
        static const Graphics::Rect s_shadow_scissor = { 0, 0, 2048, 2048 };

        Graphics::UpdateConstantBuffer(&m_shadowCbvData, sizeof(m_shadowCbvData), m_lightViewProjHandle);

        Graphics::CommandStreamDesc csd;
        csd.PipelineHandle = m_shadowPipeline;
        csd.QueueType = Graphics::GFX_COMMAND_QUEUE_TYPE_MAIN;

        Graphics::CommandStream cs;
        Graphics::CreateCommandStream(csd, &cs);
        cs.TransitionDepthStencilToDepthWrite(m_shadowTexture);
        cs.SetPipeline(m_shadowPipeline);
        cs.SetRenderTargets(0, nullptr, m_shadowTexture);
        cs.ClearDepthStencil(1.0f, 0, m_shadowTexture);
        cs.SetViewport(s_shadow_vp);
        cs.SetScissorRect(s_shadow_scissor);
        cs.SetConstantBuffer(0, m_lightViewProjHandle);
        // TODO: Descriptor heaps
        // TODO: Descriptor handles

        cs.SetPrimitiveTopology(Graphics::GFX_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        if (m_renderableIndex != -1)
        {
            auto& surface = m_surfaces[m_renderableIndex];
            cs.SetVertexBuffer(surface.vb);
            cs.SetIndexBuffer(surface.ib);
            cs.DrawInstanceIndexed(surface.ib);
        }
        else
        {
            for (u32 i = 0, count = (u32)m_surfaces.size(); i < count; ++i)
            {
                if (m_materialIndices[i] == -1) continue;
                auto& surface = m_surfaces[i];
                cs.SetVertexBuffer(surface.vb);
                cs.SetIndexBuffer(surface.ib);
                cs.DrawInstanceIndexed(surface.ib);
            }
        }

        Graphics::SubmitCommandStream(&cs, false);
    }
    
    if (s_shadowFullScreen)
    {
        Graphics::CommandStreamDesc csd;
        csd.PipelineHandle = m_fullScreenPipeline;
        csd.QueueType = Graphics::GFX_COMMAND_QUEUE_TYPE_MAIN;

        Graphics::CommandStream cs;
        Graphics::CreateCommandStream(csd, &cs);
        cs.TransitionDepthStencilToTexture(m_shadowTexture);
        cs.SetPipeline(m_fullScreenPipeline);
        cs.SetRenderTargets();
        cs.ClearRenderTarget(clear);
        cs.ClearDepthStencil(1.0f, 0);
        cs.SetViewport(vp);
        cs.SetScissorRect(scissor);
        cs.SetDescriptorTable_FixLater(0, m_baseDescriptorHandle, m_shadowTexture);

        cs.SetPrimitiveTopology(Graphics::GFX_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cs.SetVertexBuffer(Graphics::GPU_RESOURCE_HANDLE_INVALID);
        cs.SetIndexBuffer(m_fs_ib);
        cs.DrawInstanceIndexed(m_fs_ib);

        Graphics::SubmitCommandStream(&cs, false);
    }
    else
    {
        // Main visual
        Graphics::UpdateConstantBuffer(&m_cbvData, sizeof(m_cbvData), m_viewProjectionHandle);
        Graphics::UpdateConstantBuffer(&s_light, sizeof(s_light), m_lightHandle);

        Graphics::CommandStreamDesc csd;
        csd.PipelineHandle = m_opaquePipeline;
        csd.QueueType = Graphics::GFX_COMMAND_QUEUE_TYPE_MAIN;

        Graphics::CommandStream cs;
        Graphics::CreateCommandStream(csd, &cs);

        cs.TransitionDepthStencilToTexture(m_shadowTexture);
        cs.SetPipeline(m_opaquePipeline);
        cs.SetRenderTargets();
        cs.ClearRenderTarget(clear);
        cs.ClearDepthStencil(1.0f, 0);
        cs.SetViewport(vp);
        cs.SetScissorRect(scissor);
        cs.SetConstantBuffer(0, m_viewProjectionHandle);
        cs.SetConstantBuffer(1, m_lightViewProjHandle);
        cs.SetConstantBuffer(2, m_lightHandle);
        cs.SetDescriptorTable(3, m_baseDescriptorHandle);
        // TODO: Descriptor heaps
        // TODO: Descriptor handles

        cs.SetPrimitiveTopology(Graphics::GFX_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        if (m_renderableIndex != -1)
        {
            auto& surface = m_surfaces[m_renderableIndex];
            cs.SetVertexBuffer(surface.vb);
            cs.SetIndexBuffer(surface.ib);
            cs.DrawInstanceIndexed(surface.ib);
        }
        else
        {
            for (u32 i = 0, count = (u32)m_surfaces.size(); i < count; ++i)
            {
                if (m_materialIndices[i] == -1) continue;
                auto& surface = m_surfaces[i];
                cs.SetVertexBuffer(surface.vb);
                cs.SetIndexBuffer(surface.ib);
                cs.DrawInstanceIndexed(surface.ib);
            }
        }

        Graphics::SubmitCommandStream(&cs, false);
    }
        
    Graphics::Present();

    frame++;

    auto pInput = m_engine->OS()->GetInput();
    if (pInput)
    {
        if (pInput->GetKeyUp(KEYCODE_UP))
        {
            m_renderableIndex += 1;
            if (m_renderableIndex >= (i32)m_surfaces.size())
            {
                m_renderableIndex = -1;
            }

            std::cout << "RenderableIndex: " << m_renderableIndex << std::endl;
        }

        if (pInput->GetKey(KEYCODE_I)) s_lighting.GetTransform()->MoveForward(1);
        if (pInput->GetKey(KEYCODE_J)) s_lighting.GetTransform()->MoveRight(-1);
        if (pInput->GetKey(KEYCODE_K)) s_lighting.GetTransform()->MoveRight(1);
        if (pInput->GetKey(KEYCODE_L)) s_lighting.GetTransform()->MoveForward(-1);

        if (pInput->GetKey(KEYCODE_V)) s_lighting.SetWidth(s_lighting.GetWidth() + 1);
        if (pInput->GetKey(KEYCODE_B)) s_lighting.SetWidth(s_lighting.GetWidth() - 1);
        if (pInput->GetKey(KEYCODE_G)) s_lighting.SetHeight(s_lighting.GetHeight() + 1);
        if (pInput->GetKey(KEYCODE_H)) s_lighting.SetHeight(s_lighting.GetHeight() - 1);
    }
}

void MeshApplication::FixedUpdate(double dt)
{
    m_cameraController.Update(dt);

    float4x4 view = m_cameraController.GetCamera()->GetView();
    float4x4 proj = m_cameraController.GetCamera()->GetProjection();

    m_cbvData.view = view.transpose();
    m_cbvData.proj = proj.transpose();
    m_cbvData.inverseView = view.inverse().transpose();
    m_cbvData.inverseProj = proj.inverse().transpose();

    static double s_time = 0.0;
    static float s_pi = 3.14159265359f;
    static float s_minTheta = s_pi * 0.25f;     // 45
    static float s_maxTheta = s_pi * 0.75f;    // 135
    static float s_2pi = s_pi * 2.0f;           
    static float s_minPhi = s_2pi * 0.25f;    // 45
    static float s_maxPhi = s_2pi * 0.35f;      // 135

    float phi_t = sinf((float)s_time * s_lightMoveSpeed) * 0.5f + 0.5f;
    float phi = (1.0f - phi_t) * s_minPhi + s_maxPhi * phi_t;
    
    float theta_t = phi_t * 0.5f + 0.5f;
    float theta = (1.0f - theta_t) * s_minTheta + s_maxTheta * theta_t;

    auto pTransform = s_lighting.GetTransform();
    auto range = m_sceneBounds.max - m_sceneBounds.min;
    auto center = (m_sceneBounds.max + m_sceneBounds.min) * 0.5f;
    auto radius = lina::length(float2(range.x, range.y)) * 0.5f;
    float3 position =
    {
        center.x + radius * sinf(theta) * cosf(phi),
        center.y + radius * sinf(theta) * sinf(phi),
        center.z + radius * cosf(theta)
    };

    pTransform->SetPosition(position);

    auto direction = lina::normalize(center - position);
    auto frame = float3x3::orientation_lh(direction, float3(0.0f, 0.0f, 1.0f));
    pTransform->SetRotation(quat::create(frame));

    s_light.Direction = direction;

    view = s_lighting.GetView();
    proj = s_lighting.GetProjection();

    m_shadowCbvData.view = view.transpose();
    m_shadowCbvData.proj = proj.transpose();
    m_shadowCbvData.inverseView = view.inverse().transpose();
    m_shadowCbvData.inverseProj = proj.inverse().transpose();

    s_time += dt;
}
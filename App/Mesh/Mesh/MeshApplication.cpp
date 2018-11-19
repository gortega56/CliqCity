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
#include "CommandStream.h"
#include "Platform\Input.h"
#include <codecvt>
#include <iostream>
#include <sstream>

#define CRYTEK_SPONZA_DIRECTORY ".\\Assets\\crytek_sponza\\"
#define CRYTEK_SPONZA_OBJ_PATH  ".\\Assets\\crytek_sponza\\sponza.obj"
#define CRYTEK_SPONZA_TEXTURE_DIRECTORY ".\\Assets\\crytek_sponza\\textures"

#define DABROVIC_SPONZA_DIRECTORY ".\\Assets\\dabrovic_sponza\\"
#define DABROVIC_SPONZA_OBJ_PATH  ".\\Assets\\dabrovic_sponza\\sponza.obj"
#define DABROVIC_SPONZA_TEXTURE_DIRECTORY DABROVIC_SPONZA_DIRECTORY

using namespace Luz;
using namespace lina;

struct ShaderOptions
{
    float3 LightColor;
    float3 LightDirection;
    float4 LightIntensity;
    float Exposure;
    bool AmbientEnabled;
    bool DiffuseEnabled;
    bool SpecEnabled;
    bool BumpEnabled;
    bool ShadowEnabled;
};

static ShaderOptions s_shaderOptions =
{
    float3(0.8f, 0.8f, 0.8f),
    float3(0.0f, -0.5f, -0.1f),
    float4(0.2f, 10.0f, 100.0f, 0.0f),
    10.0f,
    true,
    true,
    true,
    true,
    true
};

static std::mutex s_shaderOptionMutex;

static OrthographicCamera s_lighting = OrthographicCamera(1500.0f, 1500.0f, 0.1f, 3000.0f);

static float s_lightMoveSpeed = 0.2f;

static bool s_shadowFullScreen = false;

static std::thread s_consoleThread;

static std::atomic_bool s_loading = ATOMIC_VAR_INIT(1);

static std::atomic_bool s_consoleWritten = ATOMIC_VAR_INIT(0);

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

static void ConsoleThread()
{
    Platform::CreateConsole();

    ShaderOptions shaderOptions = s_shaderOptions;

    int iter = 0, nDots = 3;
    while (s_loading.load())
    {
        Platform::ClearConsole();
        iter = (iter + 1) % nDots;

        switch (iter)
        {
        case 0: std::cout << "Loading." << std::endl; break;
        case 1: std::cout << "Loading.." << std::endl; break;
        case 2: std::cout << "Loading..." << std::endl; break;
        default: LUZASSERT(false);
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    while (Platform::Running())
    {
        std::cout << ">: ";
       
        std::string input;
        getline(std::cin, input);

        std::stringstream ss = std::stringstream(input);
        
        std::string cmd;
        ss >> cmd;
        if (strcmp(cmd.c_str(), "exit") == 0)
        {
            break;
        }
        else if (strcmp(cmd.c_str(), "set_exposure") == 0)
        {
            ss >> shaderOptions.Exposure;
        } 
        else if (strcmp(cmd.c_str(), "set_ambient") == 0)
        {
            ss >> shaderOptions.LightIntensity.x;
        }
        else if (strcmp(cmd.c_str(), "set_diffuse") == 0)
        {
            ss >> shaderOptions.LightIntensity.y;
        }
        else if (strcmp(cmd.c_str(), "set_spec") == 0)
        {
            ss >> shaderOptions.LightIntensity.z;
        }
        else if (strcmp(cmd.c_str(), "toggle_ambient") == 0)
        {
            shaderOptions.AmbientEnabled = !shaderOptions.AmbientEnabled;
        }
        else if (strcmp(cmd.c_str(), "toggle_diffuse") == 0)
        {
            shaderOptions.DiffuseEnabled = !shaderOptions.DiffuseEnabled;
        }
        else if (strcmp(cmd.c_str(), "toggle_spec") == 0)
        {
            shaderOptions.SpecEnabled = !shaderOptions.SpecEnabled;
        }
        else if (strcmp(cmd.c_str(), "toggle_shadow") == 0)
        {
            shaderOptions.ShadowEnabled = !shaderOptions.ShadowEnabled;
        }
        else if (strcmp(cmd.c_str(), "toggle_bump") == 0)
        {
            shaderOptions.BumpEnabled = !shaderOptions.BumpEnabled;
        }
        else
        {
            continue;
        }

        std::lock_guard<std::mutex> lock(s_shaderOptionMutex);
        s_shaderOptions = shaderOptions;
    }
}

MeshApplication::MeshApplication()
    : m_window(nullptr)
    , m_vs(0)
    , m_ps(0)
    , m_fs_vs(0)
    , m_fs_ps(0)
    , m_fs_ib(0)
    , m_opaquePipeline(0)
    , m_shadowPipeline(0)
    , m_fullScreenPipeline(0)
    , m_baseDescriptorHandle(0)
    , m_frameIndex(0)
{

}

MeshApplication::~MeshApplication()
{

}

bool MeshApplication::Initialize()
{
    s_consoleThread = std::thread(ConsoleThread);

    m_window = Window::Create("Mesh Application", 1600, 900, false);

    m_frameIndex = 0;

    if (!Graphics::Initialize(m_window.get(), s_nSwapChainTargets))
    {
        return false;
    }

    Resource::Obj::Desc desc;
    desc.Filename = CRYTEK_SPONZA_OBJ_PATH;
    desc.Directory = CRYTEK_SPONZA_DIRECTORY;
    desc.TextureDirectory = CRYTEK_SPONZA_DIRECTORY;
    desc.InvertUVs = true;
    auto loadingObj = Resource::Async<Resource::Obj>::Load(desc);

    m_cameraController = CameraController();

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

    static u32 s_ib[] = { 0, 1, 2 };
    Graphics::BufferDesc ib;
    ib.Alignment = 0;
    ib.SizeInBytes = 12;
    ib.StrideInBytes = sizeof(u32);
    ib.pData = s_ib;
    m_fs_ib = Graphics::CreateIndexBuffer(ib);

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
        .AppendDescriptorTableRange(3, 25, 3, 0, Graphics::DescriptorTable::Range::DESCRIPTOR_TABLE_RANGE_TYPE_CONSTANT_VIEW)   // Array of CBVs
        .AppendDescriptorTableRange(3, 38, 0, 0, Graphics::DescriptorTable::Range::DESCRIPTOR_TABLE_RANGE_TYPE_SHADER_VIEW)     // Array of SRVs
        .AppendDescriptorTableRange(3, 1, 0, 1, Graphics::DescriptorTable::Range::DESCRIPTOR_TABLE_RANGE_TYPE_SHADER_VIEW)
        .AppendAnisotropicWrapSampler(0)
        .AppendComparisonPointBorderSampler(1)
        .AppendWrapSampler(2, Graphics::GFX_FILTER_MIN_MAG_LINEAR_MIP_POINT);
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

    pd.DepthStencil = Graphics::DepthStencilState::DepthCompareLessWriteAll_StencilOff;
    pd.Rasterizer = Graphics::RasterizerState::FillSolidCullCCW;
    pd.Blend.BlendStates[0] = Graphics::RenderTargetBlendState::Replace;
    pd.Blend.AlphaToCoverageEnable = false;
    pd.Blend.IndependentBlendEnable = false;

    pd.UseSwapChain = true;

    m_opaquePipeline = Graphics::CreateGraphicsPipelineState(pd);

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

        float3 lightPos = dim * -normalize(s_shaderOptions.LightDirection);
        float3x3 lightView = float3x3::orientation_lh(s_shaderOptions.LightDirection, float3(0.0f, 0.0f, 1.0f));
        s_lighting.GetTransform()->SetPosition(lightPos);
        s_lighting.GetTransform()->SetRotation(quaternion::create(lightView));

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
            if (strlen(md.BumpTextureName)) mc.TextureIndices[1] = FindOrPushBackTextureName(textureNames, md.BumpTextureName);
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
    cbd.SizeInBytes = sizeof(CameraConstants);
    cbd.StrideInBytes = sizeof(CameraConstants);
    cbd.AllocHeap = false;
    cbd.pData = nullptr;// &m_frameConsts[0].CameraConsts;
    m_frameConsts[0].hCamera = Graphics::CreateConstantBuffer(cbd);
    m_frameConsts[1].hCamera = Graphics::CreateConstantBuffer(cbd);
    m_frameConsts[2].hCamera = Graphics::CreateConstantBuffer(cbd);
    m_frameConsts[0].hShadow = Graphics::CreateConstantBuffer(cbd);
    m_frameConsts[1].hShadow = Graphics::CreateConstantBuffer(cbd);
    m_frameConsts[2].hShadow = Graphics::CreateConstantBuffer(cbd);

    cbd.SizeInBytes = sizeof(LightingConstants);
    cbd.SizeInBytes = sizeof(LightingConstants);
    m_frameConsts[0].hLighting = Graphics::CreateConstantBuffer(cbd);
    m_frameConsts[1].hLighting = Graphics::CreateConstantBuffer(cbd);
    m_frameConsts[2].hLighting = Graphics::CreateConstantBuffer(cbd);

    cbd.SizeInBytes = sizeof(MaterialConstants);
    cbd.StrideInBytes = sizeof(MaterialConstants);
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
    pd.PixelShaderHandle = 0;
    pd.NumRenderTargets = 0;
    pd.pRenderTargets = nullptr;
    pd.DsHandle = m_shadowTexture;
    pd.UseSwapChain = false;

    pd.Rasterizer.DepthBias = 100000;
    pd.Rasterizer.DepthBiasClamp = 0.0f;
    pd.Rasterizer.SlopeScaledDepthBias = 1.0f;

    m_shadowPipeline = Graphics::CreateGraphicsPipelineState(pd);

    Graphics::CommandStreamDesc csd;
    csd.QueueType = Graphics::GFX_COMMAND_QUEUE_TYPE_DRAW;
    Graphics::CreateCommandStream(csd, &m_commandStream);

    s_loading.store(false);

    return true;
}

int MeshApplication::Shutdown()
{
    s_consoleThread.join();
    Graphics::ReleaseCommandStream(&m_commandStream);
    Graphics::Shutdown();

    return 0;
}

void MeshApplication::Update(double dt)
{
    ShaderOptions shaderOptions;
    {
        std::lock_guard<std::mutex> lock(s_shaderOptionMutex);
        shaderOptions = s_shaderOptions;
    }

    m_cameraController.Update(dt);

    float4x4 view = m_cameraController.GetCamera()->GetView();
    float4x4 proj = m_cameraController.GetCamera()->GetProjection();

    FrameConstants* pConsts = &m_frameConsts[m_frameIndex];

    pConsts->CameraConsts.View = view.transpose();
    pConsts->CameraConsts.Proj = proj.transpose();
    pConsts->CameraConsts.InverseView = view.inverse().transpose();
    pConsts->CameraConsts.InverseProj = proj.inverse().transpose();

    view = s_lighting.GetView();
    proj = s_lighting.GetProjection();

    pConsts->ShadowConsts.View = view.transpose();
    pConsts->ShadowConsts.Proj = proj.transpose();
    pConsts->ShadowConsts.InverseView = view.inverse().transpose();
    pConsts->ShadowConsts.InverseProj = proj.inverse().transpose();

    pConsts->LightingConsts.Color = shaderOptions.LightColor;
    pConsts->LightingConsts.Direction = shaderOptions.LightDirection;
    pConsts->LightingConsts.Intensity = shaderOptions.LightIntensity;
    pConsts->LightingConsts.Exposure = shaderOptions.Exposure;
    pConsts->LightingConsts.EnableAmbient = shaderOptions.AmbientEnabled;
    pConsts->LightingConsts.EnableDiffuse = shaderOptions.DiffuseEnabled;
    pConsts->LightingConsts.EnableSpec = shaderOptions.SpecEnabled;
    pConsts->LightingConsts.EnableBump = shaderOptions.BumpEnabled;
    pConsts->LightingConsts.EnableShadow = shaderOptions.ShadowEnabled;

    static const float clear[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
    static const Graphics::Viewport vp = { 0.0f, 0.0f, 1600.0f, 900.0f, 0.0f, 1.0f };
    static const Graphics::Rect scissor = { 0, 0, 1600, 900 };
    

    // Shadow 
    if (shaderOptions.ShadowEnabled)
    {
        static const Graphics::Viewport s_shadow_vp = { 0.0f, 0.0f, 2048.0f, 2048.0f, 0.0f, 1.0f };
        static const Graphics::Rect s_shadow_scissor = { 0, 0, 2048, 2048 };

        Graphics::UpdateConstantBuffer(&pConsts->ShadowConsts, sizeof(CameraConstants), pConsts->hShadow);

        auto& cs = m_commandStream;
        cs.SetPipeline(m_shadowPipeline);
        cs.SetRenderTargets(0, nullptr, m_shadowTexture);
        cs.ClearDepthStencil(1.0f, 0, m_shadowTexture);
        cs.SetViewport(s_shadow_vp);
        cs.SetScissorRect(s_shadow_scissor);
        cs.SetConstantBuffer(0, pConsts->hShadow);

        cs.SetPrimitiveTopology(Graphics::GFX_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        for (u32 i = 0, count = (u32)m_surfaces.size(); i < count; ++i)
        {
            if (m_materialIndices[i] == -1) continue;
            auto& surface = m_surfaces[i];
            cs.SetVertexBuffer(surface.vb);
            cs.SetIndexBuffer(surface.ib);
            cs.DrawInstanceIndexed(surface.ib);
        }

        Graphics::SubmitCommandStream(&cs, false);
    }
    
    if (s_shadowFullScreen)
    {
        auto& cs = m_commandStream;
        cs.TransitionDepthStencilToTexture(m_shadowTexture);
        cs.SetPipeline(m_fullScreenPipeline);
        cs.SetRenderTargets();
        cs.ClearRenderTarget(clear);
        cs.ClearDepthStencil(1.0f, 0);
        cs.SetViewport(vp);
        cs.SetScissorRect(scissor);
        cs.SetDescriptorTable(0, m_shadowTexture);

        cs.SetPrimitiveTopology(Graphics::GFX_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cs.SetVertexBuffer(0);
        cs.SetIndexBuffer(m_fs_ib);
        cs.DrawInstanceIndexed(m_fs_ib);

        Graphics::SubmitCommandStream(&cs, false);
    }
    else
    {
        // Main visual
        Graphics::UpdateConstantBuffer(&pConsts->CameraConsts, sizeof(CameraConstants), pConsts->hCamera);
        Graphics::UpdateConstantBuffer(&pConsts->LightingConsts, sizeof(LightingConstants), pConsts->hLighting);

        auto& cs = m_commandStream;
        cs.TransitionDepthStencilToTexture(m_shadowTexture);
        cs.SetPipeline(m_opaquePipeline);
        cs.SetRenderTargets();
        cs.ClearRenderTarget(clear);
        cs.ClearDepthStencil(1.0f, 0);
        cs.SetViewport(vp);
        cs.SetScissorRect(scissor);
        cs.SetConstantBuffer(0, pConsts->hCamera);
        cs.SetConstantBuffer(1, pConsts->hShadow);
        cs.SetConstantBuffer(2, pConsts->hLighting);
        cs.SetDescriptorTable(3, m_baseDescriptorHandle);

        cs.SetPrimitiveTopology(Graphics::GFX_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        for (u32 i = 0, count = (u32)m_surfaces.size(); i < count; ++i)
        {
            if (m_materialIndices[i] == -1) continue;
            auto& surface = m_surfaces[i];
            cs.SetVertexBuffer(surface.vb);
            cs.SetIndexBuffer(surface.ib);
            cs.DrawInstanceIndexed(surface.ib);
        }

        cs.TransitionDepthStencilToDepthWrite(m_shadowTexture);

        Graphics::SubmitCommandStream(&cs, false);
    }
        
    Graphics::Present();

    m_frameIndex = (m_frameIndex + 1) % s_nSwapChainTargets;

    if (Platform::Input::GetKey(Platform::Input::KEYCODE_I)) s_lighting.GetTransform()->MoveForward(1);
    if (Platform::Input::GetKey(Platform::Input::KEYCODE_J)) s_lighting.GetTransform()->MoveRight(-1);
    if (Platform::Input::GetKey(Platform::Input::KEYCODE_K)) s_lighting.GetTransform()->MoveRight(1);
    if (Platform::Input::GetKey(Platform::Input::KEYCODE_L)) s_lighting.GetTransform()->MoveForward(-1);

    if (Platform::Input::GetKey(Platform::Input::KEYCODE_V)) s_lighting.SetWidth(s_lighting.GetWidth() + 1);
    if (Platform::Input::GetKey(Platform::Input::KEYCODE_B)) s_lighting.SetWidth(s_lighting.GetWidth() - 1);
    if (Platform::Input::GetKey(Platform::Input::KEYCODE_G)) s_lighting.SetHeight(s_lighting.GetHeight() + 1);
    if (Platform::Input::GetKey(Platform::Input::KEYCODE_H)) s_lighting.SetHeight(s_lighting.GetHeight() - 1);

    if (Platform::Input::GetKeyUp(Platform::Input::KEYCODE_RETURN)) s_consoleWritten.store(true);
}

void MeshApplication::FixedUpdate(double dt)
{
    static double s_time = 0.0;



    s_time += dt;
}
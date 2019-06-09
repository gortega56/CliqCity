#include "SceneViewer.h"
#include "Engine.h"
#include "Resource\ResourceManager.h"
#include "Mesh.h"
#include <functional>
#include <memory>
#include "Resource\FbxResource.h"
#include "Resource\ObjResource.h"
#include "Platform\PlatformWindow.h"
#include "Graphics.h"
#include "CommandStream.h"
#include "Platform\PlatformInput.h"
#include "SceneResource.h"

#include "Platform/PlatformImgui.h"
#include "GraphicsImgui.h"
#include "imgui.h"

#include <iostream>
#include <sstream>

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#define ASSETS_DIRECTORY "C:\\Users\\Gabriel Ortega\\Desktop\\CliqCity\\Assets\\"

#define CUBEMAP_FILENAME "C:\\Users\\Gabriel Ortega\\Desktop\\CliqCity\\Assets\\satara_night.dds"

using namespace Luz;
using namespace lina;



Vertex::Vertex(
    float px, float py, float pz,
    float nx, float ny, float nz,
    float u, float v)
    : Tangent(0.0f, 0.0f, 0.0f, 0.0f)
    , Position(px, py, pz)
    , Normal(nx, ny, nz)
    , UV(u, v, -1.0f)
{

}

struct ShaderOptions
{
    float3 LightColor;
    float3 LightDirection;
    float4 LightIntensity;
    float Exposure;
    u32 LightingMode;
    u32 BumpMode;
    bool AmbientEnabled;
    bool DiffuseEnabled;
    bool SpecEnabled;
    bool BumpEnabled;
    bool ShadowEnabled;
    bool MicrofacetEnabled;
    bool MaskingEnabled;
    bool FresnelEnabled;
};

struct EnvironmentParameters
{
    Graphics::CommandStream* pCommandStream;
    Graphics::VertexBufferHandle hCubeVertexBuffer;
    Graphics::IndexBufferHandle hCubeIndexBuffer;
    Graphics::IndexBufferHandle hTriIndexBuffer;
    Graphics::TextureHandle hCubeTexture;
    Graphics::TextureHandle hRenderTarget;
    Graphics::TextureHandle hDfgRenderTarget;
    Graphics::ShaderHandle hCubeVertexShader;
    Graphics::ShaderHandle hTriVertexShader;
    Graphics::ShaderHandle hRadiancePixelShader;
    Graphics::ShaderHandle hIrradiancePixelShader;
    Graphics::ShaderHandle hDfgPixelShader;
    float AspectRatio;
};

struct TextureOverrides
{
	const char* Metal;
	const char* Rough;
	const char* Normal;
};

const char* s_pAssetDirectory = ASSETS_DIRECTORY;

static int s_window_width = 1600;

static int s_window_height = 900;

static float s_lightMoveSpeed = 0.2f;

static bool s_shadowFullScreen = false;

static bool s_bDebugConsole = false;

static ShaderOptions s_shaderOptions =
{
    float3(1.0f, 0.96f, 0.95f),
    normalize(float3(0.0f, -0.5f, -0.1f)),
    float4(0.2f, 10.0f, 10.0f, 1.0f),
    10.0f,
    SceneResource::SHADING_MODE_DEFAULT,
    1,
    true,
    true,
    true,
    true,
    true,
    true,
    true,
    true
};

static std::mutex s_shaderOptionMutex;

static std::atomic_bool s_loading = ATOMIC_VAR_INIT(0);

static OrthographicCamera s_lighting = OrthographicCamera(1500.0f, 1500.0f, 0.1f, 3000.0f);

static SceneViewer* s_pSceneViewer = nullptr;

static void UpdateScene(const Resource::Obj* pObj, Scene* pScene, TextureOverrides* pTextureOverrides);

static void UpdateScene(const Resource::Fbx* pFbx, Scene* pScene, TextureOverrides* pTextureOverrides);

static void DestroyScene(const Scene* pScene);

static void ProcessEnvironmentLighting(const EnvironmentParameters& params);

static int FindOrPushBackTextureName(std::vector<std::string>& textureNames, std::string textureName);

static void ConsoleThread();

SceneViewer::SceneViewer()
    : m_window(-1)
	, m_pScene(nullptr)
	, m_pStagingScene(nullptr)
    , m_frameIndex(0)
	, m_cameraController()
	, m_commandStream()
    , m_vsScene(0)
    , m_psScene(0)
    , m_vsFullScreen(0)
    , m_psFullScreen(0)
	, m_vsCubeMap(0)
	, m_psCubeMap(0)
	, m_psIrradianceMap(0)
	, m_psRadianceMap(0)
	, m_psLUT(0)
	, m_vbCubeMap(0)
	, m_ibCubeMap(0)
    , m_ibFullScreen(0)
	, m_txCubeMap(0)
	, m_txEnvMap(0)
	, m_txLUT(0)
	, m_txShadow(0)
	, m_opaquePipeline(0)
    , m_shadowPipeline(0)
    , m_fullScreenPipeline(0)
	, m_cubemapPipeline(0)
{
    s_pSceneViewer = this;
    auto nThreads = std::thread::hardware_concurrency() - 1;
    LUZASSERT(nThreads > 0);
    m_loadingThreads.resize(nThreads);
}

SceneViewer::~SceneViewer()
{

}

bool SceneViewer::Initialize()
{
    m_window = Platform::MakeWindow("Scene Viewer", s_window_width, s_window_height, false);

    bool success = Graphics::Initialize(m_window, s_nSwapChainTargets);
    LUZASSERT(success);

    // imgui
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui::StyleColorsDark();

        success = Platform::Initialize_Imgui(m_window);
        LUZASSERT(success);

        success = Graphics::Initialize_Imgui(s_nSwapChainTargets);
        LUZASSERT(success);
    }

	LoadScene("Sponza.scene", 0);

	Platform::Window window;
	Platform::GetWindow(m_window, window);

	m_cameraController = CameraController();
	m_cameraController.SetWindowWidth(window.Width);
	m_cameraController.SetWindowHeight(window.Height);

    PerspectiveCamera* pCamera = m_cameraController.GetCamera();
    pCamera->GetTransform()->SetPosition(0.0f, 0.0f, -15.0f);
    pCamera->SetFieldOfView(3.14f * 0.5f);
    pCamera->SetAspectRatio(window.Aspect);
    pCamera->SetNear(0.1f);
    pCamera->SetFar(3000.0f);

    // Shaders
    {
        if (IsDebuggerPresent())
        {
            m_vsScene = Graphics::CreateVertexShader("Shaders\\scene_model_vs.hlsl");
            m_psScene = Graphics::CreatePixelShader("Shaders\\scene_model_ps.hlsl");

            m_vsFullScreen = Graphics::CreateVertexShader("Shaders\\FS_Tri_VS.hlsl");
            m_psFullScreen = Graphics::CreatePixelShader("Shaders\\FS_Tri_PS.hlsl");

            m_vsCubeMap = Graphics::CreateVertexShader("Shaders\\cube_map_vs.hlsl");
            m_psCubeMap = Graphics::CreatePixelShader("Shaders\\cube_map_ps.hlsl");

            m_psRadianceMap = Graphics::CreatePixelShader("Shaders\\radiance_map_ps.hlsl");
            m_psIrradianceMap = Graphics::CreatePixelShader("Shaders\\irradiance_map_ps.hlsl");
            m_psLUT = Graphics::CreatePixelShader("Shaders\\brdf_integration_ps.hlsl");
        }
        else
        {
            m_vsScene = Graphics::CreateVertexShader("scene_model_vs.cso");
            m_psScene = Graphics::CreatePixelShader("scene_model_ps.cso");

            m_vsFullScreen = Graphics::CreateVertexShader("FS_Tri_VS.cso");
            m_psFullScreen = Graphics::CreatePixelShader("FS_Tri_PS.cso");

            m_vsCubeMap = Graphics::CreateVertexShader("cube_map_vs.cso");
            m_psCubeMap = Graphics::CreatePixelShader("cube_map_ps.cso");

            m_psRadianceMap = Graphics::CreatePixelShader("radiance_map_ps.cso");
            m_psIrradianceMap = Graphics::CreatePixelShader("irradiance_map_ps.cso");
            m_psLUT = Graphics::CreatePixelShader("brdf_integration_ps.cso");
        }
    }

    // Static geometry
    {
        float3 cube_map_vertices[] =
        {
            { +0.5f, +0.5f, +0.5f },
            { +0.5f, +0.5f, -0.5f },
            { +0.5f, -0.5f, +0.5f },
            { +0.5f, -0.5f, -0.5f },
            { -0.5f, +0.5f, +0.5f },
            { -0.5f, +0.5f, -0.5f },
            { -0.5f, -0.5f, +0.5f },
            { -0.5f, -0.5f, -0.5f }
        };

        unsigned int cube_map_indices[] =
        {
            // right
            0, 1, 3,
            3, 2, 0,

            // top
            5, 1, 0,
            0, 4, 5,

            // front
            1, 5, 7,
            7, 3, 1,

            // left
            5, 4, 6,
            6, 7, 5,

            // bottom
            6, 2, 3,
            3, 7, 6,

            // back
            4, 0, 2,
            2, 6, 4
        };

        Graphics::BufferDesc vb;
        vb.Alignment = 0;
        vb.SizeInBytes = sizeof(float3) * 8;
        vb.StrideInBytes = sizeof(float3);
        vb.pData = cube_map_vertices;
        m_vbCubeMap = Graphics::CreateVertexBuffer(vb);

        Graphics::BufferDesc ib;
        ib.Alignment = 0;
        ib.SizeInBytes = sizeof(int) * 36;
        ib.StrideInBytes = sizeof(int);
        ib.pData = cube_map_indices;
        m_ibCubeMap = Graphics::CreateIndexBuffer(ib);

        static u32 s_ib[] = { 0, 1, 2 };
        ib.Alignment = 0;
        ib.SizeInBytes = 12;
        ib.StrideInBytes = sizeof(u32);
        ib.pData = s_ib;
        m_ibFullScreen = Graphics::CreateIndexBuffer(ib);
    }

    // Frame Resources
    {
        for (u32 i = 0; i < s_nFrameResources; ++i)
        {
            Graphics::ConstantBufferDesc cbd;
            cbd.Alignment = 0;
            cbd.SizeInBytes = sizeof(CameraConstants);
            cbd.StrideInBytes = sizeof(CameraConstants);
            cbd.AllocHeap = false;
            cbd.pData = nullptr;// &m_frameConsts[0].CameraConsts;
            m_frameConsts[i].hCamera = Graphics::CreateConstantBuffer(cbd);
            m_frameConsts[i].hShadow = Graphics::CreateConstantBuffer(cbd);

            cbd.SizeInBytes = sizeof(LightingConstants);
            cbd.SizeInBytes = sizeof(LightingConstants);
            m_frameConsts[i].hLighting = Graphics::CreateConstantBuffer(cbd);
        }
    }

    // Textures
    {
        Graphics::DepthStencilDesc ds;
        ds.ClearDepth = 1.0f;
        ds.ClearStencil = 0;
        ds.Flags = Graphics::GFX_DEPTH_STENCIL_FLAG_SHADER_RESOURCE;
        ds.Format = Graphics::GFX_FORMAT_R32_TYPELESS;
        ds.Width = 2048;
        ds.Height = 2048;
        m_txShadow = Graphics::CreateDepthStencil(ds);

        // Environment brdf
        {
            Graphics::TextureDesc td;
            td.Width = 512;
            td.Height = 512;
            td.MipLevels = 1;
            td.Depth = 1;
            td.SampleCount = 1;
            td.SampleQuality = 0;
            td.Format = Graphics::GFX_FORMAT_R16G16_FLOAT;
            td.Dimension = Graphics::GFX_RESOURCE_DIMENSION_TEXTURE2D;
            td.Flags = Graphics::GFX_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
            td.bIsCube = false;
            m_txLUT = Graphics::CreateTexture(td);
        }

        // Environment cube
        {
            Graphics::TextureDesc td;
            td.Width = 256;
            td.Height = 256;
            td.Depth = 6;
            td.MipLevels = 6;
            td.SampleCount = 1;
            td.SampleQuality = 0;
            td.Format = Graphics::GFX_FORMAT_R8G8B8A8_UNORM;
            td.Dimension = Graphics::GFX_RESOURCE_DIMENSION_TEXTURE2D;
            td.Flags = Graphics::GFX_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
            td.bIsCube = true;
            m_txEnvMap = Graphics::CreateTexture(td);
        }

        // Sky box
        {
            Graphics::TextureFileDesc fd;
            fd.Filename = CUBEMAP_FILENAME;
            fd.GenMips = false;
            m_txCubeMap = Graphics::CreateTexture(fd);
        }
    }
    
    // Pipelines
    {
        // Opaque pipeline

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
            .AppendDescriptorTableRange(3, -1, 3, 0, Graphics::DescriptorTable::Range::DESCRIPTOR_TABLE_RANGE_TYPE_CONSTANT_VIEW)   // Array of CBVs
            .AppendDescriptorTable(Graphics::SHADER_VISIBILITY_ALL)
            .AppendDescriptorTableRange(4, -1, 0, 0, Graphics::DescriptorTable::Range::DESCRIPTOR_TABLE_RANGE_TYPE_SHADER_VIEW)     // Array of SRVs
            .AppendDescriptorTable(Graphics::SHADER_VISIBILITY_ALL)
            .AppendDescriptorTableRange(5, 1, 0, 1, Graphics::DescriptorTable::Range::DESCRIPTOR_TABLE_RANGE_TYPE_SHADER_VIEW)      // Shadow
            .AppendDescriptorTableRange(5, 1, 0, 2, Graphics::DescriptorTable::Range::DESCRIPTOR_TABLE_RANGE_TYPE_SHADER_VIEW)      // Env BRDF
            .AppendDescriptorTableRange(5, 1, 0, 3, Graphics::DescriptorTable::Range::DESCRIPTOR_TABLE_RANGE_TYPE_SHADER_VIEW)      // Env Cube
            .AppendLinearWrapSampler(0)
            .AppendAnisotropicWrapSampler(1)
            .AppendComparisonPointBorderSampler(2);
        pd.InputLayout.AppendFloat4("TANGENT")
            .AppendPosition3F()
            .AppendNormal3F()
            .AppendUV3();
        pd.VertexShaderHandle = m_vsScene;
        pd.PixelShaderHandle = m_psScene;
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

        // full screen quad pipeline

        memset(&pd.Signature, 0, sizeof(Graphics::SignatureDesc));
        pd.Signature.SetName("fullscreen")
            .AllowInputLayout()
            .DenyHS()
            .DenyDS()
            .DenyGS()
            .AppendDescriptorTable(Graphics::SHADER_VISIBILITY_PS)
            .AppendDescriptorTableRange(0, 1, 1, 0, Graphics::DescriptorTable::Range::DESCRIPTOR_TABLE_RANGE_TYPE_SHADER_VIEW)
            .AppendAnisotropicWrapSampler(0);
        pd.VertexShaderHandle = m_vsFullScreen;
        pd.PixelShaderHandle = m_psFullScreen;
        pd.UseSwapChain = true;
        m_fullScreenPipeline = Graphics::CreateGraphicsPipelineState(pd);

        // shadow map pipeline

        memset(&pd.Signature, 0, sizeof(Graphics::SignatureDesc));
        pd.Signature.SetName("shadow")
            .AllowInputLayout()
            .DenyHS()
            .DenyDS()
            .DenyGS()
            .AppendConstantView(0);
        pd.VertexShaderHandle = m_vsScene;
        pd.PixelShaderHandle = 0;
        pd.NumRenderTargets = 0;
        pd.DsHandle = m_txShadow;
        pd.UseSwapChain = false;

        pd.Rasterizer.DepthBias = 100000;
        pd.Rasterizer.DepthBiasClamp = 0.0f;
        pd.Rasterizer.SlopeScaledDepthBias = 1.0f;
        pd.Rasterizer.DepthClipEnable = false;

        m_shadowPipeline = Graphics::CreateGraphicsPipelineState(pd);

        // skybox pipeline
        Graphics::PipelineDesc skyboxPipe;
        skyboxPipe.Signature.SetName("skybox")
            .AllowInputLayout()
            .DenyHS()
            .DenyDS()
            .DenyGS()
            .AppendConstantView(0)
            .AppendDescriptorTable(Graphics::SHADER_VISIBILITY_PS)
            .AppendDescriptorTableRange(1, 1, 0, 0, Graphics::DescriptorTable::Range::DESCRIPTOR_TABLE_RANGE_TYPE_SHADER_VIEW)
            .AppendLinearWrapSampler(0);
        skyboxPipe.InputLayout.AppendPosition3F();
        skyboxPipe.VertexShaderHandle = m_vsCubeMap;
        skyboxPipe.PixelShaderHandle = m_psCubeMap;
        skyboxPipe.Topology = Graphics::GFX_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        skyboxPipe.SampleCount = 1;
        skyboxPipe.SampleQuality = 0;
        skyboxPipe.SampleMask = 0xffffffff;

        skyboxPipe.DepthStencil = Graphics::DepthStencilState::DepthCompareLessEqualWriteAll_StencilOff;
        skyboxPipe.Rasterizer = Graphics::RasterizerState::FillSolidCullCCW;
        skyboxPipe.Blend.BlendStates[0] = Graphics::RenderTargetBlendState::Replace;
        skyboxPipe.Blend.AlphaToCoverageEnable = false;
        skyboxPipe.Blend.IndependentBlendEnable = false;

        skyboxPipe.UseSwapChain = true;

        m_cubemapPipeline = Graphics::CreateGraphicsPipelineState(skyboxPipe);
    }

    Graphics::CommandStreamDesc csd;
    csd.QueueType = Graphics::GFX_COMMAND_QUEUE_TYPE_DRAW;
    Graphics::CreateCommandStream(csd, &m_commandStream);

    EnvironmentParameters params;
    params.hCubeTexture = m_txCubeMap;
    params.hRenderTarget = m_txEnvMap;
    params.hDfgRenderTarget = m_txLUT;
    params.hCubeVertexBuffer = m_vbCubeMap;
    params.hCubeIndexBuffer = m_ibCubeMap;
    params.hTriIndexBuffer = m_ibFullScreen;
    params.hCubeVertexShader = m_vsCubeMap;
    params.hTriVertexShader = m_vsFullScreen;
    params.hRadiancePixelShader = m_psRadianceMap;
    params.hIrradiancePixelShader = m_psIrradianceMap;
    params.hDfgPixelShader = m_psLUT;
    params.AspectRatio = window.Aspect;
    params.pCommandStream = &m_commandStream;
    ProcessEnvironmentLighting(params);

    if (s_bDebugConsole)
    {
        std::thread(ConsoleThread).detach();
    }
    
    return true;
}

int SceneViewer::Shutdown()
{
    for (auto& thread : m_loadingThreads)
    {
        if (thread.joinable()) thread.join();
    }

    Graphics::Flush();

    Graphics::Shutdown_Imgui();
    Platform::Shutdown_Imgui();
    ImGui::DestroyContext();

    for (u32 i = 0; i < s_nFrameResources; ++i)
    {
        Graphics::ReleaseConstantBuffer(m_frameConsts[i].hCamera);
        Graphics::ReleaseConstantBuffer(m_frameConsts[i].hLighting);
        Graphics::ReleaseConstantBuffer(m_frameConsts[i].hShadow);
    }

    DestroyScene(m_pStagingScene.get());
    DestroyScene(m_pScene.get());

    Graphics::ReleaseShader(m_vsScene);
    Graphics::ReleaseShader(m_psScene);
    Graphics::ReleaseShader(m_vsFullScreen);
    Graphics::ReleaseShader(m_psFullScreen);
    Graphics::ReleaseShader(m_vsCubeMap);
    Graphics::ReleaseShader(m_psCubeMap);
    Graphics::ReleaseShader(m_psRadianceMap);
    Graphics::ReleaseShader(m_psIrradianceMap);
    Graphics::ReleaseShader(m_psLUT);

    Graphics::ReleaseVertexBuffer(m_vbCubeMap);
    Graphics::ReleaseIndexBuffer(m_ibCubeMap);
    Graphics::ReleaseIndexBuffer(m_ibFullScreen);

    Graphics::ReleaseTexture(m_txCubeMap);
    Graphics::ReleaseTexture(m_txEnvMap);
    Graphics::ReleaseTexture(m_txLUT);

    Graphics::ReleaseDepthStencil(m_txShadow);

    Graphics::ReleasePipeline(m_opaquePipeline);
    Graphics::ReleasePipeline(m_shadowPipeline);
    Graphics::ReleasePipeline(m_fullScreenPipeline);
    Graphics::ReleasePipeline(m_cubemapPipeline);

    Graphics::ReleaseCommandStream(&m_commandStream);
    Graphics::Shutdown();

    Platform::FreeWindow(m_window);

    return 0;
}

void SceneViewer::Update(double dt)
{
	m_cameraController.Update(dt);

	std::shared_ptr<Scene> pScene = nullptr;
	bool bDidLoad = false;
	{
		std::lock_guard<std::mutex> lock(m_sceneMutex);

		if (m_pStagingScene)
		{
			// wait for any command lists in flight
			// before swapping scene
			Graphics::Flush();
			DestroyScene(m_pScene.get());
			m_pScene = m_pStagingScene;
			m_pStagingScene = nullptr;
			bDidLoad = true;
		}

		pScene = m_pScene;
	}

	if (bDidLoad)
	{
		std::lock_guard<std::mutex> lock(s_shaderOptionMutex);
		s_shaderOptions.LightingMode = pScene->eShading;
		s_shaderOptions.BumpMode = pScene->eBump;
	}

	ShaderOptions shaderOptions;
	{
		std::lock_guard<std::mutex> lock(s_shaderOptionMutex);
		shaderOptions = s_shaderOptions;
	}

    if (pScene)
    {
		float3 dim = pScene->Bounds.max - pScene->Bounds.min;
		float3 lp = dim * -normalize(shaderOptions.LightDirection);
		float3x3 lv = float3x3::orientation_lh(shaderOptions.LightDirection, float3(0.0f, 0.0f, 1.0f));
        
		s_lighting.GetTransform()->SetPosition(lp);
		s_lighting.GetTransform()->SetRotation(quaternion::create(lv));
        
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
		pConsts->LightingConsts.LightingMode = shaderOptions.LightingMode;
		pConsts->LightingConsts.BumpMode = shaderOptions.BumpMode;
		pConsts->LightingConsts.EnableAmbient = shaderOptions.AmbientEnabled;
		pConsts->LightingConsts.EnableDiffuse = shaderOptions.DiffuseEnabled;
		pConsts->LightingConsts.EnableSpec = shaderOptions.SpecEnabled;
		pConsts->LightingConsts.EnableBump = shaderOptions.BumpEnabled;
		pConsts->LightingConsts.EnableShadow = shaderOptions.ShadowEnabled;
		pConsts->LightingConsts.MicrofacetEnabled = shaderOptions.MicrofacetEnabled;
		pConsts->LightingConsts.MaskingEnabled = shaderOptions.MaskingEnabled;
		pConsts->LightingConsts.FresnelEnabled = shaderOptions.FresnelEnabled;
        
		static const Graphics::Viewport vp = { 0.0f, 0.0f, static_cast<float>(s_window_width), static_cast<float>(s_window_height), 0.0f, 1.0f };
		static const Graphics::Rect scissor = { 0, 0, static_cast<u32>(s_window_width), static_cast<u32>(s_window_height) };
		float clear[4] =
		{
		    shaderOptions.LightColor.x,
		    shaderOptions.LightColor.y,
		    shaderOptions.LightColor.z,
		    1.0f,
		};

        if (shaderOptions.ShadowEnabled)
        {
            static const Graphics::Viewport s_shadow_vp = { 0.0f, 0.0f, 2048.0f, 2048.0f, 0.0f, 1.0f };
            static const Graphics::Rect s_shadow_scissor = { 0, 0, 2048, 2048 };
            
            Graphics::UpdateConstantBuffer(&pConsts->ShadowConsts, sizeof(CameraConstants), pConsts->hShadow);
            
            auto& cs = m_commandStream;
            Graphics::ResetCommandStream(&cs, m_shadowPipeline);
            
            cs.SetRenderTargets(0, nullptr, m_txShadow);
            cs.ClearDepthStencil(1.0f, 0, m_txShadow);
            cs.SetViewport(s_shadow_vp);
            cs.SetScissorRect(s_shadow_scissor);
            cs.SetConstantBuffer(0, pConsts->hShadow);
            
            cs.SetPrimitiveTopology(Graphics::GFX_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            for (auto& surface : pScene->Surfaces)
            {
                cs.SetVertexBuffer(surface.vb);
                cs.SetIndexBuffer(surface.ib);
                cs.DrawInstanceIndexed(surface.ib);
            }
            
            Graphics::SubmitCommandStream(&cs);
        }
        
        if (s_shadowFullScreen)
        {
            auto& cs = m_commandStream;
            // cs.TransitionDepthStencil(m_txShadow, Graphics::GFX_RESOURCE_STATE_DEPTH_WRITE, Graphics::GFX_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | Graphics::GFX_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        	Graphics::ResetCommandStream(&cs, m_fullScreenPipeline);
        
            cs.SetRenderTargets();
            cs.ClearRenderTarget(clear);
            cs.ClearDepthStencil(1.0f, 0);
            cs.SetViewport(vp);
            cs.SetScissorRect(scissor);
            cs.SetDescriptorTable(0, &m_txLUT, 1);
        
            cs.SetPrimitiveTopology(Graphics::GFX_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            cs.SetVertexBuffer(0);
            cs.SetIndexBuffer(m_ibFullScreen);
            cs.DrawInstanceIndexed(m_ibFullScreen);
            // cs.TransitionDepthStencil(m_txShadow, Graphics::GFX_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | Graphics::GFX_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, Graphics::GFX_RESOURCE_STATE_DEPTH_WRITE);
        
            Graphics::SubmitCommandStream(&cs);
        }
        else
        {
            unsigned int nConstants = static_cast<unsigned int>(pScene->Constants.size());
            unsigned int nTextures = static_cast<unsigned int>(pScene->Textures.size());
            size_t szConstants = sizeof(Graphics::GpuResourceHandle) * nConstants;
            size_t szTextures = sizeof(Graphics::GpuResourceHandle) * nTextures;
        
            Graphics::DescriptorTableRange pRanges[3];
            pRanges[0].Register = 3;
            pRanges[0].nHandles = nConstants;
            pRanges[1].Register = 4;
            pRanges[1].nHandles = nTextures;
            pRanges[2].Register = 5;
            pRanges[2].nHandles = 3;
            pRanges[2].pHandles[0] = m_txShadow;
            pRanges[2].pHandles[1] = m_txLUT;
            pRanges[2].pHandles[2] = m_txEnvMap;
            memcpy_s(pRanges[0].pHandles, szConstants, pScene->Constants.data(), szConstants);
            memcpy_s(pRanges[1].pHandles, szTextures, pScene->Textures.data(), szTextures);
        
            // Main visual
            Graphics::UpdateConstantBuffer(&pConsts->CameraConsts, sizeof(CameraConstants), pConsts->hCamera);
            Graphics::UpdateConstantBuffer(&pConsts->LightingConsts, sizeof(LightingConstants), pConsts->hLighting);
        
            auto& cs = m_commandStream;
        	Graphics::ResetCommandStream(&cs, m_opaquePipeline);
        
            cs.TransitionRenderTarget(Graphics::GFX_RESOURCE_STATE_PRESENT, Graphics::GFX_RESOURCE_STATE_RENDER_TARGET);
            cs.TransitionDepthStencil(m_txShadow, Graphics::GFX_RESOURCE_STATE_DEPTH_WRITE, Graphics::GFX_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | Graphics::GFX_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            cs.SetRenderTargets();
            cs.ClearRenderTarget(clear);
            cs.ClearDepthStencil(1.0f, 0);
            cs.SetViewport(vp);
            cs.SetScissorRect(scissor);
            cs.SetConstantBuffer(0, pConsts->hCamera);
            cs.SetConstantBuffer(1, pConsts->hShadow);
            cs.SetConstantBuffer(2, pConsts->hLighting);
            cs.SetDescriptorTable(pRanges, 3);
        
            cs.SetPrimitiveTopology(Graphics::GFX_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            for (auto& surface : pScene->Surfaces)
            {
                cs.SetVertexBuffer(surface.vb);
                cs.SetIndexBuffer(surface.ib);
                cs.DrawInstanceIndexed(surface.ib);
            }
        
            cs.TransitionDepthStencil(m_txShadow, Graphics::GFX_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | Graphics::GFX_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, Graphics::GFX_RESOURCE_STATE_DEPTH_WRITE);
            Graphics::SubmitCommandStream(&cs);
        
            // Draw cube map
        	Graphics::ResetCommandStream(&cs, m_cubemapPipeline);
            cs.SetRenderTargets();
            cs.SetViewport(vp);
            cs.SetScissorRect(scissor);
            cs.SetConstantBuffer(0, pConsts->hCamera);
            cs.SetDescriptorTable(1, &m_txCubeMap, 1);
        
            cs.SetPrimitiveTopology(Graphics::GFX_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            cs.SetVertexBuffer(m_vbCubeMap);
            cs.SetIndexBuffer(m_ibCubeMap);
            cs.DrawInstanceIndexed(m_ibCubeMap);
        
            Graphics::SubmitCommandStream(&cs);
        
            //imgui
            {
                Graphics::Update_Imgui();
                Platform::Update_Imgui();
                ImGui::NewFrame();

                ImGui::Begin("Hello, world!");
                ImGui::Text("This is some text.");
                ImGui::End();

                auto& cs = m_commandStream;
                Graphics::ResetCommandStream(&cs, 0);
                cs.SetRenderTargets(false);
                Graphics::Render_Imgui(&cs);

                Graphics::SubmitCommandStream(&cs);
            }
        
        }
        
        Graphics::Present();
    }
        
    m_frameIndex = (m_frameIndex + 1) % s_nFrameResources;
}

void SceneViewer::LoadScene(const std::string filename, const u32 threadID)
{
    if (m_loadingThreads[threadID].joinable())
    {
        m_loadingThreads[threadID].join();
    }

    s_loading.store(true);

    m_loadingThreads[threadID] = std::thread([&, filename]()
    {
        char path[MAX_PATH];
        strcpy_s(path, s_pAssetDirectory);
        strcat_s(path, filename.c_str());

        auto pScene = Resource::Async<SceneResource>::Load(path).Get();
        if (!pScene)
        {
            return;
        }

        auto getExtension = [](const char* file) -> const char*
        {
            int i = static_cast<int>(strlen(file)) - 1;
            while (i > 0)
            {
                if (file[i] == '.')
                {
                    return &file[i + 1];
                }

                i -= 1;
            }

            return nullptr;
        };

        std::vector<Resource::Async<Resource::Obj>> loadingObjs;
        std::vector<Resource::Async<Resource::Fbx>> loadingFbxs;
		std::vector<TextureOverrides> textures;

        u32 nAssets = pScene->NumAssets();

        for (u32 iAsset = 0, nAssets = pScene->NumAssets(); iAsset < nAssets; ++iAsset)
        {
            const SceneResource::Asset& asset = pScene->GetAsset(iAsset);
            const char* rootDir = pScene->GetDirectory(asset.RootDir);
            const char* textureDir = pScene->GetDirectory(asset.TextureDir);

			char relativePath[MAX_PATH];
			strcpy_s(relativePath, s_pAssetDirectory);
			if (strlen(rootDir))
			{
				strcat_s(relativePath, rootDir);
				strcat_s(relativePath, "\\");
			}

			char texturePath[MAX_PATH];
			strcpy_s(texturePath, relativePath);
			if (strlen(textureDir))
			{
				strcat_s(texturePath, textureDir);
				strcat_s(texturePath, "\\");
			}

            for (int iFile = 0; iFile < asset.nFiles; ++iFile)
            {
                const char* file = pScene->GetFile(asset.pFiles[iFile]);
                const char* ext = getExtension(file);

                char filePath[MAX_PATH];
                strcpy_s(filePath, relativePath);
                strcat_s(filePath, file);

                if (strcmp(ext, "obj") == 0)
                {
                    Resource::Obj::Desc desc;
                    desc.Filename = filePath;
                    desc.Directory = relativePath;
					desc.TextureDirectory = texturePath;

                    loadingObjs.push_back(Resource::Async<Resource::Obj>::Load(desc));
                }
                else if (strcmp(ext, "fbx") == 0)
                {
                    Resource::Fbx::Desc desc;
                    desc.pFileName = filePath;
					desc.pTextureDirectory = texturePath;
					desc.eFlags = Resource::Fbx::FBX_FLAG_PACK_MATERIAL_TEXTUREW;

                    loadingFbxs.push_back(Resource::Async<Resource::Fbx>::Load(desc));
                }
            }

			TextureOverrides* pTextureOverride = &textures.emplace_back();
			pTextureOverride->Metal = pScene->GetTexture(asset.pTextures[SceneResource::TEXTURE_TYPE_METAL]);
			pTextureOverride->Rough = pScene->GetTexture(asset.pTextures[SceneResource::TEXTURE_TYPE_ROUGH]);
			pTextureOverride->Normal = pScene->GetTexture(asset.pTextures[SceneResource::TEXTURE_TYPE_NORMAL]);
        }

		static constexpr float min = (std::numeric_limits<float>::max)();
		static constexpr float max = (std::numeric_limits<float>::min)();

        Scene scene;
		scene.Bounds.min.x = min;
		scene.Bounds.min.y = min;
		scene.Bounds.min.z = min;
		scene.Bounds.max.x = max;
		scene.Bounds.max.y = max;
		scene.Bounds.max.z = max;
		scene.eShading = pScene->GetShadingMode();
		scene.eBump = pScene->GetBumpMode();

        for (size_t i = 0, n = loadingObjs.size(); i < n; ++i)
        {
            UpdateScene(loadingObjs[i].Get().get(), &scene, &textures[i]);
        }

        for (size_t i = 0, n = loadingFbxs.size(); i < n; ++i)
        {
            UpdateScene(loadingFbxs[i].Get().get(), &scene, &textures[i]);
        } 

        {
            std::lock_guard<std::mutex> lock(m_sceneMutex);
            if (m_pStagingScene)
            {
                DestroyScene(m_pStagingScene.get());
            }

            m_pStagingScene = std::make_shared<Scene>();
            m_pStagingScene->Bounds = scene.Bounds;
            m_pStagingScene->Meshes = scene.Meshes;
            m_pStagingScene->Materials = scene.Materials;
            m_pStagingScene->Constants = scene.Constants;
            m_pStagingScene->Surfaces = scene.Surfaces;
            m_pStagingScene->Textures = scene.Textures;
			m_pStagingScene->eShading = scene.eShading;
			m_pStagingScene->eBump = scene.eBump;
        }

        s_loading.store(false);
    });
}

void SceneViewer::FixedUpdate(double dt)
{

}

void ProcessEnvironmentLighting(const EnvironmentParameters& params)
{
    Graphics::PipelineDesc radiancePipe;
    radiancePipe.Signature.SetName("Radiance")
        .AllowInputLayout()
        .DenyHS()
        .DenyDS()
        .DenyGS()
        .AppendConstantView(0)
        .AppendConstantView(1)
        .AppendDescriptorTable(Graphics::SHADER_VISIBILITY_PS)
        .AppendDescriptorTableRange(2, 1, 0, 0, Graphics::DescriptorTable::Range::DESCRIPTOR_TABLE_RANGE_TYPE_SHADER_VIEW)
        .AppendLinearWrapSampler(0);
    radiancePipe.InputLayout.AppendPosition3F();
    radiancePipe.VertexShaderHandle = params.hCubeVertexShader;
    radiancePipe.PixelShaderHandle = params.hRadiancePixelShader;
    radiancePipe.Topology = Graphics::GFX_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    radiancePipe.SampleCount = 1;
    radiancePipe.SampleQuality = 0;
    radiancePipe.SampleMask = 0xffffffff;

    radiancePipe.DepthStencil = Graphics::DepthStencilState::Disabled;
    radiancePipe.Rasterizer = Graphics::RasterizerState::FillSolidCullCCW;
    radiancePipe.Blend.BlendStates[0] = Graphics::RenderTargetBlendState::Replace;
    radiancePipe.Blend.AlphaToCoverageEnable = false;
    radiancePipe.Blend.IndependentBlendEnable = false;

    radiancePipe.NumRenderTargets = 1;
    radiancePipe.pRenderTargets[0] = params.hRenderTarget;
    radiancePipe.DsHandle = 0;

    Graphics::PipelineStateHandle hRadiance = Graphics::CreateGraphicsPipelineState(radiancePipe);

    Graphics::PipelineDesc irradiancePipe;
    irradiancePipe.Signature.SetName("Irradiance")
        .AllowInputLayout()
        .DenyHS()
        .DenyDS()
        .DenyGS()
        .AppendConstantView(0)
        .AppendDescriptorTable(Graphics::SHADER_VISIBILITY_PS)
        .AppendDescriptorTableRange(1, 1, 0, 0, Graphics::DescriptorTable::Range::DESCRIPTOR_TABLE_RANGE_TYPE_SHADER_VIEW)
        .AppendLinearWrapSampler(0);
    irradiancePipe.InputLayout.AppendPosition3F();
    irradiancePipe.VertexShaderHandle = params.hCubeVertexShader;
    irradiancePipe.PixelShaderHandle = params.hIrradiancePixelShader;
    irradiancePipe.Topology = Graphics::GFX_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    irradiancePipe.SampleCount = 1;
    irradiancePipe.SampleQuality = 0;
    irradiancePipe.SampleMask = 0xffffffff;

    irradiancePipe.DepthStencil = Graphics::DepthStencilState::Disabled;
    irradiancePipe.Rasterizer = Graphics::RasterizerState::FillSolidCullCCW;
    irradiancePipe.Blend.BlendStates[0] = Graphics::RenderTargetBlendState::Replace;
    irradiancePipe.Blend.AlphaToCoverageEnable = false;
    irradiancePipe.Blend.IndependentBlendEnable = false;

    irradiancePipe.NumRenderTargets = 1;
    irradiancePipe.pRenderTargets[0] = params.hRenderTarget;
    irradiancePipe.DsHandle = 0;

    Graphics::PipelineStateHandle hIrradiance = Graphics::CreateGraphicsPipelineState(irradiancePipe);

    Graphics::PipelineDesc brdfPipe;
    brdfPipe.Signature.SetName("BRDF Integration")
        .DenyHS()
        .DenyDS()
        .DenyGS();
    brdfPipe.VertexShaderHandle = params.hTriVertexShader;
    brdfPipe.PixelShaderHandle = params.hDfgPixelShader;
    brdfPipe.Topology = Graphics::GFX_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    brdfPipe.SampleCount = 1;
    brdfPipe.SampleQuality = 0;
    brdfPipe.SampleMask = 0xffffffff;

    brdfPipe.DepthStencil = Graphics::DepthStencilState::Disabled;
    brdfPipe.Rasterizer = Graphics::RasterizerState::FillSolidCullCCW;
    brdfPipe.Blend.BlendStates[0] = Graphics::RenderTargetBlendState::Replace;
    brdfPipe.Blend.AlphaToCoverageEnable = false;
    brdfPipe.Blend.IndependentBlendEnable = false;

    brdfPipe.NumRenderTargets = 1;
    brdfPipe.pRenderTargets[0] = params.hDfgRenderTarget;
    brdfPipe.DsHandle = 0;

    Graphics::PipelineStateHandle hBrdf = Graphics::CreateGraphicsPipelineState(brdfPipe);

    float4x4 proj = float4x4::normalized_perspective_lh(3.14f * 0.5f, params.AspectRatio, 0.1f, 100.0f);
    float4x4 views[6] =
    {
        float4x4::look_at_lh(float3(+1.0f, 0.0f, 0.0), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 1.0f, 0.0)),
        float4x4::look_at_lh(float3(-1.0f, 0.0f, 0.0), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 1.0f, 0.0)),
        float4x4::look_at_lh(float3(0.0f, +1.0f, 0.0), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, -1.0)),
        float4x4::look_at_lh(float3(0.0f, -1.0f, 0.0), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, +1.0)),
        float4x4::look_at_lh(float3(0.0f, 0.0f, +1.0), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 1.0f, 0.0)),
        float4x4::look_at_lh(float3(0.0f, 0.0f, -1.0), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 1.0f, 0.0))
    };

    Graphics::ConstantBufferHandle hCameras[6];

    for (int i = 0; i < 6; ++i)
    {
		CameraConstants camera;
        camera.Proj = proj.transpose();
        camera.View = views[i].transpose();
        camera.InverseProj = proj.inverse().transpose();
        camera.InverseView = views[i].inverse().transpose();

        Graphics::ConstantBufferDesc desc;
        desc.Alignment = 0;
        desc.SizeInBytes = sizeof(CameraConstants);
        desc.StrideInBytes = sizeof(CameraConstants);
        desc.AllocHeap = false;
        desc.pData = &camera;

        hCameras[i] = Graphics::CreateConstantBuffer(desc);
    }

    float4 roughness[5];

    Graphics::ConstantBufferHandle hRoughness[5];

    for (int i = 0; i < 5; ++i)
    {
        roughness[i].x = (float)i / 4.0f;

        Graphics::ConstantBufferDesc desc;
        desc.Alignment = 0;
        desc.SizeInBytes = sizeof(float4);
        desc.StrideInBytes = sizeof(float4);
        desc.AllocHeap = false;
        desc.pData = &roughness[i];
        hRoughness[i] = Graphics::CreateConstantBuffer(desc);
    }

    Graphics::CommandStream* pCommandStream = params.pCommandStream;
	Graphics::ResetCommandStream(pCommandStream, hRadiance);

	pCommandStream->SetPrimitiveTopology(Graphics::GFX_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pCommandStream->TransitionTexture(params.hRenderTarget, Graphics::GFX_RESOURCE_STATE_GENERIC_READ, Graphics::GFX_RESOURCE_STATE_RENDER_TARGET);
    pCommandStream->SetDescriptorTable(2, &params.hCubeTexture, 1);

    for (u32 mip = 0; mip < 5; ++mip)
    {
        for (u32 face = 0; face < 6; ++face)
        {
            float w = 256.0f * powf(0.5f, (float)mip);
            float h = 256.0f * powf(0.5f, (float)mip);

            pCommandStream->SetRenderTargets(1, &params.hRenderTarget, &mip, &face, 0);
            pCommandStream->SetViewport(0.0f, 0.0f, w, h, 0.0, 1.0);
            pCommandStream->SetScissorRect(0, 0, static_cast<u32>(w), static_cast<u32>(h));
            pCommandStream->SetConstantBuffer(0, hCameras[face]);
            pCommandStream->SetConstantBuffer(1, hRoughness[mip]);

            pCommandStream->SetVertexBuffer(params.hCubeVertexBuffer);
            pCommandStream->SetIndexBuffer(params.hCubeIndexBuffer);
            pCommandStream->DrawInstanceIndexed(params.hCubeIndexBuffer);
        }
    }

    Graphics::SubmitCommandStream(pCommandStream);
	Graphics::ResetCommandStream(pCommandStream, hIrradiance);

    pCommandStream->SetPrimitiveTopology(Graphics::GFX_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pCommandStream->SetDescriptorTable(1, &params.hCubeTexture, 1);

    u32 irradianceMip = 5;
    for (u32 face = 0; face < 6; ++face)
    {
        float w = 256.0f * powf(0.5f, (float)irradianceMip);
        float h = 256.0f * powf(0.5f, (float)irradianceMip);

        pCommandStream->SetRenderTargets(1, &params.hRenderTarget, &irradianceMip, &face, 0);
        pCommandStream->SetViewport(0.0f, 0.0f, w, h, 0.0, 1.0);
        pCommandStream->SetScissorRect(0, 0, static_cast<u32>(w), static_cast<u32>(h));
        pCommandStream->SetConstantBuffer(0, hCameras[face]);

        pCommandStream->SetVertexBuffer(params.hCubeVertexBuffer);
        pCommandStream->SetIndexBuffer(params.hCubeIndexBuffer);
        pCommandStream->DrawInstanceIndexed(params.hCubeIndexBuffer);
    }

    Graphics::SubmitCommandStream(pCommandStream);
	Graphics::ResetCommandStream(pCommandStream, hBrdf);

    pCommandStream->TransitionTexture(params.hRenderTarget, Graphics::GFX_RESOURCE_STATE_RENDER_TARGET, Graphics::GFX_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | Graphics::GFX_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    // brdf integration
    pCommandStream->TransitionTexture(params.hDfgRenderTarget, Graphics::GFX_RESOURCE_STATE_GENERIC_READ, Graphics::GFX_RESOURCE_STATE_RENDER_TARGET);

    pCommandStream->SetPrimitiveTopology(Graphics::GFX_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pCommandStream->SetRenderTargets(1, &params.hDfgRenderTarget, nullptr, nullptr, 0);
    pCommandStream->SetViewport(0.0f, 0.0f, 512.0, 512.0, 0.0, 1.0);
    pCommandStream->SetScissorRect(0, 0, 512, 512);
    pCommandStream->SetVertexBuffer(0);
    pCommandStream->SetIndexBuffer(params.hTriIndexBuffer);
    pCommandStream->DrawInstanceIndexed(params.hTriIndexBuffer);

    pCommandStream->TransitionTexture(params.hDfgRenderTarget, Graphics::GFX_RESOURCE_STATE_RENDER_TARGET, Graphics::GFX_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | Graphics::GFX_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    Graphics::SubmitCommandStream(pCommandStream, true);

    Graphics::ReleasePipeline(hRadiance);
    Graphics::ReleasePipeline(hIrradiance);
    Graphics::ReleasePipeline(hBrdf);

    for (int i = 0; i < 5; ++i)
    {
        Graphics::ReleaseConstantBuffer(hRoughness[i]);
    }
    for (int i = 0; i < 6; ++i)
    {
        Graphics::ReleaseConstantBuffer(hCameras[i]);
    }
}

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

void UpdateScene(const Resource::Obj* pObj, Scene* pScene, TextureOverrides* pTextureOverrides)
{
    LUZASSERT(pObj != nullptr);

    // Surfaces
    size_t iSurfaceStart = pScene->Meshes.size();
    size_t nSurfaces = static_cast<size_t>(pObj->GetNumSurfaces());
    pScene->Meshes.resize(iSurfaceStart + nSurfaces);
    Graphics::CreateSurfaces<Vertex, u32>(pObj, &pScene->Meshes[iSurfaceStart], pObj->GetNumSurfaces());

    size_t iSceneSurfaceStart = pScene->Surfaces.size();
    pScene->Surfaces.resize(iSceneSurfaceStart + nSurfaces);

    for (size_t iSurfaceOffset = 0; iSurfaceOffset < nSurfaces; ++iSurfaceOffset)
    {
        Graphics::Surface<Vertex, u32>* pSurface = &pScene->Meshes[iSurfaceStart + iSurfaceOffset];

        Graphics::BufferDesc vbd;
        vbd.Alignment = 0;
        vbd.SizeInBytes = static_cast<u64>(sizeof(Vertex) * pSurface->Vertices.size());
        vbd.StrideInBytes = sizeof(Vertex);
        vbd.pData = pSurface->Vertices.data();

        Graphics::BufferDesc ibd;
        ibd.Alignment = 0;
        ibd.SizeInBytes = static_cast<u64>(sizeof(u32) * pSurface->Indices.size());
        ibd.StrideInBytes = sizeof(u32);
        ibd.pData = pSurface->Indices.data();

        Surface* pSceneSurface = &pScene->Surfaces[iSceneSurfaceStart + iSurfaceOffset];
        pSceneSurface->vb = Graphics::CreateVertexBuffer(vbd);
        pSceneSurface->ib = Graphics::CreateIndexBuffer(ibd);
    }

    // Materials
    size_t iMaterialStart = pScene->Materials.size();
    size_t nMaterials = static_cast<size_t>(pObj->GetNumMaterials());
    pScene->Materials.resize(iMaterialStart + nMaterials);

    size_t iConstantStart = pScene->Constants.size();
    size_t nConstants = nMaterials;
    pScene->Constants.resize(iConstantStart + nConstants);

    // Cache this for later
    size_t iTextureStart = pScene->TextureNames.size();

	const Resource::Mtl::MaterialDesc* pMds = pObj->GetMaterials();

    for (size_t iMaterialOffset = 0; iMaterialOffset < nMaterials; ++iMaterialOffset)
    {
		const Resource::Mtl::MaterialDesc md = pMds[iMaterialOffset];
        
        MaterialConstants& mc = pScene->Materials[iMaterialStart + iMaterialOffset];
        mc.SpecularExponent = md.SpecularExponent;
        mc.Transparency = md.Transparency;
        mc.OpticalDensity = md.OpticalDensity;
        mc.Dissolve = md.Dissolve;
        mc.Specular = float3(md.Specular[0], md.Specular[1], md.Specular[2]);
        mc.TransmissionFilter = float3(md.TransmissionFilter[0], md.TransmissionFilter[1], md.TransmissionFilter[2]);
        mc.Ambient = float3(md.Ambient[0], md.Ambient[1], md.Ambient[2]);
        mc.Diffuse = float3(md.Diffuse[0], md.Diffuse[1], md.Diffuse[2]);
        mc.Emissive = float3(md.Emissive[0], md.Emissive[1], md.Emissive[2]);

        if (md.AmbientTextureName)
            mc.iAmbient = FindOrPushBackTextureName(pScene->TextureNames, md.AmbientTextureName);
        
        if (md.DiffuseTextureName)
            mc.iDiffuse = FindOrPushBackTextureName(pScene->TextureNames, md.DiffuseTextureName);
        
        if (md.SpecularTextureName)
            mc.iSpecular = FindOrPushBackTextureName(pScene->TextureNames, md.SpecularTextureName);

		if (md.BumpTextureName0)
			mc.iBump = FindOrPushBackTextureName(pScene->TextureNames, md.BumpTextureName0);

		if (md.BumpTextureName1)
			mc.iNormal = FindOrPushBackTextureName(pScene->TextureNames, md.BumpTextureName1);

		if (md.DissolveTextureName)
			mc.iAlpha = FindOrPushBackTextureName(pScene->TextureNames, md.DissolveTextureName);

        if (md.SpecularPowerTextureName)
            mc.iShininess = FindOrPushBackTextureName(pScene->TextureNames, md.SpecularPowerTextureName);

		// Overrides
		if (md.AmbientTextureName)
			mc.iMetal = FindOrPushBackTextureName(pScene->TextureNames, md.AmbientTextureName);

		if (md.SpecularPowerTextureName)
			mc.iRough = FindOrPushBackTextureName(pScene->TextureNames, md.SpecularPowerTextureName);

		///if (const char* pMetalTexture = pTextureOverrides->Metal)
		///	mc.iMetal = FindOrPushBackTextureName(pScene->TextureNames, pMetalTexture);
		///
		///if (const char* pNormalTexture = pTextureOverrides->Normal)
		///	mc.iNormal = FindOrPushBackTextureName(pScene->TextureNames, pNormalTexture);
		///
		///if (const char* pRoughTexture = pTextureOverrides->Rough)
		///	mc.iRough = FindOrPushBackTextureName(pScene->TextureNames, pRoughTexture);

        Graphics::ConstantBufferDesc cbd;
        cbd.Alignment = 0;
        cbd.SizeInBytes = sizeof(MaterialConstants);
        cbd.StrideInBytes = sizeof(MaterialConstants);
        cbd.AllocHeap = true;
        cbd.pData = &mc;
        pScene->Constants[iConstantStart + iMaterialOffset] = Graphics::CreateConstantBuffer(cbd);
    }

    // Create TextureHandles
    size_t nTextures = pScene->TextureNames.size() - iTextureStart;
    pScene->Textures.resize(iTextureStart + nTextures);

    for (size_t iTextureOffset = 0; iTextureOffset < nTextures; ++iTextureOffset)
    {
		char pFilePath[256];
		strcpy_s(pFilePath, pObj->GetTextureDirectory());
		strcat_s(pFilePath, pScene->TextureNames[iTextureStart + iTextureOffset].c_str());

        Graphics::TextureFileDesc td;
        td.Filename = pFilePath;
        td.GenMips = true;
        pScene->Textures[iTextureStart + iTextureOffset] = Graphics::CreateTexture(td);
    }

	Graphics::CreateRange3D(pScene->Bounds.min.p_cols, pScene->Bounds.max.p_cols, pObj->GetPositions()->Data, pObj->GetNumPositions());
}

void UpdateScene(const Resource::Fbx* pFbx, Scene* pScene, TextureOverrides* pTextureOverrides)
{
    LUZASSERT(pFbx != nullptr);

    // Surfaces
    size_t iSurfaceStart = pScene->Meshes.size();
    size_t nSurfaces = static_cast<size_t>(pFbx->GetNumSurfaces());
    pScene->Meshes.resize(iSurfaceStart + nSurfaces);
    Graphics::CreateSurfaces<Vertex, u32>(pFbx, &pScene->Meshes[iSurfaceStart], pFbx->GetNumSurfaces());

    size_t iSceneSurfaceStart = pScene->Surfaces.size();
    pScene->Surfaces.resize(iSceneSurfaceStart + nSurfaces);

    for (size_t iSurfaceOffset = 0; iSurfaceOffset < nSurfaces; ++iSurfaceOffset)
    {
        Graphics::Surface<Vertex, u32>* pSurface = &pScene->Meshes[iSurfaceStart + iSurfaceOffset];

        Graphics::BufferDesc vbd;
        vbd.Alignment = 0;
        vbd.SizeInBytes = static_cast<u64>(sizeof(Vertex) * pSurface->Vertices.size());
        vbd.StrideInBytes = sizeof(Vertex);
        vbd.pData = pSurface->Vertices.data();

        Graphics::BufferDesc ibd;
        ibd.Alignment = 0;
        ibd.SizeInBytes = static_cast<u64>(sizeof(u32) * pSurface->Indices.size());
        ibd.StrideInBytes = sizeof(u32);
        ibd.pData = pSurface->Indices.data();

        Surface* pSceneSurface = &pScene->Surfaces[iSceneSurfaceStart + iSurfaceOffset];
        pSceneSurface->vb = Graphics::CreateVertexBuffer(vbd);
        pSceneSurface->ib = Graphics::CreateIndexBuffer(ibd);
    }

    // Materials
    size_t iMaterialStart = pScene->Materials.size();
    size_t nMaterials = static_cast<size_t>(pFbx->GetNumMaterials());
    pScene->Materials.resize(iMaterialStart + nMaterials);

    size_t iConstantStart = pScene->Constants.size();
    size_t nConstants = nMaterials;
    pScene->Constants.resize(iConstantStart + nConstants);

    // Cache this for later
    size_t iTextureStart = pScene->TextureNames.size();

	const Resource::Fbx::Material* pMaterials = pFbx->GetMaterials();

    for (size_t iMaterialOffset = 0; iMaterialOffset < nMaterials; ++iMaterialOffset)
    {
		const Resource::Fbx::Material& md = pMaterials[iMaterialOffset];

        MaterialConstants& mc = pScene->Materials[iMaterialStart + iMaterialOffset];
        mc.SpecularExponent = md.SpecularFactor;
        mc.Transparency = md.TransparencyFactor;
        mc.OpticalDensity = md.ReflectionFactor;
        mc.Dissolve = md.TransparencyFactor;
        mc.Specular = float3(md.SpecularColor[0], md.SpecularColor[1], md.SpecularColor[2]);
        mc.TransmissionFilter = float3(md.TransparentColor[0], md.TransparentColor[1], md.TransparentColor[2]);
        mc.Ambient = float3(md.AmbientColor[0], md.AmbientColor[1], md.AmbientColor[2]);
        mc.Diffuse = float3(md.DiffuseColor[0], md.DiffuseColor[1], md.DiffuseColor[2]);
        mc.Emissive = float3(md.EmissiveColor[0], md.EmissiveColor[1], md.EmissiveColor[2]);

		if (const char* pAmbientTexture = pFbx->GetTextureFileName(md.iAmbient))
			mc.iAmbient = FindOrPushBackTextureName(pScene->TextureNames, pAmbientTexture);

		if (const char* pDiffuseTexture = pFbx->GetTextureFileName(md.iDiffuse))
			mc.iDiffuse = FindOrPushBackTextureName(pScene->TextureNames, pDiffuseTexture);

		if (const char* pSpecularTexture = pFbx->GetTextureFileName(md.iSpecular))
			mc.iSpecular = FindOrPushBackTextureName(pScene->TextureNames, pSpecularTexture);

		if (const char* pNormalTexture = pFbx->GetTextureFileName(md.iNormal))
			mc.iNormal = FindOrPushBackTextureName(pScene->TextureNames, pNormalTexture);

		if (const char* pBumpTexture = pFbx->GetTextureFileName(md.iBump))
			mc.iBump = FindOrPushBackTextureName(pScene->TextureNames, pBumpTexture);

		if (const char* pAlphaTexture = pFbx->GetTextureFileName(md.iAlpha))
			mc.iAlpha = FindOrPushBackTextureName(pScene->TextureNames, pAlphaTexture);

		if (const char* pEmissiveTexture = pFbx->GetTextureFileName(md.iEmissive))
			mc.iEmissive = FindOrPushBackTextureName(pScene->TextureNames, pEmissiveTexture);

		if (const char* pReflectTexture = pFbx->GetTextureFileName(md.iReflection))
			mc.iReflection = FindOrPushBackTextureName(pScene->TextureNames, pReflectTexture);

		if (const char* pShininess = pFbx->GetTextureFileName(md.iShininess))
			mc.iShininess = FindOrPushBackTextureName(pScene->TextureNames, pShininess);

		// Overrides

		if (const char* pMetalTexture = pTextureOverrides->Metal)
			mc.iMetal = FindOrPushBackTextureName(pScene->TextureNames, pMetalTexture);

		if (const char* pNormalTexture = pTextureOverrides->Normal)
			mc.iNormal = FindOrPushBackTextureName(pScene->TextureNames, pNormalTexture);

		if (const char* pRoughTexture = pTextureOverrides->Rough)
			mc.iRough = FindOrPushBackTextureName(pScene->TextureNames, pRoughTexture);

        Graphics::ConstantBufferDesc cbd;
        cbd.Alignment = 0;
        cbd.SizeInBytes = sizeof(MaterialConstants);
        cbd.StrideInBytes = sizeof(MaterialConstants);
        cbd.AllocHeap = true;
        cbd.pData = &mc;
        pScene->Constants[iConstantStart + iMaterialOffset] = Graphics::CreateConstantBuffer(cbd);
    }

    // Create TextureHandles
    size_t nTextures = pScene->TextureNames.size() - iTextureStart;
    pScene->Textures.resize(iTextureStart + nTextures);

    for (size_t iTextureOffset = 0; iTextureOffset < nTextures; ++iTextureOffset)
    {
		char pFilePath[256];
		strcpy_s(pFilePath, pFbx->GetTextureDirectory());
		strcat_s(pFilePath, pScene->TextureNames[iTextureStart + iTextureOffset].c_str());

        Graphics::TextureFileDesc td;
		td.Filename = pFilePath;
        td.GenMips = true;
        pScene->Textures[iTextureStart + iTextureOffset] = Graphics::CreateTexture(td);
    }

	Graphics::CreateRange3D(pScene->Bounds.min.p_cols, pScene->Bounds.max.p_cols, pFbx->GetPositions()->Data, pFbx->GetNumPositions());
}

void DestroyScene(const Scene* pScene)
{
    if (!pScene) return;

    for (auto& surface : pScene->Surfaces)
    {
        Graphics::ReleaseVertexBuffer(surface.vb);
        Graphics::ReleaseIndexBuffer(surface.ib);
    }

    for (auto& hConstant : pScene->Constants)
    {
        Graphics::ReleaseConstantBuffer(hConstant);
    }

    for (auto& hTexture : pScene->Textures)
    {
        Graphics::ReleaseTexture(hTexture);
    }
}

void ConsoleThread()
{
    Platform::CreateConsole();

    while (Platform::Running())
    {
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

		ShaderOptions shaderOptions;
		{
			std::lock_guard<std::mutex> lock(s_shaderOptionMutex);
			shaderOptions = s_shaderOptions;
		}

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
        else if (strcmp(cmd.c_str(), "setlightcolor") == 0)
        {
            ss >> shaderOptions.LightColor.x >> shaderOptions.LightColor.y >> shaderOptions.LightColor.z;
        }
        else if (strcmp(cmd.c_str(), "setexposure") == 0)
        {
            ss >> shaderOptions.Exposure;
        }
        else if (strcmp(cmd.c_str(), "setambient") == 0)
        {
            ss >> shaderOptions.LightIntensity.x;
        }
        else if (strcmp(cmd.c_str(), "setdiffuse") == 0)
        {
            ss >> shaderOptions.LightIntensity.y;
        }
        else if (strcmp(cmd.c_str(), "setspecular") == 0)
        {
            ss >> shaderOptions.LightIntensity.z;
        }
        else if (strcmp(cmd.c_str(), "setlux") == 0)
        {
            ss >> shaderOptions.LightIntensity.w;
        }
        else if (strcmp(cmd.c_str(), "toggleambient") == 0)
        {
            shaderOptions.AmbientEnabled = !shaderOptions.AmbientEnabled;
        }
        else if (strcmp(cmd.c_str(), "togglediffuse") == 0)
        {
            shaderOptions.DiffuseEnabled = !shaderOptions.DiffuseEnabled;
        }
        else if (strcmp(cmd.c_str(), "togglespecular") == 0)
        {
            shaderOptions.SpecEnabled = !shaderOptions.SpecEnabled;
        }
        else if (strcmp(cmd.c_str(), "toggleshadow") == 0)
        {
            shaderOptions.ShadowEnabled = !shaderOptions.ShadowEnabled;
        }
        else if (strcmp(cmd.c_str(), "togglebump") == 0)
        {
            shaderOptions.BumpEnabled = !shaderOptions.BumpEnabled;
        }
        else if (strcmp(cmd.c_str(), "toggled") == 0)
        {
            shaderOptions.MicrofacetEnabled = !shaderOptions.MicrofacetEnabled;
        }
        else if (strcmp(cmd.c_str(), "togglev") == 0)
        {
            shaderOptions.MaskingEnabled = !shaderOptions.MaskingEnabled;
        }
        else if (strcmp(cmd.c_str(), "togglef") == 0)
        {
            shaderOptions.FresnelEnabled = !shaderOptions.FresnelEnabled;
        }
        else if (strcmp(cmd.c_str(), "shadedefault") == 0)
        {
            shaderOptions.LightingMode = 0;
        }
        else if (strcmp(cmd.c_str(), "shadeblinnphong") == 0)
        {
            shaderOptions.LightingMode = 1;
        }
        else if (strcmp(cmd.c_str(), "shadebeckmann") == 0)
        {
            shaderOptions.LightingMode = 2;
        }
        else if (strcmp(cmd.c_str(), "shadeggx") == 0)
        {
            shaderOptions.LightingMode = 3;
        }
        else if (strcmp(cmd.c_str(), "setbumpheight") == 0)
        {
            shaderOptions.BumpMode = 0;
        }
        else if (strcmp(cmd.c_str(), "setbumpnormal") == 0)
        {
            shaderOptions.BumpMode = 1;
        }
        else if (strcmp(cmd.c_str(), "reset") == 0)
        {
            shaderOptions.AmbientEnabled = true;
            shaderOptions.DiffuseEnabled = true;
            shaderOptions.SpecEnabled = true;
            shaderOptions.BumpEnabled = true;
            shaderOptions.ShadowEnabled = true;
            shaderOptions.MaskingEnabled = true;
            shaderOptions.MicrofacetEnabled = true;
            shaderOptions.FresnelEnabled = true;
        }
        else if (strcmp(cmd.c_str(), "loadscene") == 0)
        {
            if (!s_loading.load())
            {
                std::string filename;
                ss >> filename;
                s_pSceneViewer->LoadScene(filename,0);
            }
            else
            {
                std::cout << "scene is already queued" << std::endl;
            }
        }
        else
        {
            continue;
        }

		{
			std::lock_guard<std::mutex> lock(s_shaderOptionMutex);
			s_shaderOptions = shaderOptions;
		}
    }
}

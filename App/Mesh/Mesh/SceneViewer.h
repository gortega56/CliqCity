#pragma once
#ifndef SCENEVIEWER_H
#define SCENEVIEWER_H
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <atomic>

#include <wrl.h>
#include "GeneralMacros.h"

#ifndef IAPPLICATION_H
#include "IApplication.h"
#endif

#ifndef TYPEDEFS_H
#include "TypeDefs.h"
#endif

#ifndef GRAPHICSTYPES_H
#include "GraphicsTypes.h"
#endif

#ifndef LINA_H
#include "lina.h"
#endif

#ifndef CAMERACONTROLLER_H
#include "CameraController.h"
#endif

#ifndef COMMANDSTREAM_H
#include "CommandStream.h"
#endif

#ifndef MESH_H
#include "Mesh.h"
#endif

class SceneResource;
class Window;

struct Vertex
{
    float4 Tangent;
    float3 Position;
    float3 Normal;
    float3 UV;

    Vertex(float px, float py, float pz, float nx, float ny, float nz, float u, float v);

    Vertex() = default;
};

struct CameraConstants
{
    float4x4 View;
    float4x4 Proj;
    float4x4 InverseView;
    float4x4 InverseProj;
};

struct LightingConstants
{
    float3 Color;
    float _padding0;
    float3 Direction;
    float _padding1;
    float4 Intensity; // ambient, diffuse, spec, lx
    float Exposure;
    u32 LightingMode;
    u32 BumpMode;
    u32 EnableAmbient;
    u32 EnableDiffuse;
    u32 EnableSpec;
    u32 EnableBump;
    u32 EnableShadow;
    u32 MicrofacetEnabled;
    u32 MaskingEnabled;
    u32 FresnelEnabled;
};

struct MaterialConstants
{
    float3 Specular;   
    float SpecularExponent;       
    float3 TransmissionFilter;  
    float Transparency;           
    float3 Ambient;           
    float OpticalDensity;         
    float3 Diffuse;             
    float Dissolve;               
    float3 Emissive;
    float _unused0;
	int iAmbient	= -1;
    int iDiffuse	= -1;
	int iSpecular	= -1;
	int iBump		= -1;
	int iNormal		= -1;
	int iAlpha		= -1;
	int iEmissive	= -1;
	int iShininess	= -1;
	int iReflection = -1;
	int iMetal		= -1;
    int iRough		= -1;
    float _unused1[33];
};

static_assert(sizeof(MaterialConstants) == 256, "MaterialConstants must be 256 bytes");

struct Surface
{
    Graphics::VertexBufferHandle vb;
    Graphics::IndexBufferHandle ib;
};

struct FrameConstants
{
    CameraConstants CameraConsts;
    CameraConstants ShadowConsts;
    LightingConstants LightingConsts;

    Graphics::ConstantBufferHandle hCamera;
    Graphics::ConstantBufferHandle hShadow;
    Graphics::ConstantBufferHandle hLighting;
};

struct Scene
{
    std::vector<Graphics::Surface<Vertex, u32>> Meshes;
    std::vector<MaterialConstants> Materials;
    std::vector<Surface> Surfaces;
    std::vector<std::string> TextureNames;

    std::vector<Graphics::ConstantBufferHandle> Constants;
    std::vector<Graphics::TextureHandle> Textures;
    range3 Bounds;
	unsigned short eShading;
	unsigned short eBump;
};

class SceneViewer :
    public IApplication, public std::enable_shared_from_this<SceneViewer>
{
public:
    using IApplication::IApplication;

    static constexpr u32 s_nSwapChainTargets = 3;

    static constexpr u32 s_nFrameResources = s_nSwapChainTargets;

	Platform::WindowHandle m_window;

	std::shared_ptr<Scene> m_pScene;

	std::shared_ptr<Scene> m_pStagingScene;

	std::mutex m_sceneMutex;

	FrameConstants m_frameConsts[s_nFrameResources];

    u32 m_frameIndex;

    Luz::CameraController m_cameraController;

    Graphics::CommandStream m_commandStream;

    Graphics::ShaderHandle m_vsScene;
    Graphics::ShaderHandle m_psScene;
    Graphics::ShaderHandle m_vsFullScreen;
    Graphics::ShaderHandle m_psFullScreen;
    Graphics::ShaderHandle m_vsCubeMap;
    Graphics::ShaderHandle m_psCubeMap;
    Graphics::ShaderHandle m_psIrradianceMap;
    Graphics::ShaderHandle m_psRadianceMap;
    Graphics::ShaderHandle m_psLUT;

    Graphics::VertexBufferHandle m_vbCubeMap;
    Graphics::IndexBufferHandle m_ibCubeMap;
    Graphics::IndexBufferHandle m_ibFullScreen;

    Graphics::TextureHandle m_txCubeMap;
    Graphics::TextureHandle m_txEnvMap;
    Graphics::TextureHandle m_txLUT;

    Graphics::DepthStencilHandle m_txShadow;

    Graphics::PipelineStateHandle m_opaquePipeline;
    Graphics::PipelineStateHandle m_shadowPipeline;
    Graphics::PipelineStateHandle m_fullScreenPipeline;
    Graphics::PipelineStateHandle m_cubemapPipeline;

	std::vector<std::thread> m_loadingThreads;

    SceneViewer();

    ~SceneViewer();

    bool Initialize() override;
    
    int Shutdown() override;

    void Update(double delta) override;
    
    void FixedUpdate(double delta) override;

    void LoadScene(const std::string filename, const u32 threadID);
};

#endif
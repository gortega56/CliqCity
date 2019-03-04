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
    int iMetal = -1;
    int iDiffuse = -1;
    int iSpec = -1;
    int iRough = -1;
    int iBump = -1;
    int iNormal = -1;
    float _unused1[38];
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
    std::vector<Graphics::Mesh<Vertex, u32>> Meshes;
    std::vector<MaterialConstants> Materials;
    std::vector<Surface> Surfaces;
    std::vector<Graphics::ConstantBufferHandle> Constants;
    std::vector<Graphics::TextureHandle> Textures;
    range3 Bounds;
};

class SceneViewer :
    public IApplication, public std::enable_shared_from_this<SceneViewer>
{
public:
    using IApplication::IApplication;

    static constexpr u32 s_nSwapChainTargets = 3;

    static constexpr u32 s_nFrameResources = s_nSwapChainTargets;

    FrameConstants m_frameConsts[s_nFrameResources];

    u32 m_frameIndex;

    Luz::CameraController m_cameraController;

    std::shared_ptr<Window> m_window;

    Graphics::CommandStream m_commandStream;

    Graphics::ShaderHandle m_vs;
    Graphics::ShaderHandle m_ps;
    Graphics::ShaderHandle m_fs_vs;
    Graphics::ShaderHandle m_fs_ps;
    Graphics::ShaderHandle m_cube_map_vs;
    Graphics::ShaderHandle m_cube_map_ps;
    Graphics::ShaderHandle m_irradiance_map_ps;
    Graphics::ShaderHandle m_radiance_map_ps;
    Graphics::ShaderHandle m_brdf_map_ps;

    Graphics::VertexBufferHandle m_cube_map_vb_handle;
    Graphics::IndexBufferHandle m_cube_map_ib_handle;
    Graphics::IndexBufferHandle m_fs_ib;

    Graphics::TextureHandle m_cube_map_texture_handle;
    Graphics::TextureHandle m_environment_cube_map_handle;
    Graphics::TextureHandle m_environment_brdf_map_handle;

    Graphics::DepthStencilHandle m_shadowTexture;

    Graphics::PipelineStateHandle m_opaquePipeline;
    Graphics::PipelineStateHandle m_shadowPipeline;
    Graphics::PipelineStateHandle m_fullScreenPipeline;
    Graphics::PipelineStateHandle m_cubemapPipeline;

    SceneViewer();
    ~SceneViewer();

    bool Initialize() override;
    int Shutdown() override;

    void Update(double delta) override;
    void FixedUpdate(double delta) override;

    void LoadScene(const std::string filename, const u32 threadID);
    
    std::mutex m_sceneMutex;
    std::shared_ptr<Scene> m_pScene;
    std::shared_ptr<Scene> m_pStagingScene;

    std::vector<std::thread> m_loadingThreads;
};

#endif
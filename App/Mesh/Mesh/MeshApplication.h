#pragma once
#ifndef MESHAPPLICATION_H
#define MESHAPPLICATION_H
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

class Window;

struct Vertex
{
    float4 Tangent;
    float3 Position;
    float3 Normal;
    float3 UV;
    Vertex(float px, float py, float pz,
        float nx, float ny, float nz,
        float u, float v) :
        Tangent(0.0f, 0.0f, 0.0f, 0.0f),
        Position(px, py, pz),
        Normal(nx, ny, nz),
        UV(u, v, -1.0f) {}
    Vertex() {}
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
    float4 Intensity; // ambient, diffuse, spec
    u32 EnableAmbient;
    u32 EnableDiffuse;
    u32 EnableSpec;
    u32 EnableBump;
    u32 EnableShadow;
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
    float _padding0;
    int TextureIndices[4] = { -1, -1, -1, -1 };
    float4 _padding2[10];    
};

struct Surface
{
    Graphics::VertexBufferHandle vb;
    Graphics::IndexBufferHandle ib;
};

class MeshApplication :
    public IApplication, public std::enable_shared_from_this<MeshApplication>
{
public:
    using IApplication::IApplication;

    std::shared_ptr<Window> m_window;

    Graphics::CommandStream m_commandStream;

    Graphics::ShaderHandle m_vs;
    Graphics::ShaderHandle m_ps;
    Graphics::ShaderHandle m_fs_vs;
    Graphics::ShaderHandle m_fs_ps;

    Graphics::PipelineStateHandle m_opaquePipeline;
    Graphics::PipelineStateHandle m_shadowPipeline;
    Graphics::PipelineStateHandle m_fullScreenPipeline;

    Graphics::IndexBufferHandle m_fs_ib;

    Graphics::ConstantBufferHandle m_baseDescriptorHandle;
    Graphics::ConstantBufferHandle m_cameraHandle;
    Graphics::ConstantBufferHandle m_lightHandle;
    Graphics::ConstantBufferHandle m_shadowHandle;
    Graphics::DepthStencilHandle m_shadowTexture;

    CameraConstants m_cameraConsts;
    CameraConstants m_shadowConsts;
    LightingConstants m_lightingConsts;
    std::vector<MaterialConstants> m_materialConstants;

    Luz::CameraController m_cameraController;

    std::vector<i32> m_materialIndices;
    std::vector<Surface> m_surfaces;

    range3 m_sceneBounds;

    MeshApplication();
    ~MeshApplication();

    bool Initialize() override;
    int Shutdown() override;

    void Update(double delta) override;
    void FixedUpdate(double delta) override;
};

#endif
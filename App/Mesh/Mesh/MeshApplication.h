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

struct ConstantBufferData
{
    float4x4 view;
    float4x4 proj;
    float4x4 inverseView;
    float4x4 inverseProj;
};

struct MaterialIndex
{
    u32 Index;
    float3 _padding;
};

struct PhongMaterial
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

struct Light
{
    float3 Color;
    float _p0;
    float3 Direction;
    float _p1;

    Light(float3 C, float3 D)
        : Color(C)
        , Direction(D)
    {

    }
};

class MeshApplication :
    public IApplication, public std::enable_shared_from_this<MeshApplication>
{
public:
    using IApplication::IApplication;

    std::shared_ptr<Window> m_window;

    Graphics::ShaderHandle m_vs;
    Graphics::ShaderHandle m_ps;
    Graphics::ShaderHandle m_fs_vs;
    Graphics::ShaderHandle m_fs_ps;

    Graphics::PipelineStateHandle m_opaquePipeline;
    Graphics::PipelineStateHandle m_shadowPipeline;
    Graphics::PipelineStateHandle m_fullScreenPipeline;

    Graphics::IndexBufferHandle m_fs_ib;
    std::vector<Surface> m_surfaces;

    Graphics::ConstantBufferHandle m_viewProjectionHandle;
    Graphics::ConstantBufferHandle m_baseDescriptorHandle;
    Graphics::ConstantBufferHandle m_lightHandle;
    Graphics::ConstantBufferHandle m_lightViewProjHandle;
    Graphics::DepthStencilHandle m_shadowTexture;

    MaterialIndex m_materialIndex;
    ConstantBufferData m_cbvData;
    ConstantBufferData m_shadowCbvData;
    std::vector<PhongMaterial> m_materialConstants;

    Luz::CameraController m_cameraController;

    std::vector<i32> m_materialIndices;

    i32 m_renderableIndex = -1;

    range3 m_sceneBounds;

    MeshApplication();
    ~MeshApplication();

    bool Initialize() override;
    int Shutdown() override;

    void Update(double delta) override;
    void FixedUpdate(double delta) override;
};

#endif
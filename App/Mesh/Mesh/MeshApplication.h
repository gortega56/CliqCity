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

#ifndef GMATH_H
#include "GMath.h"
#endif

#ifndef CAMERACONTROLLER_H
#include "CameraController.h"
#endif

class Window;

struct Vertex
{
    gmath::float4 Tangent;
    gmath::float3 Position;
    gmath::float3 Normal;
    gmath::float3 UV;
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
    gmath::float4x4 view;
    gmath::float4x4 proj;
    gmath::float4x4 inverseView;
    gmath::float4x4 inverseProj;
};

struct MaterialIndex
{
    u32 Index;
    gmath::float3 _padding;
};

struct PhongMaterial
{
    gmath::float3 Specular;   
    float SpecularExponent;       
    gmath::float3 TransmissionFilter;  
    float Transparency;           
    gmath::float3 Ambient;           
    float OpticalDensity;         
    gmath::float3 Diffuse;             
    float Dissolve;               
    gmath::float3 Emissive;
    float _padding0;
    int TextureIndices[4] = { -1, -1, -1, -1 };
    gmath::float4 _padding2[10];    
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

    Graphics::ShaderHandle m_vs;
    Graphics::ShaderHandle m_ps;
    Graphics::PipelineStateHandle m_opaquePipeline;
    Graphics::PipelineStateHandle m_transparentPipeline;
    std::vector<Surface> m_surfaces;

    Graphics::ConstantBufferHandle m_viewProjectionHandle;
    Graphics::ConstantBufferHandle m_baseDescriptorHandle;

    MaterialIndex m_materialIndex;
    ConstantBufferData m_cbvData;
    std::vector<PhongMaterial> m_materialConstants;

    Luz::CameraController m_cameraController;

    std::vector<i32> m_materialIndices;

    i32 m_renderableIndex = -1;

    MeshApplication();
    ~MeshApplication();

    bool Initialize() override;
    int Shutdown() override;

    void Update(double delta) override;
    void FixedUpdate(double delta) override;
};

#endif
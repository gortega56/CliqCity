#pragma once
#ifndef MESHAPPLICATION_H
#define MESHAPPLICATION_H
#include <unordered_set>
#include <mutex>
#include <wrl.h>
#include "GeneralMacros.h"

#ifndef IAPPLICATION_H
#include "IApplication.h"
#endif

#ifndef TYPEDEFS_H
#include "TypeDefs.h"
#endif

#ifndef DX12_SHADER_H
#include "DirectX12\Shader.h"
#endif

#ifndef DX12_PIPELINESTATE_H
#include "DirectX12\PipelineState.h"
#endif

#ifndef DX12_SHADER_H
#include "DirectX12\Shader.h"
#endif

#ifndef DX12_ROOTSIGNATURE_H
#include "DirectX12\RootSignature.h"
#endif

#ifndef RENDERABLE_H
#include "Renderable.h"
#endif

#ifndef MATERIAL_H
#include "Material.h"
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
};

struct PhongMaterial
{
    float SpecularExponent;       
    float Transparency;           
    float OpticalDensity;         
    float Dissolve;               
    gmath::float3 Specular;            
    gmath::float3 TransmissionFilter;  
    gmath::float3 Ambient;           
    gmath::float3 Diffuse;             
    gmath::float3 Emissive;            
};

class MeshApplication :
    public IApplication, public std::enable_shared_from_this<MeshApplication>
{
public:
    using IApplication::IApplication;

    std::shared_ptr<Window> m_window;

    Dx12::Shader m_vs;
    Dx12::Shader m_ps;
    Dx12::GraphicsPipeline m_pipeline;

    std::shared_ptr<Dx12::RootSignature> m_rs;

    std::vector<std::shared_ptr<Renderable>> m_renderables;
    std::vector<std::shared_ptr<MaterialState>> m_materials;

    std::shared_ptr<MaterialState> m_material0;
    std::shared_ptr<MaterialState> m_material1;

    ConstantBufferData m_cbvData;
    std::vector<PhongMaterial> m_materialConstants;

    Luz::CameraController m_cameraController;

    std::vector<i32> m_materialIndices;

    MeshApplication();
    ~MeshApplication();

    bool Initialize() override;
    int Shutdown() override;

    void Update(double delta) override;
    void FixedUpdate(double delta) override;
};

#endif
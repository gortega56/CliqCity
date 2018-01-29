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

class Window;

struct Vertex
{
    gmath::float3 Tangent;
    gmath::float3 Position;
    gmath::float3 Normal;
    gmath::float2 UV;
    Vertex(float px, float py, float pz,
        float nx, float ny, float nz,
        float u, float v) :
        Tangent(0.0f, 0.0f, 0.0f),
        Position(px, py, pz),
        Normal(nx, ny, nz),
        UV(u, v) {}
    Vertex() {}
};

struct ConstantBufferData
{
    gmath::float4x4 model;
    gmath::float4x4 view;
    gmath::float4x4 proj;
};

class MeshApplication :
    public IApplication
{
public:
    using IApplication::IApplication;

    std::shared_ptr<Window> m_window;

    Dx12::Shader m_vs;
    Dx12::Shader m_ps;
    Dx12::GraphicsPipeline m_pipeline;

    std::shared_ptr<Dx12::RootSignature> m_rs;

    std::shared_ptr<Renderable> m_renderable0;

    std::shared_ptr<MaterialState> m_material0;
    std::shared_ptr<MaterialState> m_material1;

    ConstantBufferData m_cbvData0;
    ConstantBufferData m_cbvData1;

    MeshApplication();
    ~MeshApplication();

    bool Initialize() override;
    int Shutdown() override;

    void Update(double delta) override;
    void FixedUpdate(double delta) override;
};

#endif
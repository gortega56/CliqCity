#pragma once
#ifndef MESHAPPLICATION_H
#define MESHAPPLICATION_H
#include <mutex>
#include <wrl.h>
#include "GeneralMacros.h"

#ifndef IAPPLICATION_H
#include "IApplication.h"
#endif

//#ifndef LUZ_H
//#include "Luz.h"
//#endif

#ifndef TYPEDEFS_H
#include "TypeDefs.h"
#endif

#ifndef CGM_H
#include "cgm.h"
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

class Window;

struct Vertex
{
    vec3f position;
    vec3f tangent;
    vec3f normal;
    vec2f uv;
    Vertex(float px, float py, float pz,
        float nx, float ny, float nz, 
        float u, float v) :
        tangent(0.0f, 0.0f, 0.0f),
        position(px, py, pz), 
        normal(nx, ny, nz),
        uv(u, v) {}
};

struct ConstantBufferData
{
    mat4f model;
    mat4f view;
    mat4f proj;
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
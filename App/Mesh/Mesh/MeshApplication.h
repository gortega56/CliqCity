#pragma once
#ifndef IAPPLICATION_H
#include "IApplication.h"
#endif

#include <memory>

#ifndef LUZ_H
#include "Luz.h"
#endif

#ifndef CGM_H
#include "cgm.h"
#endif

struct Vertex
{
    vec3f position;
    vec2f uv;
    Vertex(float x, float y, float z, float u, float v) :
        position(x, y, z), uv(u, v) {}
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

    Shader m_vs;
    Shader m_ps;
    GraphicsPipeline m_pipeline;
    RootSignature m_rs;

    std::shared_ptr<Renderable> m_renderable;
    std::shared_ptr<Material> m_material;
    std::shared_ptr<Dx12::DescriptorHeap> m_srvHeap;
    std::shared_ptr<Dx12::PixelBuffer> m_srvBuffer;

    ConstantBufferData m_cbvData;
    Dx12::UploadBuffer m_gpuBuffer;

    MeshApplication();
    ~MeshApplication();

    bool Initialize() override;
    int Shutdown() override;

    void Update(double delta) override;
    void FixedUpdate(double delta) override;
};


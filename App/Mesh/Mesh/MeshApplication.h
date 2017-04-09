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
    vec4f color;
    vec3f position;
    Vertex(float r, float g, float b, float a, float x, float y, float z) :
        color(r, g, b, a), position(x, y, z) {}
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

    ConstantBufferData m_cbvData;
    dx12::UploadBuffer m_gpuBuffer;

    MeshApplication();
    ~MeshApplication();

    bool Initialize() override;
    int Shutdown() override;

    void Update(double delta) override;
    void FixedUpdate(double delta) override;
};


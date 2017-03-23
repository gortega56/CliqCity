#pragma once
#include "IApplication.h"

#include <memory>

#ifndef LUZ_H
#include "Luz.h"
#endif

#ifndef CGM_H
#include "..\..\Library\GraphicsMath\cgm.h"
#endif

struct Vertex
{
    vec3f position;
};

struct ViewProj
{
    mat4f view;
    mat4f proj;
};

class MeshApplication :
    public IApplication
{
public:
    using IApplication::IApplication;

    GraphicsPipeline m_pipeline;
    RootSignature m_rs;

    std::shared_ptr<Renderable> m_renderable;

    MeshApplication();
    ~MeshApplication();

    int Initialize() override;
    int Shutdown() override;

    void Update(double delta) override;
    void FixedUpdate(double delta) override;
};


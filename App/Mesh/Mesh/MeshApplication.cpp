#include "MeshApplication.h"

MeshApplication::MeshApplication() : m_rs(1)
{
}


MeshApplication::~MeshApplication()
{
}

int MeshApplication::Initialize()
{
    Renderer* pRenderer = m_engine->Graphics().get();

    Vertex verts[] = 
    {
        { { +0.5, +0.5, +0.5 } },
        { { -0.5, +0.5, +0.5 } },
        { { +0.5, -0.5, +0.5 } },
        { { -0.5, -0.5, +0.5 } }
    };

    u8 indices[] =
    {
        0, 2, 1,
        1, 2, 3
    };

    Mesh<Vertex, u8> mesh(std::vector<Vertex>(std::begin(verts), std::begin(verts) + 4), std::vector<u8>(std::begin(indices), std::begin(indices) + 6));
    
    m_renderable = std::make_shared<Renderable>();

    if (!m_renderable->LoadMesh(pRenderer, &mesh))
    {
        return 1;
    }

    Shader vs;
    Shader ps;

    vs.InitializeVS(L"");
    ps.InitializePS(L"");

    InputLayout inputLayout;
    inputLayout.AppendPosition3F();

    m_rs.AppendRootCBV(0, 1);
    if (!m_rs.Finalize(pRenderer))
    {
        return 1;
    }

    m_pipeline.SetInputLayout(&inputLayout);
    m_pipeline.SetRootSignature(&m_rs);
    m_pipeline.SetVS(&vs);
    m_pipeline.SetPS(&ps);
    m_pipeline.SetTriangleTopology();
    m_pipeline.SetSampleCount(1);
    m_pipeline.SetSampleQuality(0);
    m_pipeline.SetSampleMask(0xffffffff);
    m_pipeline.SetRasterizerState(&RasterizerState());
    m_pipeline.SetDepthStencilState(&DepthStencilState());
    m_pipeline.SetBlendState(&BlendState());
    m_pipeline.SetRenderTargets(pRenderer);
    
    if (!m_pipeline.Finalize(pRenderer))
    {
        return 1;
    }


    return 0;
}

int MeshApplication::Shutdown()
{
    return 0;
}

void MeshApplication::Update(double dt)
{
    Renderer* pRenderer = m_engine->Graphics().get();

    pRenderer->UpdatePipeline(&m_pipeline);
    pRenderer->SetRootSignature(&m_rs);

    pRenderer->SetRenderContext();
    pRenderer->ClearRenderContext();

    pRenderer->SetViewport();

    pRenderer->Prepare(m_renderable.get());

    // Set CBV

    pRenderer->DrawIndexedInstanced(m_renderable.get());

    pRenderer->Present();
}

void MeshApplication::FixedUpdate(double dt)
{

}
#include "MeshApplication.h"

MeshApplication::MeshApplication() : m_rs(1)
{
}


MeshApplication::~MeshApplication()
{
}

bool MeshApplication::Initialize()
{
    Renderer* pRenderer = m_engine->Graphics().get();

    Vertex verts[] =
    {
        Vertex(1.0f, 0.0f, 0.0f, 1.0f, -0.5f, +0.5f, -0.5f),    // 0 Back Top Left
        Vertex(1.0f, 0.0f, 1.0f, 1.0f, +0.5f, -0.5f, -0.5f),    // 1 Back Bottom Right
        Vertex(0.0f, 0.0f, 1.0f, 1.0f, -0.5f, -0.5f, -0.5f),    // 2 Back Bottom Left
        Vertex(0.0f, 1.0f, 0.0f, 1.0f, +0.5f, +0.5f, -0.5f),    // 3 Back Top Right
        Vertex(1.0f, 0.0f, 0.0f, 1.0f, -0.5f, +0.5f, +0.5f),    // 4 Front Top Left
        Vertex(1.0f, 0.0f, 1.0f, 1.0f, +0.5f, -0.5f, +0.5f),    // 5 Front Bottom Right
        Vertex(0.0f, 0.0f, 1.0f, 1.0f, -0.5f, -0.5f, +0.5f),    // 6 Front Bottom Left
        Vertex(0.0f, 1.0f, 0.0f, 1.0f, +0.5f, +0.5f, +0.5f)     // 7 Front Top Right
    };

    u32 indices[] =
    {
        0, 1, 2,    // Back face
        0, 3, 1,
        3, 7, 1,    // Right face
        7, 5, 1,
        4, 0, 2,    // Left face
        4, 2, 6,
        4, 6, 5,    // Front face
        4, 5, 7,
        4, 0, 3,    // Top face
        4, 7, 3,
        2, 1, 6,    // Bottom face
        1, 5, 6
    };

    Mesh<Vertex, u32> mesh(std::vector<Vertex>(std::begin(verts), std::begin(verts) + 8), std::vector<u32>(std::begin(indices), std::begin(indices) + 36));
    
    m_renderable = std::make_shared<Renderable>();

    if (!m_renderable->LoadMesh(pRenderer, &mesh))
    {
        return false;
    }

    if (!m_vs.InitializeVS(L"VertexShader.hlsl"))
    {
        return false;
    }

    if (!m_ps.InitializePS(L"PixelShader.hlsl"))
    {
        return false;
    }

    InputLayout inputLayout;
    inputLayout.AppendColor4F()
        .AppendPosition3F()
        .Finalize();

    m_rs.AllowInputLayout()
        .DenyHS()
        .DenyDS()
        .DenyGS()
        .DenyPS()
        .AppendRootCBV(0, 0);

    if (!m_rs.Finalize(pRenderer->GetDevice()))
    {
        return false;
    }

    m_pipeline.SetInputLayout(&inputLayout);
    m_pipeline.SetRootSignature(&m_rs);
    m_pipeline.SetVS(&m_vs);
    m_pipeline.SetPS(&m_ps);
    m_pipeline.SetTriangleTopology();
    m_pipeline.SetSampleCount(1);
    m_pipeline.SetSampleQuality(0);
    m_pipeline.SetSampleMask(0xffffffff);
    m_pipeline.SetRasterizerState(&RasterizerState());
    m_pipeline.SetDepthStencilState(&DepthStencilState());
    m_pipeline.SetBlendState(&BlendState());
    m_pipeline.SetRenderTargets(pRenderer->GetRenderContext());
    
    if (!m_pipeline.Finalize(pRenderer->GetDevice()))
    {
        return false;
    }

    m_cbvData.model = mat4f(1.0f).transpose();
    m_cbvData.view = mat4f::lookAtLH(vec3f(0.0f), vec3f(0.0f, 0.0f, -15.0f), vec3f(0.0f, 1.0f, 0.0f)).transpose();
    m_cbvData.proj = mat4f::perspectiveLH(3.14f * 0.5f, pRenderer->AspectRatio(), 0.1f, 100.0f).transpose();

    if (!m_gpuBuffer.Initialize(pRenderer->GetGraphicsContext(), sizeof(ConstantBufferData), sizeof(ConstantBufferData), 1, &m_cbvData))
    {
        return false;
    }
    
    return true;
}

int MeshApplication::Shutdown()
{
    return 0;
}

void MeshApplication::Update(double dt)
{
    Renderer* pRenderer = m_engine->Graphics().get();

    // Rendering
    m_gpuBuffer.Map(&m_cbvData);
    m_gpuBuffer.Unmap();

    pRenderer->GetGraphicsContext()->WaitForPreviousFrame();
    pRenderer->GetGraphicsContext()->Reset(&m_pipeline);

    pRenderer->SetRenderContext();
    pRenderer->ClearRenderContext();

    pRenderer->GetGraphicsContext()->SetRootSignature(&m_rs);
    pRenderer->SetViewport();

    pRenderer->Prepare(m_renderable.get());

    pRenderer->SetGraphicsRootConstantBuffer(&m_gpuBuffer);

    pRenderer->DrawIndexedInstanced(m_renderable.get());

    pRenderer->Present();
}

void MeshApplication::FixedUpdate(double dt)
{
    double time = m_engine->Total();
    float st = sin(time);
    float ct = cos(time);

    mat4f trn = mat4f::translate(vec3f(ct, st, ct) * 6.0f);
    mat4f rot = quatf::rollPitchYaw(st, ct, st).toMatrix4();

    m_cbvData.model = (rot * trn * rot).transpose();
}
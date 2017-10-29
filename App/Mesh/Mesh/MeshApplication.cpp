#include "MeshApplication.h"
#include "Engine.h"
#include "Resource\ResourceManager.h"
#include "Mesh.h"
#include "DirectX12\GpuResource.h"
#include <functional>
#include <memory>
#include "DirectX12\CommandContext.h"
#include "Resource\Texture.h"
#include "Window.h"
#include "Graphics.h"
#include "Console.h"

#define DIFF_PATH0 L".\\Assets\\BrickDiff.dds"
#define DIFF_PATH1 L".\\Assets\\RockPileDiff.dds"

#define NORM_PATH0 L".\\Assets\\BrickNorm.dds"
#define NORM_PATH1 L".\\Assets\\RockPileNorm.dds"

MeshApplication::MeshApplication()
{
}


MeshApplication::~MeshApplication()
{
}

bool MeshApplication::Initialize()
{
    m_window = Window::Create("Mesh Application", 1600, 900, false);

    if (!Graphics::Initialize(m_window.get(), 3))
    {
        return false;
    }

    Vertex verts[] =
    {
        // front face
        { -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f },
        { 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f },
        { -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f },
        { 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f },

        // right side face
        { 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f },
        { 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f },
        { 0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
        { 0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },

        // left side face
        { -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
        { -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
        { -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f },
        { -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f },

        // back face
        { 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
        { -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f },
        { 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
        { -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f },

        // top face
        { -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
        { 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f },
        { 0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f },
        { -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },

        // bottom face
        { 0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f },
        { -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f },
        { 0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f },
        { -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f }
    };

    u32 indices[] =
    {
        // ffront face
        0, 1, 2, // first triangle
        0, 3, 1, // second triangle

        // left face
        4, 5, 6, // first triangle
        4, 7, 5, // second triangle

        // right face
        8, 9, 10, // first triangle
        8, 11, 9, // second triangle

        // back face
        12, 13, 14, // first triangle
        12, 15, 13, // second triangle

        // top face
        16, 17, 18, // first triangle
        16, 19, 17, // second triangle

        // bottom face
        20, 21, 22, // first triangle
        20, 23, 21, // second triangle
    };

    Mesh<Vertex, u32> mesh(std::vector<Vertex>(std::begin(verts), 
        std::begin(verts) + 24), 
        std::vector<u32>(std::begin(indices), 
        std::begin(indices) + 36));
    
    m_renderable0 = std::make_shared<Renderable>();
    if (!m_renderable0->LoadMesh(&mesh))
    {
        return false;
    }

    float aspectRatio = m_window->AspectRatio();

    m_cbvData0.model = mat4f(1.0f).transpose();
    m_cbvData0.view = mat4f::lookAtLH(vec3f(0.0f), vec3f(0.0f, 0.0f, -15.0f), vec3f(0.0f, 1.0f, 0.0f)).transpose();
    m_cbvData0.proj = mat4f::perspectiveLH(3.14f * 0.5f, aspectRatio, 0.1f, 100.0f).transpose();

    m_cbvData1.model = mat4f(1.0f).transpose();
    m_cbvData1.view = mat4f::lookAtLH(vec3f(0.0f), vec3f(0.0f, 0.0f, -15.0f), vec3f(0.0f, 1.0f, 0.0f)).transpose();
    m_cbvData1.proj = mat4f::perspectiveLH(3.14f * 0.5f, aspectRatio, 0.1f, 100.0f).transpose();

    if (!m_vs.InitializeVS(L"VertexShader.hlsl"))
    {
        return false;
    }

    if (!m_ps.InitializePS(L"PixelShader.hlsl"))
    {
        return false;
    }

    InputLayout inputLayout;
    inputLayout.AppendPosition3F()
        .AppendNormal3F()
        .AppendUV2()
        .Finalize();

    auto range = Dx12::DescriptorTable(1).AppendRangeSRV(1, 0);

    m_rs = std::make_shared<Dx12::RootSignature>(2);
    m_rs->AllowInputLayout()
        .DenyHS()
        .DenyDS()
        .DenyGS()
        .AppendRootCBV(0).AppendRootDescriptorTable(range, D3D12_SHADER_VISIBILITY_PIXEL).AppendAnisotropicWrapSampler(0);

    if (!m_rs->Finalize())
    {
        return false;
    }

    m_pipeline.SetInputLayout(&inputLayout);
    m_pipeline.SetRootSignature(m_rs.get());
    m_pipeline.SetVS(&m_vs);
    m_pipeline.SetPS(&m_ps);
    m_pipeline.SetTriangleTopology();
    m_pipeline.SetSampleCount(1);
    m_pipeline.SetSampleQuality(0);
    m_pipeline.SetSampleMask(0xffffffff);
    m_pipeline.SetRasterizerState(&RasterizerState());
    m_pipeline.SetDepthStencilState(&DepthStencilState());
    m_pipeline.SetBlendState(&BlendState());
    m_pipeline.SetRenderTargets();
    
    if (!m_pipeline.Finalize())
    {
        return false;
    }

    MaterialBuilder matBuilder0(m_rs);
    matBuilder0.SetRootConstantBufferView(0, 0, sizeof(ConstantBufferData), sizeof(ConstantBufferData), 1, &m_cbvData0);
    matBuilder0.SetDescriptorTableEntry(1, 0, 0, DIFF_PATH0);
    matBuilder0.SetDescriptorTableEntry(1, 0, 1, NORM_PATH0);

    m_material0 = matBuilder0.ToImmutable();
    

    MaterialBuilder mb1(m_rs);
    mb1.SetRootConstantBufferView(0, 0, sizeof(ConstantBufferData), sizeof(ConstantBufferData), 1, &m_cbvData1);
    mb1.SetDescriptorTableEntry(1, 0, 0, DIFF_PATH1);
    mb1.SetDescriptorTableEntry(1, 0, 1, NORM_PATH1);
    m_material1 = mb1.ToImmutable();

    return true;
}

int MeshApplication::Shutdown()
{
    Graphics::Shutdown();

    return 0;
}

void MeshApplication::Update(double dt)
{
    static int frame = 0;

    auto pCtx = Dx12::GraphicsCommandContext::Create();
    pCtx->Reset(&m_pipeline);

    pCtx->SetRenderContext();
    pCtx->ClearRenderContext();
    pCtx->SetViewport();

    pCtx->SetRootSignature(m_rs.get());

    m_material0->UpdateConstantBufferView(0, &m_cbvData0);
    m_material0->Prepare(pCtx.get());

    m_renderable0->Prepare(pCtx.get());
    m_renderable0->DrawIndexedInstanced(pCtx.get());

    m_material1->UpdateConstantBufferView(0, &m_cbvData1);
    m_material1->Prepare(pCtx.get());

    m_renderable0->DrawIndexedInstanced(pCtx.get());

    pCtx->Present();
    frame++;
}

void MeshApplication::FixedUpdate(double dt)
{
    double time = m_engine->Total();
    float st = (float)sin(time);
    float ct = (float)cos(time);

    mat4f trn = mat4f::translate(vec3f(ct, st, ct) * 6.0f);
    mat4f rot = quatf::rollPitchYaw(st, ct, st).toMatrix4();

    m_cbvData0.model = (rot * trn * rot).transpose();
    m_cbvData1.model = (trn * rot * trn).transpose();
}
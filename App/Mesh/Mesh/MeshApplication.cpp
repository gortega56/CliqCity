#include "MeshApplication.h"
#include "Engine.h"
#include "Resource\ResourceManager.h"
#include "Mesh.h"
#include "DirectX12\GpuResource.h"
#include <functional>
#include <memory>
#include "DirectX12\CommandContext.h"
#include "Resource\Texture.h"

#define TEXURE_PATH0 L".\\Assets\\tarmac_0.dds"
#define TEXURE_PATH1 L".\\Assets\\oldsandbags.png"


MeshApplication::MeshApplication()
{
}


MeshApplication::~MeshApplication()
{
}

bool MeshApplication::Initialize()
{
    Vertex verts[] =
    {
        // front face
        { -0.5f,  0.5f, -0.5f, 0.0f, 0.0f },
        { 0.5f, -0.5f, -0.5f, 1.0f, 1.0f },
        { -0.5f, -0.5f, -0.5f, 0.0f, 1.0f },
        { 0.5f,  0.5f, -0.5f, 1.0f, 0.0f },

        // right side face
        { 0.5f, -0.5f, -0.5f, 0.0f, 1.0f },
        { 0.5f,  0.5f,  0.5f, 1.0f, 0.0f },
        { 0.5f, -0.5f,  0.5f, 1.0f, 1.0f },
        { 0.5f,  0.5f, -0.5f, 0.0f, 0.0f },

        // left side face
        { -0.5f,  0.5f,  0.5f, 0.0f, 0.0f },
        { -0.5f, -0.5f, -0.5f, 1.0f, 1.0f },
        { -0.5f, -0.5f,  0.5f, 0.0f, 1.0f },
        { -0.5f,  0.5f, -0.5f, 1.0f, 0.0f },

        // back face
        { 0.5f,  0.5f,  0.5f, 0.0f, 0.0f },
        { -0.5f, -0.5f,  0.5f, 1.0f, 1.0f },
        { 0.5f, -0.5f,  0.5f, 0.0f, 1.0f },
        { -0.5f,  0.5f,  0.5f, 1.0f, 0.0f },

        // top face
        { -0.5f,  0.5f, -0.5f, 0.0f, 1.0f },
        { 0.5f,  0.5f,  0.5f, 1.0f, 0.0f },
        { 0.5f,  0.5f, -0.5f, 1.0f, 1.0f },
        { -0.5f,  0.5f,  0.5f, 0.0f, 0.0f },

        // bottom face
        { 0.5f, -0.5f,  0.5f, 0.0f, 0.0f },
        { -0.5f, -0.5f, -0.5f, 1.0f, 1.0f },
        { 0.5f, -0.5f, -0.5f, 0.0f, 1.0f },
        { -0.5f, -0.5f,  0.5f, 1.0f, 0.0f }
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

    Mesh<Vertex, u32> mesh(std::vector<Vertex>(std::begin(verts), std::begin(verts) + 24), std::vector<u32>(std::begin(indices), std::begin(indices) + 36));
    
    m_renderable0 = std::make_shared<Renderable>();

    if (!m_renderable0->LoadMesh(&mesh))
    {
        return false;
    }

    float aspectRatio = m_engine->Graphics()->AspectRatio();

    m_cbvData0.model = mat4f(1.0f).transpose();
    m_cbvData0.view = mat4f::lookAtLH(vec3f(0.0f), vec3f(0.0f, 0.0f, -15.0f), vec3f(0.0f, 1.0f, 0.0f)).transpose();
    m_cbvData0.proj = mat4f::perspectiveLH(3.14f * 0.5f, aspectRatio, 0.1f, 100.0f).transpose();

    m_cbvData1.model = mat4f(1.0f).transpose();
    m_cbvData1.view = mat4f::lookAtLH(vec3f(0.0f), vec3f(0.0f, 0.0f, -15.0f), vec3f(0.0f, 1.0f, 0.0f)).transpose();
    m_cbvData1.proj = mat4f::perspectiveLH(3.14f * 0.5f, aspectRatio, 0.1f, 100.0f).transpose();

    m_gpuBuffer0 = std::make_shared<Dx12::UploadBuffer>();
    if (!m_gpuBuffer0->InitializeStructure(&m_cbvData0))
    {
        return false;
    }

    m_gpuBuffer1 = std::make_shared<Dx12::UploadBuffer>();
    if (!m_gpuBuffer1->InitializeStructure(&m_cbvData1))
    {
        return false;
    }

    ResourceManager rm;
    auto texture0 = rm.GetResourceFuture<Texture2D>(TEXURE_PATH0).get();
    
    m_srvBuffer0 = std::make_shared<Dx12::PixelBuffer>();
    m_srvBuffer0->SetResourceState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    if (!m_srvBuffer0->InitializeTexture2D(texture0))
    {
        return false;
    }
    
    m_srvBuffer0->CreateShaderResourceView();

    auto texture1 = rm.GetResourceFuture<Texture2D>(TEXURE_PATH1).get();

    m_srvBuffer1 = std::make_shared<Dx12::PixelBuffer>();
    m_srvBuffer1->SetResourceState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    if (!m_srvBuffer1->InitializeTexture2D(texture1))
    {
        return false;
    }

    m_srvBuffer1->CreateShaderResourceView();

    /*    ResourceManager rm;
    rm.LoadResource<Texture2D>(L"somefuckingtexture", [weakMaterial = std::weak_ptr<Material>(m_material)](std::shared_ptr<const Texture2D> pTexture)
    {
        if (!pTexture)
        {
            return;
        }

        if (auto shared = weakMaterial.lock())
        {
            shared->SetTexture2D((ParamID)1, pTexture);
        }
    });  */  

    if (!m_vs.InitializeVS(L"VertexShader.hlsl"))
    {
        return false;
    }

    if (!m_ps.InitializePS(L"PixelShader.hlsl"))
    {
        return false;
    }

    InputLayout inputLayout;
    inputLayout.AppendPosition3F().AppendUV2().Finalize();

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
    m_pipeline.SetRenderTargets(m_engine->Graphics()->GetRenderContext());
    
    if (!m_pipeline.Finalize())
    {
        return false;
    }

    m_material0 = std::make_shared<MaterialState>(m_rs);
    m_material0->SetRootConstantBufferView(m_gpuBuffer0, 0);
    m_material0->SetShaderResourceViewTableEntry(m_srvBuffer0, 1, 0, 0);
    
    m_material1 = std::make_shared<MaterialState>(m_rs);
    m_material1->SetRootConstantBufferView(m_gpuBuffer1, 0);
    m_material1->SetShaderResourceViewTableEntry(m_srvBuffer1, 1, 0, 0);

    return true;
}

int MeshApplication::Shutdown()
{
    return 0;
}

void MeshApplication::Update(double dt)
{
    static int frame = 0;

    Renderer* pRenderer = m_engine->Graphics().get();

    auto pCtx = Dx12::GraphicsCommandContext::Create();
    pCtx->Reset(&m_pipeline);

    pRenderer->SetRenderContext(pCtx);
    pRenderer->ClearRenderContext(pCtx);
    pRenderer->SetViewport(pCtx);

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
#include "MeshApplication.h"
#include <functional>

#define TEXURE_PATH L".\\Assets\\tarmac_0.dds"

MeshApplication::MeshApplication() : m_rs(1)
{
}


MeshApplication::~MeshApplication()
{
}

bool MeshApplication::Initialize()
{
    auto pRenderer = m_engine->Graphics();

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
    
    m_renderable = std::make_shared<Renderable>();

    if (!m_renderable->LoadMesh(pRenderer, &mesh))
    {
        return false;
    }

    m_cbvData.model = mat4f(1.0f).transpose();
    m_cbvData.view = mat4f::lookAtLH(vec3f(0.0f), vec3f(0.0f, 0.0f, -15.0f), vec3f(0.0f, 1.0f, 0.0f)).transpose();
    m_cbvData.proj = mat4f::perspectiveLH(3.14f * 0.5f, pRenderer->AspectRatio(), 0.1f, 100.0f).transpose();

    m_gpuBuffer = std::make_shared<Dx12::UploadBuffer>();
    if (!m_gpuBuffer->InitializeStructure(pRenderer, &m_cbvData))
    {
        return false;
    }

    ResourceManager rm;
    auto texture = rm.GetResourceFuture<Texture2D>(TEXURE_PATH).get();
    
    m_srvBuffer = std::make_shared<Dx12::PixelBuffer>();
    m_srvBuffer->SetResourceState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    if (!m_srvBuffer->InitializeTexture2D(pRenderer, texture))
    {
        return false;
    }
    
    m_srvHeap = std::make_shared<Dx12::DescriptorHeap>(1);
    if (!m_srvHeap->InitializeMixed(pRenderer->GetDevice(), L"SRV Heap"))
    {
        return false;
    }


    m_srvHeap->CreateShaderResourceViews(pRenderer->GetDevice(), m_srvBuffer.get());

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

    m_rs.AllowInputLayout()
        .DenyHS()
        .DenyDS()
        .DenyGS()
        .AppendRootCBV(0).AppendRootDescriptorTable(range, D3D12_SHADER_VISIBILITY_PIXEL).AppendAnisotropicWrapSampler(0);

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

    std::shared_ptr<const RootSignature> sharedRootSignature;
    sharedRootSignature.reset(&m_rs);

    m_material = std::make_shared<MaterialState>(sharedRootSignature);
    m_material->SetRootConstantBufferView(m_gpuBuffer, 0);
    m_material->SetRootDescriptorTable(m_srvHeap, std::vector<std::shared_ptr<const Dx12::GpuResource>>({ m_srvBuffer }), 1);
    
    return true;
}

int MeshApplication::Shutdown()
{
    return 0;
}

void MeshApplication::Update(double dt)
{
    // Rendering
    m_gpuBuffer->Map(&m_cbvData);
    m_gpuBuffer->Unmap();

    Renderer* pRenderer = m_engine->Graphics().get();

    auto pCtx = pRenderer->GetContext();
    pCtx->Reset(&m_pipeline);

    pRenderer->SetRenderContext(pCtx);
    pRenderer->ClearRenderContext(pCtx);

    pCtx->SetRootSignature(&m_rs);
    pCtx->SetDescriptorHeap(m_srvHeap);

    pRenderer->SetViewport(pCtx);

    m_material->Prepare(pCtx.get());
    //pCtx->SetGraphicsRootConstantBufferView(&m_gpuBuffer);
    //pCtx->SetGraphicsRootDescriptorTable(m_srvHeap.get(), 1);

    m_renderable->Prepare(pCtx.get());
    m_renderable->DrawIndexedInstanced(pCtx.get());

    pRenderer->Present(pCtx);
}

void MeshApplication::FixedUpdate(double dt)
{
    double time = m_engine->Total();
    float st = (float)sin(time);
    float ct = (float)cos(time);

    mat4f trn = mat4f::translate(vec3f(ct, st, ct) * 6.0f);
    mat4f rot = quatf::rollPitchYaw(st, ct, st).toMatrix4();

    m_cbvData.model = (rot * trn * rot).transpose();
}
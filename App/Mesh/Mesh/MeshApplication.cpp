#include "MeshApplication.h"
#include "Engine.h"
#include "Resource\ResourceManager.h"
#include "Mesh.h"
#include "DirectX12\GpuResource.h"
#include <functional>
#include <memory>
#include "DirectX12\CommandContext.h"
#include "Resource\Texture.h"
#include "Resource\Fbx.h"
#include "Resource\ObjResource.h"
#include "Resource\MtlResource.h"
#include "Platform\Window.h"
#include "Graphics.h"
#include "Console.h"
#include "Platform\Input.h"

#define DIFF_PATH0 L".\\Assets\\BrickDiff.dds"
#define DIFF_PATH1 L".\\Assets\\RockPileDiff.dds"

#define NORM_PATH0 L".\\Assets\\BrickNorm.dds"
#define NORM_PATH1 L".\\Assets\\RockPileNorm.dds"

#define FBX_PATH0 L".\\Assets\\/*Prof_Animated*/.fbx"
#define FBX_PATH1 L".\\Assets\\cube.FBX"

#define SPONZA_OBJ_PATH L".\\Assets\\sponza_obj\\sponza.obj"
#define SPONZA_MTL_PATH L".\\Assets\\sponza_obj\\sponza.mtl"
#define SPONZA_TEX_PATH L".\\Assets\\sponza_textures\\textures\\"

using namespace gmath;
static float g_scale = 5.0f;
Console g_console;

MeshApplication::MeshApplication()
{
}


MeshApplication::~MeshApplication()
{
}

bool MeshApplication::Initialize()
{
    Console::Initialize(&g_console);

    m_window = Window::Create("Mesh Application", 1600, 900, false);

    if (!Graphics::Initialize(m_window.get(), 3))
    {
        return false;
    }

    m_renderable0 = std::make_shared<Renderable>();
    std::weak_ptr<Renderable> weakRenderable = m_renderable0;
    ResourceManager rm;



    rm.LoadResource<Resource::Mtl>(SPONZA_MTL_PATH, [](std::shared_ptr<const Resource::Mtl> pObj)
    {
       
    });

    std::weak_ptr<MeshApplication> weakApp = shared_from_this();
    std::vector<Mesh<Vertex, u32>> meshes;
    rm.LoadResource<Resource::Obj>(SPONZA_OBJ_PATH, [weakApp, &meshes](std::shared_ptr<const Resource::Obj> pObj)
    {
        if (auto app = weakApp.lock())
        {
            if (pObj)
            {
                pObj->Export(meshes);
            }
        }
    });

    rm.LoadResource<Resource::Fbx>(FBX_PATH1, [weakRenderable](std::shared_ptr<const Resource::Fbx> pFbx)
    {
        if (!pFbx) return;

        if (auto sharedRenderable = weakRenderable.lock())
        {
            Mesh<Vertex, u32> mesh;
            pFbx->WriteVertices<Vertex>(mesh.Vertices(), [](Vertex& vertex, const Resource::Fbx::Vertex& fbx)
            {
                vertex.Position = *reinterpret_cast<const float3*>(&fbx.Position[0]);
                vertex.Normal = *reinterpret_cast<const float3*>(&fbx.Normal[0]);
                vertex.UV = *reinterpret_cast<const float2*>(&fbx.UV[0]);
            });

            mesh.SetIndices(pFbx->GetIndices());
            mesh.GenerateTangents();

            sharedRenderable->LoadMesh(&mesh);
            sharedRenderable->m_isRenderable.store(true);
        }
    });

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

    for (int i = 4; i < 25; i += 4)
    {
        Vertex* v1 = &verts[i - 4];
        Vertex* v2 = &verts[i - 3];
        Vertex* v3 = &verts[i - 2];
        Vertex* v4 = &verts[i - 1];

        float x1 = v2->Position.x - v1->Position.x;
        float x2 = v3->Position.x - v1->Position.x;
        float y1 = v2->Position.y - v1->Position.y;
        float y2 = v3->Position.y - v1->Position.y;
        float z1 = v2->Position.z - v1->Position.z;
        float z2 = v3->Position.z - v1->Position.z;

        float s1 = v2->UV.x - v1->UV.x;
        float s2 = v3->UV.x - v1->UV.x;
        float t1 = v2->UV.y - v1->UV.y;
        float t2 = v3->UV.y - v1->UV.y;

        float r = 1.0f / ((s1 * t2) - (s2 * t1));

        float3 tangent = { (((t2 * x1) - (t1 * x2)) * r), (((t2 * y1) - (t1 * y2)) * r), (((t2 * z1) - (t1 * z2)) * r) };
        v1->Tangent = tangent;
        v2->Tangent = tangent;
        v3->Tangent = tangent;
        v4->Tangent = tangent;
    }

    Mesh<Vertex, u32> mesh(std::vector<Vertex>(std::begin(verts), 
        std::begin(verts) + 24), 
        std::vector<u32>(std::begin(indices), 
        std::begin(indices) + 36));
    
    //m_renderable0 = std::make_shared<Renderable>();
    //if (!m_renderable0->LoadMesh(&mesh))
    //{
    //    return false;
    //}
    //m_renderable0->m_isRenderable.store(true);

    float aspectRatio = m_window->AspectRatio();


    m_cbvData0.model = float4x4::scale(float3(g_scale));
    m_cbvData0.view = float4x4::look_at_lh(float3(0.0f), float3(0.0f, 0.0f, -15.0f), float3(0.0f, 1.0f, 0.0f)).transpose();
    m_cbvData0.proj = float4x4::perspective_lh(3.14f * 0.5f, aspectRatio, 0.1f, 100.0f).transpose();
/*
    m_cbvData1.model = float4x4(1.0f).transpose();
    m_cbvData1.view = float4x4::look_at_lh(float3(0.0f), float3(0.0f, 0.0f, -15.0f), float3(0.0f, 1.0f, 0.0f)).transpose();
    m_cbvData1.proj = float4x4::perspective_lh(3.14f * 0.5f, aspectRatio, 0.1f, 100.0f).transpose();
*/
    if (!m_vs.InitializeVS(L"VertexShader.hlsl"))
    {
        return false;
    }

    if (!m_ps.InitializePS(L"PixelShader.hlsl"))
    {
        return false;
    }

    InputLayout inputLayout;
    inputLayout
        .AppendFloat4("TANGENT")
        .AppendPosition3F()
        .AppendNormal3F()
        .AppendUV2()
        .Finalize();

    auto range = Dx12::DescriptorTable(1).AppendRangeSRV(2, 0);

    m_rs = std::make_shared<Dx12::RootSignature>(2);
    m_rs->AllowInputLayout()
        .DenyHS()
        .DenyDS()
        .DenyGS()
        .AppendRootCBV(0)
        .AppendRootDescriptorTable(range, D3D12_SHADER_VISIBILITY_PIXEL)
        .AppendAnisotropicWrapSampler(0);

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
    m_pipeline.SetDepthStencilState(&DepthStencilState());
    m_pipeline.SetRasterizerState(&RasterizerState()); 
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

    if (m_renderable0->m_isRenderable.load())
    {
        m_renderable0->Prepare(pCtx.get());
        m_renderable0->DrawIndexedInstanced(pCtx.get());
    }
    
   /* m_material1->UpdateConstantBufferView(0, &m_cbvData1);
    m_material1->Prepare(pCtx.get());

    if (m_renderable0->m_isRenderable.load())
    {
        m_renderable0->DrawIndexedInstanced(pCtx.get());
    }*/

    pCtx->Present();
    frame++;
}

void MeshApplication::FixedUpdate(double dt)
{
    auto pInput = m_engine->OS()->GetInput();
    if (pInput)
    {
        float twoPi = 6.28318530718f;
        float incr = twoPi / 48.0f;

        static euler rotation = euler(0.0f, 0.0f, 0.0f);
        if (pInput->GetKey(Luz::KeyCode::KEYCODE_LEFT))
        {
            rotation.y += incr;
        }

        if (pInput->GetKey(Luz::KeyCode::KEYCODE_RIGHT))
        {
            rotation.y -= incr;
        }

        if (pInput->GetKey(Luz::KeyCode::KEYCODE_DOWN))
        {
            rotation.x -= incr;
        }

        if (pInput->GetKey(Luz::KeyCode::KEYCODE_UP))
        {
            rotation.x += incr;
        }

        float4x4 r;
        quaternion(rotation).to_matrix(r);
        m_cbvData0.model = (r * float4x4::scale(g_scale)).transpose();
    }

    //double time = m_engine->Total() * 0.5f;
    //float st = (float)sin(time);
    //float ct = (float)cos(time);

    //mat4f trn = mat4f::translate(vec3f(ct, st, ct) * 6.0f);
    //mat4f rot = quatf::rollPitchYaw(st, ct, st).toMatrix4();
    //mat4f scl = mat4f::scale(vec3f(10.0f));

    //m_cbvData0.model = (rot * trn * rot * scl).transpose();
    //m_cbvData1.model = (trn * rot * trn * scl).transpose();
}
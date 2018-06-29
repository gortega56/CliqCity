#include "MeshApplication.h"
#include "Engine.h"
#include "Resource\ResourceManager.h"
#include "Mesh.h"
#include <functional>
#include <memory>
//#include "Resource\Fbx.h"
#include "Resource\ObjResource.h"
#include "Platform\Window.h"
#include "Graphics.h"
#include "Console.h"
#include "Platform\Input.h"
#include <codecvt>
#include <iostream>
#include <sstream>

#define DIFF_PATH0 L".\\Assets\\BrickDiff.dds"
#define DIFF_PATH1 L".\\Assets\\RockPileDiff.dds"

#define NORM_PATH0 L".\\Assets\\BrickNorm.dds"
#define NORM_PATH1 L".\\Assets\\RockPileNorm.dds"

#define FBX_PATH0 ".\\Assets\\/*Prof_Animated*/.fbx"
#define FBX_PATH1 ".\\Assets\\cube.FBX"

#define SPONZA_OBJ_PATH ".\\Assets\\sponza_obj\\sponza.obj"
#define OBJ_PATH ".\\Assets\\cube.obj"

#define SPONZA_MTL_PATH ".\\Assets\\sponza_obj\\sponza.mtl"
#define SPONZA_TEX_PATH ".\\Assets\\sponza_textures\\"

using namespace Luz;
using namespace gmath;
static float g_scale = 5.0f;
Console g_console;

static int FindOrPushBackTextureName(std::vector<std::string>& textureNames, std::string textureName);

int FindOrPushBackTextureName(std::vector<std::string>& textureNames, std::string textureName)
{
    auto iter = std::find(textureNames.begin(), textureNames.end(), textureName);
    if (iter == textureNames.end())
    {
        textureNames.push_back(textureName);
        return (int)textureNames.size() - 1;
    }

    return (int)(iter - textureNames.begin());
}

MeshApplication::MeshApplication()
{
    m_renderableIndex = -1;
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

    Resource::Obj::Desc desc;
    desc.Filename = SPONZA_OBJ_PATH;
    desc.Directory = ".\\Assets\\sponza_obj\\";
    desc.TextureDirectory = ".\\Assets\\sponza_textures\\";
    auto loadingObj = Resource::Async<Resource::Obj>::Load(desc);

    m_cameraController = CameraController(m_engine->OS()->GetInput());

    PerspectiveCamera* pCamera = m_cameraController.GetCamera();
    pCamera->GetTransform()->SetPosition(0.0f, 0.0f, -15.0f);
    pCamera->SetFieldOfView(3.14f * 0.5f);
    pCamera->SetAspectRatio(m_window->AspectRatio());
    pCamera->SetNear(0.1f);
    pCamera->SetFar(3000.0f);

    m_cbvData.view = pCamera->GetView().transpose();
    m_cbvData.proj = pCamera->GetProjection().transpose();

    //rm.LoadResource<Resource::Fbx>(FBX_PATH1, [weakRenderable](std::shared_ptr<const Resource::Fbx> pFbx)
    //{
    //    if (!pFbx) return;

    //    if (auto sharedRenderable = weakRenderable.lock())
    //    {
    //        Mesh<Vertex, u32> mesh;
    //        pFbx->WriteVertices<Vertex>(mesh.Vertices(), [](Vertex& vertex, const Resource::Fbx::Vertex& fbx)
    //        {
    //            vertex.Position = *reinterpret_cast<const float3*>(&fbx.Position[0]);
    //            vertex.Normal = *reinterpret_cast<const float3*>(&fbx.Normal[0]);
    //            vertex.UV = *reinterpret_cast<const float2*>(&fbx.UV[0]);
    //        });

    //        mesh.SetIndices(pFbx->GetIndices());
    //        mesh.GenerateTangents();

    //        sharedRenderable->LoadMesh(&mesh);
    //        sharedRenderable->m_isRenderable.store(true);
    //    }
    //});

    m_vs = Graphics::CreateVertexShader("VertexShader.hlsl");
    m_ps = Graphics::CreatePixelShader("PixelShader.hlsl");

    Graphics::PipelineDesc pd;
    pd.Signature.AllowInputLayout()
        .DenyHS()
        .DenyDS()
        .DenyGS()
        .AppendConstantView(0)
        .AppendDescriptorTable(Graphics::SHADER_VISIBILITY_ALL)
        .AppendDescriptorTableRange(1, 26, 1, 0, Graphics::DescriptorTable::Range::DESCRIPTOR_TABLE_RANGE_TYPE_CONSTANT_VIEW)   // Array of CBVs
        .AppendDescriptorTableRange(1, 36, 0, 0, Graphics::DescriptorTable::Range::DESCRIPTOR_TABLE_RANGE_TYPE_SHADER_VIEW)     // Array of SRVs
        .AppendAnisotropicWrapSampler(0);
    pd.InputLayout.AppendFloat4("TANGENT")
        .AppendPosition3F()
        .AppendNormal3F()
        .AppendUV3();
    pd.VertexShaderHandle = m_vs;
    pd.PixelShaderHandle = m_ps;
    pd.Topology = Graphics::GFX_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pd.SampleCount = 1;
    pd.SampleQuality = 0;
    pd.SampleMask = 0xffffffff;

    pd.DepthStencil.DepthEnable = true;
    pd.DepthStencil.WriteMask = Graphics::GFX_DEPTH_WRITE_MASK_ALL;
    pd.DepthStencil.Comparison = Graphics::COMPARISON_TYPE_LESS;
    pd.DepthStencil.StencilReadMask = 0xff;
    pd.DepthStencil.StencilWriteMask = 0xff;
    pd.DepthStencil.StencilEnable = false;
    pd.DepthStencil.FrontFace.StencilFailOp = Graphics::GFX_STENCIL_OP_KEEP;
    pd.DepthStencil.FrontFace.StencilDepthFailOp = Graphics::GFX_STENCIL_OP_KEEP;
    pd.DepthStencil.FrontFace.StencilPassOp = Graphics::GFX_STENCIL_OP_KEEP;
    pd.DepthStencil.FrontFace.Comparison = Graphics::COMPARISON_TYPE_ALWAYS;
    pd.DepthStencil.BackFace.StencilFailOp = Graphics::GFX_STENCIL_OP_KEEP;
    pd.DepthStencil.BackFace.StencilDepthFailOp = Graphics::GFX_STENCIL_OP_KEEP;
    pd.DepthStencil.BackFace.StencilPassOp = Graphics::GFX_STENCIL_OP_KEEP;
    pd.DepthStencil.BackFace.Comparison = Graphics::COMPARISON_TYPE_ALWAYS;

    pd.Rasterizer.Fill = Graphics::GFX_FILL_MODE_SOLID;
    pd.Rasterizer.Cull = Graphics::GFX_CULL_MODE_BACK;
    pd.Rasterizer.FrontCounterClockwise = false;
    pd.Rasterizer.DepthBias = 0;
    pd.Rasterizer.DepthBiasClamp = 0.0f;
    pd.Rasterizer.SlopeScaledDepthBias = 0.0f;
    pd.Rasterizer.DepthClipEnable = true;
    pd.Rasterizer.MsaaEnable = false;
    pd.Rasterizer.AntialiasedLineEnable = false;
    pd.Rasterizer.RasterizationMode = Graphics::GFX_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    pd.Blend.AlphaToCoverageEnable = false;
    pd.Blend.IndependentBlendEnable = false;
    pd.Blend.BlendStates[0].BlendEnable = false;
    pd.Blend.BlendStates[0].LogicOpEnable = false;
    pd.Blend.BlendStates[0].SrcBlend = Graphics::GFX_BLEND_ONE;
    pd.Blend.BlendStates[0].DestBlend = Graphics::GFX_BLEND_ZERO;
    pd.Blend.BlendStates[0].BlendOp = Graphics::GFX_BLEND_OP_ADD;
    pd.Blend.BlendStates[0].SrcBlendAlpha = Graphics::GFX_BLEND_ONE;
    pd.Blend.BlendStates[0].DestBlendAlpha = Graphics::GFX_BLEND_ZERO;
    pd.Blend.BlendStates[0].BlendOpAlpha = Graphics::GFX_BLEND_OP_ADD;
    pd.Blend.BlendStates[0].LogicOp = Graphics::GFX_LOGIC_OP_NOOP;
    pd.Blend.BlendStates[0].RenderTargetWriteMask = Graphics::GFX_COLOR_WRITE_ENABLE_ALL;

    pd.UseSwapChain = true;

    m_opaquePipeline = Graphics::CreateGraphicsPipelineState(pd);
    
    pd.Blend.BlendStates[0].BlendEnable = true;
    pd.Blend.BlendStates[0].LogicOpEnable = false;
    pd.Blend.BlendStates[0].SrcBlend = Graphics::GFX_BLEND_SRC_ALPHA;
    pd.Blend.BlendStates[0].DestBlend = Graphics::GFX_BLEND_INV_SRC_ALPHA;
    pd.Blend.BlendStates[0].BlendOp = Graphics::GFX_BLEND_OP_ADD;
    pd.Blend.BlendStates[0].SrcBlendAlpha = Graphics::GFX_BLEND_ONE;
    pd.Blend.BlendStates[0].DestBlendAlpha = Graphics::GFX_BLEND_ZERO;
    pd.Blend.BlendStates[0].BlendOpAlpha = Graphics::GFX_BLEND_OP_ADD;
    pd.Blend.BlendStates[0].LogicOp = Graphics::GFX_LOGIC_OP_NOOP;
    pd.Blend.BlendStates[0].RenderTargetWriteMask = Graphics::GFX_COLOR_WRITE_ENABLE_ALL;

    m_transparentPipeline = Graphics::CreateGraphicsPipelineState(pd);


    std::vector<Graphics::Mesh<Vertex, u32>> meshes;
    std::vector<std::string> textureNames;

    std::shared_ptr<const Resource::Obj> pObj = loadingObj.Get();
    if (pObj)
    {
        // Create geo
        meshes.resize(static_cast<size_t>(pObj->GetNumSurfaces()));
        pObj->CreateStructuredSurfaces<Vertex, u32>([&](const u32 index, const Resource::Obj::StructuredSurface<Vertex, u32>& desc)
        {
            meshes[index].SetVertices(desc.VerticesPtr, desc.NumVertices);
            meshes[index].SetIndices(desc.IndicesPtr, desc.NumIndices);
            m_materialIndices.push_back(desc.MaterialHandle);
        });

        // Create mats
        for (u32 i = 0, numMaterials = pObj->GetNumMaterials(); i < numMaterials; ++i)
        {
            auto md = pObj->GetMaterialDesc(i);

            m_materialConstants.emplace_back();
            auto& mc = m_materialConstants.back();
            mc.SpecularExponent = md.SpecularExponent;
            mc.Transparency = md.Transparency;
            mc.OpticalDensity = md.OpticalDensity;
            mc.Dissolve = md.Dissolve;
            mc.Specular = float3(md.Specular[0], md.Specular[1], md.Specular[2]);
            mc.TransmissionFilter = float3(md.TransmissionFilter[0], md.TransmissionFilter[1], md.TransmissionFilter[2]);
            mc.Ambient = float3(md.Ambient[0], md.Ambient[1], md.Ambient[2]);
            mc.Diffuse = float3(md.Diffuse[0], md.Diffuse[1], md.Diffuse[2]);
            mc.Emissive = float3(md.Emissive[0], md.Emissive[1], md.Emissive[2]);

            if (strlen(md.DiffuseTextureName)) mc.TextureIndices[0] = FindOrPushBackTextureName(textureNames, md.DiffuseTextureName);
            if (strlen(md.NormalTextureName)) mc.TextureIndices[1] = FindOrPushBackTextureName(textureNames, md.NormalTextureName);
            if (strlen(md.DissolveTextureName)) mc.TextureIndices[2] = FindOrPushBackTextureName(textureNames, md.DissolveTextureName);
        }
    }

    m_surfaces.resize(meshes.size());
    for (u32 i = 0, count = static_cast<u32>(meshes.size()); i < count; ++i)
    {
        auto& mesh = meshes[i];
        
        Graphics::BufferDesc vbd;
        vbd.Alignment = 0;
        vbd.SizeInBytes = mesh.NumVertexBytes();
        vbd.StrideInBytes = static_cast<u16>(mesh.VertexStride());
        vbd.pData = mesh.Vertices();

        Graphics::BufferDesc ibd;
        ibd.Alignment = 0;
        ibd.SizeInBytes = mesh.NumIndexBytes();
        ibd.StrideInBytes = mesh.IndexStride();
        ibd.pData = mesh.Indices();

        auto& surface = m_surfaces[i];
        surface.vb = Graphics::CreateVertexBuffer(vbd);
        surface.ib = Graphics::CreateIndexBuffer(ibd);
       // surface.isReady = true;
    }

    Graphics::ConstantBufferDesc cbd;
    cbd.Alignment = 0;
    cbd.SizeInBytes = sizeof(ConstantBufferData);
    cbd.StrideInBytes = sizeof(ConstantBufferData);
    cbd.AllocHeap = false;
    cbd.pData = &m_cbvData;
    m_viewProjectionHandle = Graphics::CreateConstantBuffer(cbd);

    cbd.SizeInBytes = sizeof(PhongMaterial);
    cbd.StrideInBytes = sizeof(PhongMaterial);
    cbd.AllocHeap = true;
    for (u32 i = 0, count = static_cast<u32>(m_materialConstants.size()); i < count; ++i)
    {
        cbd.pData = &m_materialConstants[i];
        Graphics::ConstantBufferHandle cbHandle = Graphics::CreateConstantBuffer(cbd);
        if (i == 0) m_baseDescriptorHandle = cbHandle;
    }

    for (u32 i = 0, count = static_cast<u32>(textureNames.size()); i < count; ++i)
    {
        Graphics::TextureFileDesc td;
        td.Filename = textureNames[i].c_str();
        td.GenMips = true;
        Graphics::CreateTexture(td);
    }
    
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
    static const float clear[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
    static const Graphics::Viewport vp = { 0.0f, 0.0f, 1600.0f, 900.0f, 0.0f, 1.0f };
    static const Graphics::Rect scissor = { 0, 0, 1600, 900 };
    
    Graphics::UpdateConstantBuffer(&m_cbvData, sizeof(m_cbvData), m_viewProjectionHandle);

    Graphics::CommandStreamDesc csd;
    csd.PipelineHandle = m_opaquePipeline;
    csd.QueueType = Graphics::GFX_COMMAND_QUEUE_TYPE_MAIN;

    Graphics::CommandStream cs;
    Graphics::CreateCommandStream(csd, &cs);

    cs.SetPipeline(m_opaquePipeline);
    cs.SetRenderTargets();
    cs.ClearRenderTarget(clear);
    cs.ClearDepthStencil(1.0f, 0);
    cs.SetViewport(vp);
    cs.SetScissorRect(scissor);
    cs.SetConstantBuffer(0, m_viewProjectionHandle);
    cs.SetDescriptorTable(1, m_baseDescriptorHandle);
    // TODO: Descriptor heaps
    // TODO: Descriptor handles
    
    cs.SetPrimitiveTopology(Graphics::GFX_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    if (m_renderableIndex != -1)
    {
        auto& surface = m_surfaces[m_renderableIndex];
        cs.SetVertexBuffer(surface.vb);
        cs.SetIndexBuffer(surface.ib);
        cs.DrawInstanceIndexed(surface.ib);
    }
    else
    {
        for (u32 i = 0, count = (u32)m_surfaces.size(); i < count; ++i)
        {
            if (m_materialIndices[i] == -1) continue;
            auto& surface = m_surfaces[i];
            cs.SetVertexBuffer(surface.vb);
            cs.SetIndexBuffer(surface.ib);
            cs.DrawInstanceIndexed(surface.ib);
        }
    }

    Graphics::SubmitCommandStream(&cs, false);
    
  //  cs.SetPipeline(m_transparentPipeline);
    
    Graphics::Present();

    frame++;

    auto pInput = m_engine->OS()->GetInput();
    if (pInput)
    {
        if (pInput->GetKeyUp(KEYCODE_UP))
        {
            m_renderableIndex += 1;
            if (m_renderableIndex >= (i32)m_surfaces.size())
            {
                m_renderableIndex = -1;
            }

            std::cout << "RenderableIndex: " << m_renderableIndex << std::endl;
        }
    }
}

void MeshApplication::FixedUpdate(double dt)
{
    m_cameraController.Update(dt);
    m_cbvData.view = m_cameraController.GetCamera()->GetView().transpose();
}
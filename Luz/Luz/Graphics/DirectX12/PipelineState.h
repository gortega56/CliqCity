#pragma once
#ifndef DX12_PIPELINESTATE_H
#define DX12_PIPELINESTATE_H

#ifndef DX12_ROOTSIGNATURE_H
#include "RootSignature.h"
#endif

#ifndef DX12_GPUSTATE_H
#include "GpuState.h"
#endif

#ifndef DX12_SHADER_H
#include "Shader.h"
#endif

namespace Dx12
{
    class PixelBuffer;
    class RenderContext;
    class Device;

    class LUZ_API PipelineState
    {
    public:
        PipelineState() : m_pipelineState(nullptr) {}
        ~PipelineState();

        ID3D12PipelineState* PSO() { return m_pipelineState; }

    protected:
        ID3D12PipelineState* m_pipelineState;
    };

    class LUZ_API GraphicsPipeline : public PipelineState
    {
    public:
        GraphicsPipeline();
        ~GraphicsPipeline();

        void SetInputLayout(InputLayout* pInputLayout) { m_desc.InputLayout = pInputLayout->Desc(); }
        void SetRootSignature(RootSignature* pSignature) { m_desc.pRootSignature = pSignature->Signature(); }

        void SetPointTopology() { m_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT; }
        void SetLineTopology() { m_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE; }
        void SetPatchTopology() { m_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH; }
        void SetTriangleTopology() { m_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; }

        void SetVS(Shader* pShader) { m_desc.VS = pShader->ByteCode(); }
        void SetHS(Shader* pShader) { m_desc.HS = pShader->ByteCode(); }
        void SetDS(Shader* pShader) { m_desc.DS = pShader->ByteCode(); }
        void SetGS(Shader* pShader) { m_desc.GS = pShader->ByteCode(); }
        void SetPS(Shader* pShader) { m_desc.PS = pShader->ByteCode(); }

        void SetSampleCount(u32 count) { m_desc.SampleDesc.Count = count; }
        void SetSampleQuality(u32 quality) { m_desc.SampleDesc.Quality = quality; }
        void SetSampleMask(u32 mask) { m_desc.SampleMask = mask; }

        void SetRenderTargets();
        void SetRenderTargets(const RenderContext* pRc);
        void SetRasterizerState(RasterizerState* pRS) { m_desc.RasterizerState = pRS->Desc(); }
        void SetDepthStencilState(DepthStencilState* pDSS) { m_desc.DepthStencilState = pDSS->Desc(); }
        void SetBlendState(BlendState* pBS) { m_desc.BlendState = pBS->Desc(); }

        bool Finalize();
        D3D12_GRAPHICS_PIPELINE_STATE_DESC m_desc;

    private:
    };
}

#endif
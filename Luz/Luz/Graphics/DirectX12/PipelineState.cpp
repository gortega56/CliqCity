#include "stdafx.h"
#include "PipelineState.h"
#include "dx12_renderer.h"
#include "GpuResource.h"

using namespace dx12;

GraphicsPipeline::GraphicsPipeline()
{
}


GraphicsPipeline::~GraphicsPipeline()
{
}

void GraphicsPipeline::SetRenderTargets(Renderer* pRenderer)
{
    ColorBuffer* pRtvs = pRenderer->m_renderContext.RTV();

    m_desc.NumRenderTargets = pRtvs->NumResources();

    for (UINT i = 0; i < m_desc.NumRenderTargets; ++i)
    {
        m_desc.RTVFormats[i] = pRtvs->Format(i);
    }
}

bool GraphicsPipeline::Finalize(Renderer* pRenderer)
{
    HRESULT hr = pRenderer->m_device.DX()->CreateGraphicsPipelineState(&m_desc, IID_PPV_ARGS(&m_pipelineState));
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}
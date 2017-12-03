#include "stdafx.h"
#include "PipelineState.h"
#include "dx12_internal.h"
#include "GpuResource.h"
#include "RenderContext.h"
#include "Device.h"
#include "Dx12Graphics.h"

using namespace Dx12;

PipelineState::~PipelineState()
{
    SAFE_RELEASE(m_pipelineState);
}

GraphicsPipeline::GraphicsPipeline()
{
}


GraphicsPipeline::~GraphicsPipeline()
{
}

void GraphicsPipeline::SetRenderTargets()
{
    SetRenderTargets(SharedSwapChainContext());
}

void GraphicsPipeline::SetRenderTargets(const RenderContext* pRenderContext)
{
    m_desc.NumRenderTargets = pRenderContext->NumRenderTargetViews();

    for (UINT i = 0; i < m_desc.NumRenderTargets; ++i)
    {
        m_desc.RTVFormats[i] = pRenderContext->Buffer(i)->Format();
    }

    m_desc.DSVFormat = pRenderContext->Depth()->Format();
}

bool GraphicsPipeline::Finalize()
{
    HRESULT hr = Device::SharedInstance()->DX()->CreateGraphicsPipelineState(&m_desc, IID_PPV_ARGS(&m_pipelineState));
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}
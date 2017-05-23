#include "stdafx.h"
#include "PipelineState.h"
#include "dx12_internal.h"
#include "GpuResource.h"
#include "RenderContext.h"
#include "Device.h"

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

void GraphicsPipeline::SetRenderTargets(std::shared_ptr<const RenderContext> pRenderContext)
{
    m_desc.NumRenderTargets = 1;//pRenderContext->NumRTVs();

    for (UINT i = 0; i < m_desc.NumRenderTargets; ++i)
    {
        m_desc.RTVFormats[i] = DXGI_FORMAT_R8G8B8A8_UNORM;// pRenderContext->RTV(i)->Format();
    }

    m_desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
}

bool GraphicsPipeline::Finalize(std::shared_ptr<const Device> pDevice)
{
    HRESULT hr = pDevice->DX()->CreateGraphicsPipelineState(&m_desc, IID_PPV_ARGS(&m_pipelineState));
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}
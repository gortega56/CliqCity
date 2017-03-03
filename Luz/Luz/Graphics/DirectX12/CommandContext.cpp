#include "stdafx.h"
#include "CommandContext.h"
#include "dx12_renderer.h"
#include "PipelineState.h"

using namespace dx12;

#pragma region CommandContext

CommandContext::CommandContext(u32 numThreads) : 
    m_commandAllocators(numThreads), 
    m_fences(numThreads), 
    m_fenceValues(numThreads), 
    m_fenceEvent(nullptr), 
    m_numThreads(numThreads)
{

}

CommandContext::~CommandContext()
{

}

bool CommandContext::WaitForFence(int i)
{
    HRESULT hr;

    bool result = true;

    // if the current fence value is still less than "fenceValue", then we know the GPU has not finished executing
    // the command queue since it has not reached the "commandQueue->Signal(fence, fenceValue)" command
    if (m_fences[i]->GetCompletedValue() < m_fenceValues[i])
    {
        hr = m_fences[i]->SetEventOnCompletion(m_fenceValues[i], m_fenceEvent);
        if (FAILED(hr))
        {
            result = false;
        }

        // We will wait until the fence has triggered the event that it's current value has reached "fenceValue". once it's value
        // has reached "fenceValue", we know the command queue has finished executing
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }

    // increment fenceValue for next frame
    m_fenceValues[i]++;

    return result;
}

#pragma endregion CommandContext

#pragma region GraphicsCommandContext

GraphicsCommandContext::GraphicsCommandContext(u32 numFrameBuffers) : CommandContext(numFrameBuffers), m_numFrameBuffers(numFrameBuffers), m_commandList(nullptr)
{
    
}

GraphicsCommandContext::~GraphicsCommandContext()
{

}

void GraphicsCommandContext::Release(Renderer* pRenderer)
{
    for (u32 i = 0; i < m_numFrameBuffers; ++i)
    {
        m_frameIndex = i;
        WaitForFence();
        m_frameIndex = pRenderer->m_device.SwapChain3()->GetCurrentBackBufferIndex();
    }

    for (u32 i = 0, num = m_numThreads * m_numFrameBuffers; i < num; ++i)
    {
        SAFE_RELEASE(m_commandAllocators[i]);
        SAFE_RELEASE(m_fences[i]);
    };
}

bool GraphicsCommandContext::Initialize(Renderer* pRenderer)
{
    ID3D12Device* pDevice = pRenderer->m_device.DX();

    if (!CreateGraphicsCommandAllocators(pDevice, m_commandAllocators.data(), (UINT)m_numThreads))
    {
        return false;
    }

    if (!CreateGraphicsCommandList(pDevice, m_commandAllocators[0], nullptr, &m_commandList))
    {
        return false;
    }

    if (!CreateFences(pDevice, m_fences.data(), m_fenceValues.data(), D3D12_FENCE_FLAG_NONE, (UINT)m_numThreads))
    {
        return false;
    }

    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_fenceEvent == nullptr)
    {
        return false;
    }

    m_frameIndex = pRenderer->m_device.SwapChain3()->GetCurrentBackBufferIndex();

    //for (int i = 0, num = pCommandList->NumAllocators(); i < num; ++i)
    //{
    //    hr = pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCommandList->m_commandAllocators[i]));
    //    if (FAILED(hr))
    //    {
    //        return false;
    //    }
    //}

    //// Create command list using first allocator
    //hr = pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandList->m_commandAllocators[0], nullptr, IID_PPV_ARGS(&pCommandList->m_commandList));
    //if (FAILED(hr))
    //{
    //    return false;
    //}

    //// Create fence
    //for (int i = 0, num = pCommandList->NumAllocators(); i < num; ++i)
    //{
    //    hr = pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pCommandList->m_fences[i]));
    //    if (FAILED(hr))
    //    {
    //        return false;
    //    }

    //    pCommandList->m_fenceValues[i] = 0;
    //}

    //pCommandList->m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    //if (pCommandList->m_fenceEvent == nullptr)
    //{
    //    return false;
    //}

    return true;
}

bool GraphicsCommandContext::Reset(GraphicsPipeline* pGraphicsPipeline)
{
    bool running;
    if (!WaitForFence())
    {
        running = false;
    }

    HRESULT hr = m_commandAllocators[m_frameIndex]->Reset();
    if (FAILED(hr))
    {
        running = false;
    }

    hr = m_commandList->Reset(m_commandAllocators[m_frameIndex], pGraphicsPipeline->PSO());
    if (FAILED(hr))
    {
        running = false;
    }

    return running;
}

bool GraphicsCommandContext::Close()
{
    HRESULT hr = m_commandList->Close();
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

bool GraphicsCommandContext::Execute(Renderer* pRenderer)
{
    if (!Close())
    {
        return false;
    }

    // Create array of command lists
    ID3D12CommandList* ppCommandLists[] = { m_commandList };

    // execute command lists
    pRenderer->m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // add signal to command queue so the fence value gets updated
    HRESULT hr = pRenderer->m_commandQueue->Signal(m_fences[m_frameIndex], m_fenceValues[m_frameIndex]);
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}
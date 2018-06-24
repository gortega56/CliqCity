#include "stdafx.h"
#include "CommandContext.h"
#include "dx12_internal.h"

using namespace Dx12;

#pragma region CommandAllocator

CommandAllocator::CommandAllocator() : m_allocator(nullptr)
{

}

bool CommandAllocator::Initialize(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE type)
{
    if (!CreateCommandAllocators(pDevice, m_allocator.ReleaseAndGetAddressOf(), 1, type))
    {
        return false;
    }

    if (!m_fence.Initialize(pDevice))
    {
        return false;
    }

    return true;
}

#pragma endregion

#pragma region CommandAllocatorPool

static const D3D12_COMMAND_LIST_TYPE g_commandListTypes[] =
{
    D3D12_COMMAND_LIST_TYPE_DIRECT,
    D3D12_COMMAND_LIST_TYPE_BUNDLE,
    D3D12_COMMAND_LIST_TYPE_COMPUTE,
    D3D12_COMMAND_LIST_TYPE_COPY
};

static const int g_numCommandListTypes = 4;
static CommandAllocatorPool* g_allocatorPools = nullptr;
static std::mutex g_allocatorPoolMutex;

static u8 g_memory[sizeof(CommandAllocatorPool) * g_numCommandListTypes];

CommandAllocatorPool::CommandAllocatorPool()
{

}

bool CommandAllocatorPool::Initialize(ID3D12Device* pDevice)
{
    LUZASSERT(!g_allocatorPools);

    std::lock_guard<std::mutex> lock(g_allocatorPoolMutex);

    for (int i = 0; i < g_numCommandListTypes; ++i)
    {
        auto pool = new (&g_memory[sizeof(CommandAllocatorPool) * i]) CommandAllocatorPool();
        for (u32 j = 0; j < sm_maxAllocatorsPerType; ++j)
        {
            if (!pool->m_allocators[j].Initialize(pDevice, g_commandListTypes[i]))
            {
                return false;
            }
        }
    }

    g_allocatorPools = reinterpret_cast<CommandAllocatorPool*>(g_memory);

    //for (int typeIndex = 0; typeIndex < g_numCommandListTypes; ++typeIndex)
    //{
    //    D3D12_COMMAND_LIST_TYPE listType = g_commandListTypes[typeIndex];
    //    for (u32 i = 0; i < sm_maxAllocatorsPerType; ++i)
    //    {
    //        if (!g_allocatorPools[listType].m_allocators[i].Initialize(listType))
    //        {
    //            return false;
    //        }
    //    }
    //}

    return true;
}

CommandAllocator* CommandAllocatorPool::Allocate(D3D12_COMMAND_LIST_TYPE type)
{
    std::lock_guard<std::mutex> lock(g_allocatorPoolMutex);

    auto& allocatorPool = g_allocatorPools[type];

    UINT64 min = UINT64_MAX;
    int idx = 0;

    CommandAllocator* pCommandAllocator = nullptr;

    // Just look for any ctx that we aren't waiting for
    for (int i = 0, count = (int)sm_maxAllocatorsPerType; i < count; ++i)
    {
        CommandAllocator* iter = &allocatorPool.m_allocators[i];
        if (iter->GetFence().IsIdle())
        {
            pCommandAllocator = iter;
            break;
        }
        else if (iter->GetFence().IsWaiting())
        {
            UINT64 sig = iter->GetFence().Signal();
            if (sig < min)
            {
                min = sig;
                idx = i;
            }
        }
    }

    if (!pCommandAllocator)
    {
        pCommandAllocator = &allocatorPool.m_allocators[idx];

        // We know this guy WAS waiting... but we gotta check again...
        if (pCommandAllocator->GetFence().IsWaiting())
        {
            pCommandAllocator->GetFence().Wait();
        }

        HRESULT hr = pCommandAllocator->Allocator()->Reset();
        LUZASSERT(SUCCEEDED(hr));
    }

    // Increment so we know it's in use
    pCommandAllocator->GetFence().IncrementSignal();
    
    return pCommandAllocator;
}

void CommandAllocatorPool::Destroy()
{
    WaitAll();

    LUZASSERT(g_allocatorPools);
    std::lock_guard<std::mutex> lock(g_allocatorPoolMutex);

    for (int i = 0; i < g_numCommandListTypes; ++i)
    {
        g_allocatorPools[i].~CommandAllocatorPool();
    }

    ZeroMemory(g_memory, sizeof(CommandAllocatorPool) * g_numCommandListTypes);
    g_allocatorPools = nullptr;
}

void CommandAllocatorPool::WaitAll()
{
    for (int typeIndex = 0; typeIndex < g_numCommandListTypes; ++typeIndex)
    {
        D3D12_COMMAND_LIST_TYPE listType = g_commandListTypes[typeIndex];
        for (u32 i = 0; i < sm_maxAllocatorsPerType; ++i)
        {
            g_allocatorPools[listType].m_allocators[i].GetFence().Wait();
        }
    }
}

#pragma endregion
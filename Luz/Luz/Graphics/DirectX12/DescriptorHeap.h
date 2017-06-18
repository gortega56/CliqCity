#pragma once
#ifndef DX12_DESCRIPTORHEAP_H
#define DX12_DESCRIPTORHEAP_H

#ifndef __D3DX12_H__
#include "d3dx12.h"
#endif

namespace Dx12
{
    class Device;
    class UploadBuffer;
    class PixelBuffer;

    struct DescriptorHeapParams
    {
        D3D12_DESCRIPTOR_HEAP_TYPE Type;
        D3D12_DESCRIPTOR_HEAP_FLAGS Flags;
    };

    static const DescriptorHeapParams RtvHeapParams = { D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE };
    static const DescriptorHeapParams DsvHeapParams = { D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE };
    static const DescriptorHeapParams CbvSrvUavHeapParams = { D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE };
    static const DescriptorHeapParams SamplerHeapParams = { D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE };

    class DescriptorHeap
    {
    public:
        DescriptorHeap(u32 numDescriptors = 1);
        ~DescriptorHeap();

        DescriptorHeap(DescriptorHeap&& other);
        DescriptorHeap& operator=(DescriptorHeap&& other);

        ID3D12DescriptorHeap* Native() const { return m_descriptorHeap; }
        ID3D12DescriptorHeap** Address() { return &m_descriptorHeap; }
        u32 Size() const { return m_descriptorHeapSize; }
        u32 NumDescriptors() { return m_numDescriptors; }

        CD3DX12_CPU_DESCRIPTOR_HANDLE CpuHandle(int i = 0);

        bool Initialize(std::shared_ptr<const Device> pDevice, DescriptorHeapParams const* pParams, std::wstring name);

        bool InitializeRTV(std::shared_ptr<const Device> pDevice, std::wstring name);
        bool InitializeDSV(std::shared_ptr<const Device> pDevice, std::wstring name);
        bool InitializeMixed(std::shared_ptr<const Device> pDevice, std::wstring name);
        bool InitializeSampler(std::shared_ptr<const Device> pDevice, std::wstring name);

        void CreateShaderResourceViews(std::shared_ptr<const Device> pDevice, const PixelBuffer* pixelBuffer, int offset = 0);

    private:
        ID3D12DescriptorHeap* m_descriptorHeap;
        u32 m_descriptorHeapSize;
        u32 m_numDescriptors;
        D3D12_DESCRIPTOR_HEAP_TYPE m_type;
    
        DescriptorHeap(const DescriptorHeap& other) = delete;
        DescriptorHeap& operator=(const DescriptorHeap& other) = delete;
    };
}

#endif
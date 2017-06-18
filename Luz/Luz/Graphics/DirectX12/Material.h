#pragma once
#ifndef DX12_MATERIAL_H
#define DX12_MATERIAL_H

#ifndef DX12_GPURESOURCE_H
#include "GpuResource.h"
#endif

#include <map>
#include <string>

namespace Dx12
{
    class RootSignature;

    namespace Material
    {
        enum ParamType
        {
            MATERIAL_PARAM_TYPE_CBV,
            MATERIAL_PARAM_TYPE_SRV,
            MATERIAL_PARAM_TYPE_UAV,
            MATERIAL_PARAM_TYPE_TABLE
        };

        struct ResourceParam
        {
            u32 Index;
            u32 HeapIndex;
            u32 HeapOffset;
            ParamType Type;
        
            ResourceParam(u32 index, u32 heapIndex, u32 heapOffset, ParamType type) :
                Index(index), HeapIndex(heapIndex), HeapOffset(heapOffset), Type(type)
            {

            }
        };

        class Immutable
        {
        public:
            Immutable(std::shared_ptr<const RootSignature> rootSignature);
            Immutable();
            ~Immutable() = default;

            inline void SetRootSignature(std::shared_ptr<const RootSignature> rootSignature) { m_rootSignature = rootSignature; }
            
            void SetRootConstantBufferView(std::shared_ptr<const UploadBuffer> uploadBuffer, u32 paramIndex, u32 heapOffset = 0);
            void SetRootDescriptorTable(std::shared_ptr<const DescriptorHeap> descriptorHeap, std::vector<std::shared_ptr<const GpuResource>> gpuResources, u32 paramIndex, u32 heapOffset = 0);
            void Prepare(GraphicsCommandContext* pGraphicsContext);

        private:
            std::vector<std::shared_ptr<const DescriptorHeap>> m_descriptorHeaps;
            std::vector<std::shared_ptr<const GpuResource>> m_gpuResources;
            std::vector<ResourceParam> m_resourceParams;
            std::shared_ptr<const RootSignature> m_rootSignature;
        };

        

        //class Param
        //{
        //public:
        //    Param(u32 index) : m_index(index) {}

        //    inline u32 Index() const { return m_index; }
        //    u32 ShaderRegister() const;
        //protected:
        //    u32 m_index;
        //    u32 m_shaderRegister;
        //};

        //class RangeParam
        //{
        //public:
        //    RangeParam();

        //    u32 Offset() const;

        //protected:
        //    u32 m_offset;
        //};

        //class CbvPreloadData
        //{
        //public:
        //    CbvPreloadData();

        //    u32 BufferSize() const;
        //    u32 ElementSize() const;
        //    u32 NumElements() const;
        //    void* Data() const;

        //protected:
        //    u32 m_bufferSize;
        //    u32 m_elementSize;
        //    u32 m_numElements;
        //    void* m_data;
        //};

        //class CbvParam : public CbvPreloadData, public Param
        //{
        //public:
        //    CbvParam();

        //};

        //class SrvPreloadData : public RangeParam
        //{
        //public:
        //    SrvPreloadData();

        //    std::wstring& Filename() const;

        //protected:
        //    std::wstring m_filename;
        //};

        //class SrvParam : public Param
        //{
        //public:
        //    SrvParam();

        //    u32 NumResources() const;
        //    u32 Offset(int i) const;
        //    std::wstring& Filename(int i) const;
        //    
        //    std::vector<const SrvPreloadData>& PreloadData() const;
        //private:
        //    std::vector<const SrvPreloadData> m_preloadData;
        //};

        //class Builder
        //{
        //public:

        //    std::shared_ptr<const Immutable> ToImmutable();

        //private:
        //    std::shared_ptr<DescriptorHeap> m_resourceHeap;
        //    std::shared_ptr<DescriptorHeap> m_samplerHeap;
        //    std::vector<const Param> m_params;
        //    std::vector<const CbvParam> m_cbvParams;
        //    std::vector<const SrvParam> m_srvParams;
        //};
    }


    class Renderer;
    class DescriptorHeap;
    class RootSignature;
    class GraphicsCommandContext;

    //class Material : public std::enable_shared_from_this<Material>
    //{
    //public:
    //    Material(std::shared_ptr<const Renderer> pRenderer);
    //    ~Material();

    //    //bool Prepare(GraphicsCommandContext* pCtx);

    //    //bool Finalize();

    //private:
    //    std::vector<std::shared_ptr<const DescriptorHeap>> m_descriptorHeaps;
    //    std::shared_ptr<const RootSignature> m_rootSignature;
    //    std::weak_ptr<const Renderer> m_renderer;
    //};
}

namespace std
{
    //template<>
    //struct hash<Dx12::Material::ParamID>
    //{
    //    size_t operator()(const Dx12::Material::ParamID& param)
    //    {
    //        size_t h = 571;
    //        h ^= (unsigned)param.RootParamIndex;
    //        h ^= (unsigned)param.DescriptorHeapIndex;
    //        h ^= (unsigned)param.DescriptorHeapOffset;
    //        return h;
    //    }
    //};
}

#endif

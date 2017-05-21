#pragma once
#ifndef DX12_MATERIAL_H
#define DX12_MATERIAL_H

#ifndef DX12_GPURESOURCE_H
#include "GpuResource.h"
#endif

#include <map>

class Texture2D;

namespace Dx12
{
    /*struct ResourceParamBinding
    {
        u32 ParamIndex;
        
    };

    class Param
    {
    public:
        virtual void Finalize(std::shared_ptr<const CommandQueue> pQueue, std::shared_ptr<GraphicsCommandContext> pCtx) = 0;
    };


    class RootCbvParam : public Param
    {
    public:
        RootCbvParam();
        ~RootCbvParam();

        void Finalize(std::shared_ptr<const CommandQueue> pQueue, std::shared_ptr<GraphicsCommandContext> pCtx) override;
    private:
        D3D12_RESOURCE_DESC m_desc;
        void* m_data;
    };

    class Texture2dParam : public Param
    {
    public:
        Texture2dParam(std::string filename);
        ~Texture2dParam();

        void Finalize(std::shared_ptr<const CommandQueue> pQueue, std::shared_ptr<GraphicsCommandContext> pCtx) override;
    
    private:
        std::string m_filename;
    };*/

    class Renderer;
    class DescriptorHeap;
    class RootSignature;
    class GraphicsCommandContext;

    class Material : public std::enable_shared_from_this<Material>
    {
    public:
        Material(std::shared_ptr<const Renderer> pRenderer);
        ~Material();

        //bool Prepare(GraphicsCommandContext* pCtx);

        //bool Finalize();

    private:
        std::vector<std::shared_ptr<const DescriptorHeap>> m_descriptorHeaps;
        std::shared_ptr<const RootSignature> m_rootSignature;
        std::weak_ptr<const Renderer> m_renderer;
    };
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

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


    class Renderer;
    class DescriptorHeap;
    class RootSignature;
    class GraphicsCommandContext;

    class Material : public std::enable_shared_from_this<Material>
    {
    public:

        enum ParamType
        {
            UNKNOWN,
            ROOT_CBV,
            CBV,
            SRV,
            UAV,
            SAMPLER
        };

        enum TextureType
        {
            UNKNOWN,
            TEXTURE_1D,
            TEXTURE_2D,
            TEXTURE_3D,
            TEXTURE_ARRAY,
            TEXTURE_CUBE
        };

        struct ParamID
        {
            i32 RootParamIndex;
            i32 DescriptorHeapIndex;
            i32 DescriptorHeapOffset;
        };

        struct ParamKey
        {
            u32 Index;
            ParamType Type;
        };

        struct Param
        {
            union
            {
                struct RootCBV
                {
                    u64 BufferSize;
                    u32 ElementSize;
                    u32 NumElements;
                    void* Data;
                } AsRootCBV;

                struct Texture
                {
                    std::string Filename;
                    TextureType Type;
                } AsTexture;
            };
        };
        
        Material(std::shared_ptr<const Renderer> pRenderer);
        ~Material();

        template<class PARAM>
        void SetParam(const ParamID& paramID, std::shared_ptr<PARAM> param, std::map<const ParamID, std::shared_ptr<PARAM>>& map);

        void SetDescriptorTable(const ParamID& paramID, std::shared_ptr<const DescriptorHeap> param) {  SetParam<const DescriptorHeap>(paramID, param, m_descriptorMap); }
        void SetPixelBuffer(const ParamID& paramID, std::shared_ptr<const PixelBuffer> param) { SetParam<const PixelBuffer>(paramID, param, m_pixelBufferMap); }
        void SetTexture2D(const ParamID& paramID, std::shared_ptr<const Texture2D> pTexture);
        
        void SetConstantBuffer(const ParamID& paramID, std::shared_ptr<const UploadBuffer> param) { SetParam<const UploadBuffer>(paramID, param, m_constantBufferMap); }
        
        template<class PARAM>
        void SetConstantBuffer(const ParamID& paramID, std::shared_ptr<PARAM> param);

        bool Prepare(GraphicsCommandContext* pCtx);

        bool Finalize();

        void AppendParam(const ParamKey& key, const Param& param);
    private:


        std::vector<std::shared_ptr<const DescriptorHeap>> m_descriptorHeaps;

        std::map<const ParamID, std::shared_ptr<const DescriptorHeap>> m_descriptorMap;
        std::map<const ParamID, std::shared_ptr<const PixelBuffer>> m_pixelBufferMap;
        std::map<const ParamID, std::shared_ptr<const UploadBuffer>> m_constantBufferMap;
        std::shared_ptr<const RootSignature> m_rootSignature;
        std::weak_ptr<const Renderer> m_renderer;

    public:
        class Builder
        {
        public:
            Builder& AddRootConstantBufferView(const u32 paramIndex, const u64 bufferSize, const u32 elementSize, const u32 numElements, void* data);
            Builder& AddTexture2D(std::string filename, u32 paramIndex);

            std::shared_ptr<const Material> Finalize(std::shared_ptr<const Renderer> pRenderer);
        private:

            

            Param& AppendParam();
            std::vector<Param> m_params;
        };
    };

    template<class PARAM>
    void Material::SetParam(const ParamID& paramID, std::shared_ptr<PARAM> param, std::map<const ParamID, std::shared_ptr<PARAM>>& map)
    {
        if (map.find(paramID) == map.end())
        {
            map.insert({ paramID, std::move(param) });
        }
        else
        {
            map[paramID] = std::move(param);
        }
    }

    template<class PARAM>
    void Material::SetConstantBuffer(const ParamID& paramID, std::shared_ptr<PARAM> param)
    {
        auto uploadBuffer = std::make_shared<UploadBuffer>();
        uploadBuffer->Initialize()
    }
}

namespace std
{
    template<>
    struct hash<Dx12::Material::ParamID>
    {
        size_t operator()(const Dx12::Material::ParamID& param)
        {
            size_t h = 571;
            h ^= (unsigned)param.RootParamIndex;
            h ^= (unsigned)param.DescriptorHeapIndex;
            h ^= (unsigned)param.DescriptorHeapOffset;
            return h;
        }
    };
}

#endif

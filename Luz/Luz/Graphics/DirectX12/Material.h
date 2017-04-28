#pragma once
#ifndef DX12_MATERIAL_H
#define DX12_MATERIAL_H

#include <map>

class Texture2D;

enum class ParamID : int32_t;

namespace dx12
{
    class PixelBuffer;
    class UploadBuffer;
    class DescriptorHeap;
    class RootSignature;
    class GraphicsCommandContext;

    class Material : public std::enable_shared_from_this<Material>
    {
    public:
        Material();
        ~Material();

        template <class PARAM>
        void SetParam(const ParamID& paramID, std::shared_ptr<PARAM> param, std::map<const ParamID, std::shared_ptr<PARAM>>& map);
       // void SetDescriptorTable(const ParamID& paramID, std::shared_ptr<const DescriptorHeap> param) {  SetParam<const DescriptorHeap>(paramID, param, m_descriptorMap); }
        void SetPixelBuffer(const ParamID& paramID, std::shared_ptr<const PixelBuffer> param) { SetParam<const PixelBuffer>(paramID, param, m_pixelBufferMap); }
      //  void SetConstantBuffer(const ParamID& paramID, std::shared_ptr<const UploadBuffer> param) { SetParam<const UploadBuffer>(paramID, param, m_constantBufferMap); }
        
        void SetTexture2D(const ParamID& paramID, std::shared_ptr<const Texture2D> pTexture);
        
        bool Prepare(GraphicsCommandContext* pCtx);

    private:
        std::map<const ParamID, std::shared_ptr<const DescriptorHeap>> m_descriptorMap;
        std::map<const ParamID, std::shared_ptr<const PixelBuffer>> m_pixelBufferMap;
        std::map<const ParamID, std::shared_ptr<const UploadBuffer>> m_constantBufferMap;
        std::shared_ptr<const RootSignature> m_rootSignature;
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
}

#endif

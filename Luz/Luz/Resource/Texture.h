#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

namespace Internal
{
    template<class Impl> class Texture2D;
}

#ifdef _WIN64

#ifndef DX12_TEXTUREIMPL_H
#include "Graphics\DirectX12\TextureImpl.h"
#endif

namespace Dx12 { class TextureImpl; }
typedef Internal::Texture2D<Dx12::TextureImpl> Texture2D;

#elif _WIN32

#ifndef DX12_TEXTUREIMPL_H
#include "Graphics\DirectX12\TextureImpl.h"
#endif

namespace Dx12 { class TextureImpl; }
typedef Texture2D<Dx12::TextureImpl> Tex2D;

#elif __APPLE__
#endif

namespace Internal
{
    template<class Impl>
    class Texture2D
    {
    public:
        enum class Type : uint8_t
        {
            NONE,
            DIFFUSE,
            NORMAL,
            SPECULAR,
            HEIGHT,
            DEPTH,
            COUNT
        };

        //Texture2D(u64 width, u32 height, u32 imageSize, u16 mipLevels, u8* data, DXGI_FORMAT format, std::shared_ptr<std::vector<D3D12_SUBRESOURCE_DATA>> subresources);
        Texture2D() : m_pImpl(std::make_unique<Impl>()) {}
        ~Texture2D() {}

        const Impl* GetImpl() const { return m_pImpl.get(); }

        //u64 Width() const { return m_width; }
        //u32 Height() const { return m_height; }
        //u32 Size() const { return m_size; }
        //u16 MipLevels() const { return m_mipLevels; }
        //u8 const* Data() const { return m_data; }
        //DXGI_FORMAT GetDXGI() const { return m_format.asDXGI; }
        //std::shared_ptr<std::vector<D3D12_SUBRESOURCE_DATA>> GetSubresources() const { return m_subresources; }
        static std::shared_ptr<const Texture2D> Load(const std::wstring& filename);

    private:
        std::unique_ptr<Impl> m_pImpl;
        //u64 m_width;
        //u32 m_height;
        //u32 m_size;
        //u16 m_mipLevels;
        //u8* m_data;
        //union
        //{
        //    DXGI_FORMAT asDXGI;
        //} m_format;

        //std::shared_ptr<std::vector<D3D12_SUBRESOURCE_DATA>> m_subresources;
    };

    template<class Impl>
    std::shared_ptr<const Texture2D<Impl>> Texture2D<Impl>::Load(const std::wstring& filename)
    {
        auto pTex2d = std::make_shared<Texture2D<Impl>>();
        pTex2d->m_pImpl->Load(filename);
        return pTex2d;
    }
}

#endif
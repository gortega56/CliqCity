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
typedef Internal::Texture2D<Dx12::TextureImpl> Texture2D;

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

        Texture2D(std::string filename);
        ~Texture2D();

        Texture2D(Texture2D<Impl>&& other);
        Texture2D<Impl>& operator=(Texture2D<Impl>&& other);

        const Impl* GetImpl() const;

        static std::shared_ptr<const Texture2D> Load(const std::string& filename);

    private:
        std::unique_ptr<Impl> m_pImpl;

        Texture2D(const Texture2D<Impl>& other) = delete;
        Texture2D& operator=(const Texture2D<Impl>& other) = delete;
    };

    template<class Impl>
    Texture2D<Impl>::Texture2D(std::string filename) : m_pImpl(std::make_unique<Impl>(filename)) 
    {
    
    }

    template<class Impl>
    Texture2D<Impl>::~Texture2D() 
    {
    
    }

    template<class Impl>
    Texture2D<Impl>::Texture2D(Texture2D<Impl>&& other) : m_pImpl(std::move(other.m_pImpl))
    {
        other.m_pImpl = nullptr;
    }

    template<class Impl>
    Texture2D<Impl>& Texture2D<Impl>::operator=(Texture2D<Impl>&& other)
    {
        m_pImpl = std::move(m_pImpl);

        other.m_pImpl = nullptr;
        
        return *this;
    }

    template<class Impl>
    const Impl* Texture2D<Impl>::GetImpl() const 
    { 
        return m_pImpl.get(); 
    }

    template<class Impl>
    std::shared_ptr<const Texture2D<Impl>> Texture2D<Impl>::Load(const std::string& filename)
    {
        auto pTex2d = std::make_shared<Texture2D<Impl>>(filename);
        pTex2d->m_pImpl->Load();
        return pTex2d;
    }
}

#endif
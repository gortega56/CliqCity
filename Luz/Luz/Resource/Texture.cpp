#include "stdafx.h"
#include "Texture.h"
#include "DirectX12\TextureImpl.h"

namespace Resource
{
    std::shared_ptr<const Texture> Texture::Load(const std::string& filename)
    {
        auto pTexture = std::make_shared<Texture>();
#if _WIN64 || _WIN32
       
        auto pImpl = new Dx12::TextureImpl(filename);
        pImpl->Load();
        pTexture->m_pImpl = pImpl;
#elif __APPLE__

#endif
        return pTexture;
    }

    Texture::Texture()
        : m_pImpl(nullptr)
        , m_type(Type::NONE)
    {
        
    }

    Texture::~Texture()
    {
#if _WIN64 || _WIN32
        Dx12::TextureImpl const* p = static_cast<Dx12::TextureImpl const*>(m_pImpl);
        delete p;
#elif __APPLE__

#endif
    }

    Texture::Texture(Texture&& o)
        : m_pImpl(std::move(o.m_pImpl))
        , m_type(o.m_type)
    {
        o.m_type = Type::NONE;
    }

    Texture& Texture::operator=(Texture&& o)
    {
        m_pImpl = std::move(o.m_pImpl);
        m_type = o.m_type;

        o.m_type = Type::NONE;
        return *this;
    }
}
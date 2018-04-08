#pragma once
#ifndef TEXTURERESOURCE_H
#define TEXTURERESOURCE_H

namespace Resource
{
    class Texture
    {
    public:
        enum class Type : u8
        {
            NONE = 0,
            TEXTURE1D,
            TEXTURE2D,
            TEXTURE2DARRAY,
            TEXTURE3D,
            TEXTURECUBE
        };

        static std::shared_ptr<const Texture> Load(const std::string& filename);

        Texture();
        ~Texture();

        Texture(Texture&& other);
        Texture& operator=(Texture&& other);

        template<class T> const T* GetImpl() const;

    private:
        void* m_pImpl;
        Type m_type;

        Texture(const Texture& other) = delete;
        Texture& operator=(const Texture& other) = delete;
    };

    template<class T> const T* Texture::GetImpl() const
    {
        return static_cast<T*>(m_pImpl);
    }
}

#endif
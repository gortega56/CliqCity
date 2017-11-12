#pragma once
#ifndef DX12_TEXTUREIMPL_H
#define DX12_TEXTUREIMPL_H

namespace DirectX
{
    class ScratchImage;
}

namespace Dx12
{
    class LUZ_API TextureImpl
    {
    public:
        TextureImpl(std::wstring filename);
        ~TextureImpl();
    
        TextureImpl(TextureImpl&& other);
        TextureImpl& operator=(TextureImpl&& other);

        std::wstring Filename() const;
        size_t GetImageWidth(size_t mip, size_t item, size_t slice) const;
        size_t GetImageHeight(size_t mip, size_t item, size_t slice) const;
        size_t GetImageRowPitch(size_t mip, size_t item, size_t slice) const;
        size_t GetImageSlicePitch(size_t mip, size_t item, size_t slice) const;
        const u8* GetImagePixels(size_t mip, size_t item, size_t slice) const;

        bool Load() const;

    protected:
        friend class PixelBuffer;
        std::wstring m_filename;
        std::unique_ptr<DirectX::ScratchImage> m_image;
    
    private:
        TextureImpl(const TextureImpl& other) = delete;
        TextureImpl& operator=(const TextureImpl& other) = delete;
    };
}

#endif
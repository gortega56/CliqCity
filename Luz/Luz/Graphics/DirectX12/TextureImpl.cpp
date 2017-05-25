#include "stdafx.h"
#include "TextureImpl.h"
#include "dx12_internal.h"

using namespace Dx12;

TextureImpl::TextureImpl() : m_image(std::make_unique<DirectX::ScratchImage>())
{

}

TextureImpl::~TextureImpl()
{

}

size_t TextureImpl::GetImageWidth(size_t mip, size_t item, size_t slice) const
{
    return m_image->GetImage(mip, item, slice)->width;
}

size_t TextureImpl::GetImageHeight(size_t mip, size_t item, size_t slice) const
{
    return m_image->GetImage(mip, item, slice)->height;
}

size_t TextureImpl::GetImageRowPitch(size_t mip, size_t item, size_t slice) const
{
    return m_image->GetImage(mip, item, slice)->rowPitch;
}

size_t TextureImpl::GetImageSlicePitch(size_t mip, size_t item, size_t slice) const
{
    return m_image->GetImage(mip, item, slice)->slicePitch;
}

const u8* TextureImpl::GetImagePixels(size_t mip, size_t item, size_t slice) const
{
    return m_image->GetImage(mip, item, slice)->pixels;
}

bool TextureImpl::Load(const std::wstring& filename) const
{
    static bool coIntialized = false;
    if (!coIntialized)
    {
        CoInitialize(NULL);
        coIntialized = true;
    }

    DirectX::TexMetadata texMetaData;
    DirectX::ScratchImage scratchImage;
    HRESULT hr = DirectX::LoadFromWICFile(filename.c_str(), DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &texMetaData, scratchImage);
    if (FAILED(hr))
    {
        __debugbreak();
    }

    hr = DirectX::GenerateMipMaps(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), (DWORD)DirectX::TEX_FILTER_FLAGS::TEX_FILTER_DEFAULT, 0, *m_image);
    if (FAILED(hr))
    {
        __debugbreak();
    }

    return true;
}
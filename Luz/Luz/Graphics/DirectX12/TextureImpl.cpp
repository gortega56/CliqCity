#include "stdafx.h"
#include "TextureImpl.h"
#include "dx12_internal.h"
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

using namespace Dx12;

static bool LoadWICImage(std::wstring filename, DirectX::ScratchImage& outImage, bool genMipsIfNecessary = false);
static bool LoadDDSImage(std::wstring filename, DirectX::ScratchImage& outImage, bool genMipsIfNecessary = false);
static bool LoadTgaImage(std::wstring filename, DirectX::ScratchImage& outImage, bool genMipsIfNecessary = false);
static HRESULT GenerateMips(DirectX::ScratchImage& inImage, DirectX::ScratchImage& outImage);


bool LoadWICImage(std::wstring filename, DirectX::ScratchImage& outImage, bool genMipsIfNecessary /*= false*/)
{
    DirectX::TexMetadata texMetaData;
    DirectX::ScratchImage scratchImage;
    HRESULT hr = DirectX::LoadFromWICFile(filename.c_str(), DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &texMetaData, scratchImage);
    if (FAILED(hr))
    {
        return false;
    }

    hr = DirectX::GenerateMipMaps(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), (DWORD)DirectX::TEX_FILTER_FLAGS::TEX_FILTER_DEFAULT, 0, outImage);
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

bool LoadDDSImage(std::wstring filename, DirectX::ScratchImage& outImage, bool genMipsIfNecessary /*= false*/)
{
    DirectX::TexMetadata texMetaData;
    DirectX::ScratchImage scratchImage;
    HRESULT hr = DirectX::LoadFromDDSFile(filename.c_str(), DirectX::DDS_FLAGS_NONE, &texMetaData, scratchImage);
    if (FAILED(hr))
    {
        return false;
    }

    size_t mipLevels = texMetaData.mipLevels;
    if (mipLevels == 1 && genMipsIfNecessary)
    {
        hr = GenerateMips(scratchImage, outImage);
        if (FAILED(hr))
        {
            return false;
        }
    }
    else
    {
        outImage = std::move(scratchImage);
    }

    return true;
}

static bool LoadTgaImage(std::wstring filename, DirectX::ScratchImage& outImage, bool genMipsIfNecessary /*= false*/)
{
    DirectX::TexMetadata texMetaData;
    DirectX::ScratchImage scratchImage;
    HRESULT hr = DirectX::LoadFromTGAFile(filename.c_str(), &texMetaData, scratchImage);
    if (FAILED(hr))
    {
        return false;
    }

    size_t mipLevels = texMetaData.mipLevels;
    if (mipLevels == 1 && genMipsIfNecessary)
    {
        hr = GenerateMips(scratchImage, outImage);
        if (FAILED(hr))
        {
            return false;
        }
    }
    else
    {
        outImage = std::move(scratchImage);
    }

    return true;
}


HRESULT GenerateMips(DirectX::ScratchImage& inImage, DirectX::ScratchImage& outImage)
{
    return DirectX::GenerateMipMaps(inImage.GetImages(), inImage.GetImageCount(), inImage.GetMetadata(), (DWORD)DirectX::TEX_FILTER_FLAGS::TEX_FILTER_DEFAULT, 0, outImage);
}

TextureImpl::TextureImpl(std::string filename) : m_filename(std::wstring(filename.begin(), filename.end())), m_image(std::make_unique<DirectX::ScratchImage>())
{

}

TextureImpl::~TextureImpl()
{

}

TextureImpl::TextureImpl(TextureImpl&& other) : m_filename(other.m_filename), m_image(std::move(m_image))
{
    other.m_filename.clear();
    other.m_image = nullptr;
}

TextureImpl& TextureImpl::operator=(TextureImpl&& other)
{
    m_filename = other.m_filename;
    m_image = std::move(m_image);

    other.m_filename.clear();
    other.m_image = nullptr;

    return *this;
}

std::wstring TextureImpl::Filename() const
{
    return m_filename;
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

bool TextureImpl::Load() const
{
    static std::atomic_bool coIntialized = false;
    if (!coIntialized)
    {
        CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
        coIntialized = true;
    }

    LPCWSTR extension = PathFindExtension(m_filename.c_str());
    if (_wcsicmp(L".dds", extension) == 0)
    {
        return LoadDDSImage(m_filename, *m_image, true);
    }
    else if (_wcsicmp(L".tga", extension) == 0)
    {
        return LoadTgaImage(m_filename, *m_image, true);
    }
    else
    {
        return LoadWICImage(m_filename, *m_image, true);
    }

    DirectX::TexMetadata texMetaData;
    DirectX::ScratchImage scratchImage;
    HRESULT hr = DirectX::LoadFromWICFile(m_filename.c_str(), DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &texMetaData, scratchImage);
    if (FAILED(hr))
    {
        return false;
    }

    hr = DirectX::GenerateMipMaps(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), (DWORD)DirectX::TEX_FILTER_FLAGS::TEX_FILTER_DEFAULT, 0, *m_image);
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}
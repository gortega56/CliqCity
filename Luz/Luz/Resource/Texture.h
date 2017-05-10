#pragma once
#ifndef TEXTURE
#define TEXTURE

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

    Texture2D(u64 width, u32 height, u32 imageSize, u8* data, DXGI_FORMAT format);
    Texture2D();
    ~Texture2D();

    u64 Width() const { return m_width; }
    u32 Height() const { return m_height; }
    u32 Size() const { return m_size; }
    u8 const* Data() const { return m_data; }
    DXGI_FORMAT GetDXGI() const { return m_format.asDXGI; }

    static std::shared_ptr<Texture2D> Load(const std::wstring& filename);

private:
    u64 m_width;
    u32 m_height;
    u32 m_size;
    u8* m_data;

    union
    {
        DXGI_FORMAT asDXGI;
    } m_format;
};

#endif
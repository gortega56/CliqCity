#pragma once
#ifndef WINDOW_H
#define WINDOW_H

class LUZ_API Window
{
public:
    static std::shared_ptr<Window> Create(std::string caption, i32 width, i32 height, bool fullscreen);

    Window(int width, int height, bool fullscreen);
    virtual ~Window();

    inline int Width() { return m_width; }
    inline int Height() { return m_height; }
    inline bool FullScreen() { return m_fullScreen; }
    inline float AspectRatio() { return (float)m_width / (float)m_height; }

private:
    int m_width;
    int m_height;
    bool m_fullScreen;
};

#endif
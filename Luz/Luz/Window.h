#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include <Windows.h>
#include <string>

class Window
{
public:
    Window(int width, int height, bool fullscreen);
    virtual ~Window();

    inline int Width() { return mWidth; }
    inline int Height() { return mHeight; }
    inline bool FullScreen() { return mFullScreen; }

private:
    int mWidth;
    int mHeight;
    bool mFullScreen;
};

#endif
#pragma once
#ifndef MSPLATFORMWINDOW_H
#define MSPLATFORMWINDOW_H

#ifndef WINDOW_H
#include "Window.h"
#endif // !WINDOW_H

namespace MS
{
    class PlatformWindow : public Window
    {
    public:
        PlatformWindow(HWND hwnd, i32 width, i32 height, bool fullscreen);
        ~PlatformWindow();

        HWND Handle() const;

    private:
        HWND m_hwnd;
    };
}

#endif

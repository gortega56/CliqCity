#include "stdafx.h"
#include "MSPlatformWindow.h"

namespace MS
{
    PlatformWindow::PlatformWindow(HWND hwnd, i32 width, i32 height, bool fullscreen) :
        Window(width, height, fullscreen),
        m_hwnd(hwnd)
    {

    }

    PlatformWindow::~PlatformWindow() 
    {
        DestroyWindow(m_hwnd);
    };

    HWND PlatformWindow::Handle() const
    {
        return m_hwnd;
    }
}

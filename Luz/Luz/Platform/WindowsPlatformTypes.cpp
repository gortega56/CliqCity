#include <stdafx.h>
#include "WindowsPlatformTypes.h"

namespace Platform
{
    HINSTANCE s_hInstance = 0;

    HINSTANCE s_hPrevInstance = 0;

    LPSTR s_lpCmdLine = 0;

    int s_nShowCmd = 0;

    WindowsMessageCenter s_messageBus = WindowsMessageCenter();

    std::atomic_bool s_quit = ATOMIC_VAR_INIT(0);

    Window::Window(HWND hwnd, i32 width, i32 height, bool fullscreen) :
        ::Window(width, height, fullscreen),
        m_hwnd(hwnd)
    {

    }

    Window::~Window()
    {
        DestroyWindow(m_hwnd);
    };

    HWND Window::Handle() const
    {
        return m_hwnd;
    }

    Console s_console;
}

namespace Luz
{
    Input s_input = Input();
}
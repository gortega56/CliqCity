#pragma once
#ifndef WINDOWSPLATFORMTYPES_H
#define WINDOWSPLATFORMTYPES_H

#pragma region Platform Independent

#ifndef PLATFORM_H
#include "Platform.h"
#endif

#ifndef WINDOW_H
#include "Window.h"
#endif
#pragma endregion

#pragma region Platform Dependent

#include <Windows.h>
#include <wrl.h>
#include <Windowsx.h>
#include <xinput.h>

#ifndef WINDOWSMESSAGECENTER_H
#include "WindowsMessageCenter.h"
#endif

#pragma endregion

namespace Platform
{
    extern HINSTANCE s_hInstance;

    extern HINSTANCE s_hPrevInstance;

    extern LPSTR s_lpCmdLine;

    extern int s_nShowCmd;

    extern WindowsMessageCenter s_messageBus;

    extern std::atomic_bool s_quit;

    class Window : public ::Window
    {
    public:
        Window(HWND hwnd, i32 width, i32 height, bool fullscreen);
        
        ~Window();

        HWND Handle() const;

    private:
        HWND m_hwnd;
    };

    struct Console
    {
        FILE* Cin;
        FILE* Cout;
        FILE* Cerr;
    };

    extern Console s_console;

    UINT GetWindowsMessage(const NotificationType type);

    UINT GetWindowsMouseInputType(const MouseInputType type);

    UINT GetKeyboardInputType(const KeyboardInputType type);

    NotificationType GetNotificationType(const UINT message);

    MouseInputType GetMouseInputType(const UINT message);

    KeyboardInputType GetKeyboardInputType(const UINT message);
}

#endif
#include "stdafx.h"
#include "PlatformImgui.h"
#include "Platform.h"

#ifdef WINXX
#include "PlatformWindow.h"
#include "PlatformEvents.h"
#include "Platform/Win64/PlatformTypes_Win64.h"

#include "imgui.h"
#include "examples/imgui_impl_win32.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Platform
{

    static LRESULT WndProc_Imgui(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        return ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
    }

    bool Initialize_Imgui(const WindowHandle hWindow)
    {
        Register_EventFunction(&WndProc_Imgui);

        return ImGui_ImplWin32_Init(Platform::s_pWindows[hWindow].hwnd);
    }

    void Update_Imgui()
    {
        ImGui_ImplWin32_NewFrame();
    }

    void Shutdown_Imgui()
    {
        ImGui_ImplWin32_Shutdown();
    }
}

#endif
#include "stdafx.h"
#include "Window.h"

#if _WIN64 || _WIN32
#include "WindowsPlatform.h"
#include "windows_utils.h"
#include "MSPlatformWindow.h"
#include "WindowsMessageCenter.h"
HINSTANCE MS::Windows::sm_hinstance;
#elif __APPLE__

#endif

std::shared_ptr<Window> Window::Create(std::string caption, i32 width, i32 height, bool fullscreen)
{
#if _WIN64 || _WIN32
    HWND hwnd = MakeWindowInternal(MS::Windows::GetHInstance(), WinProc, L"BaseWindow", std::wstring(caption.begin(), caption.end()).c_str(), width, height, fullscreen);
    return std::make_shared<MS::PlatformWindow>(hwnd, width, height, fullscreen);
#elif __APPLE__
    return nullptr;
#endif
}


Window::Window(int width, int height, bool fullscreen) : m_width(width), m_height(height), m_fullScreen(fullscreen)
{

}

Window::~Window()
{

}



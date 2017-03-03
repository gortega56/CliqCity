#include "stdafx.h"
#include "OSWin.h"
#include "Window.h"
#include "windows_utils.h"
#include "Options.h"
#include "WindowsMessageCenter.h"

OSWin::OSWin() : m_shouldQuit(false)
{

}

OSWin::~OSWin()
{

}

bool OSWin::Initialize(const Options& options)
{
    m_hInstance = options.Win32.HInstance;
    m_messagePump = WindowsMessageCenter::Create();

    m_messagePump->RegisterReceiver(WM_QUIT, "WinEngine_QUIT", [&](const WindowsMessage& wm)
    {
        m_shouldQuit = true;
    });

    m_messagePump->RegisterReceiver(WM_CLOSE, "WinEngine_CLOSE", [&](const WindowsMessage& wm)
    {
        if (m_root == wm.hwnd)
        {
            m_shouldQuit = true;
        }
        else
        {
            m_windows.erase(wm.hwnd);
        }
       /* for (int i = (int)mWindows.size() - 1; i >= 0; --i)
        {
            if (mWindows[i].mHwnd == wm.hwnd)
            {
                mWindows.pop_back();
                if (i == 0)
                {
                    m_shouldQuit = true;
                }
            }
        }*/
        // Close window... if main window kill app
    });

    m_messagePump->RegisterReceiver(WM_DESTROY, "WinEngine_DESTROY", [](const WindowsMessage& wm)
    {
        PostQuitMessage(0);
    });

    MakeWindow(std::wstring(options.Caption.begin(), options.Caption.end()).c_str(), options.WindowWidth, options.WindowHeight, options.FullScreen);

    return true;
}

void OSWin::Update(double dt)
{
    m_messagePump->PeekMessages();
}

void OSWin::Shutdown()
{
    for (auto kvp : m_windows)
    {
        DestroyWindow(kvp.first);
    }

    m_windows.clear();

    WindowsMessageCenter::Destroy(m_messagePump);
}

std::shared_ptr<Window> OSWin::MakeWindow(LPCWSTR caption, int width, int height, bool fullscreen)
{
    HWND hwnd = MakeWindowInternal(m_hInstance, WinProc, L"BaseWindow", caption, width, height, fullscreen);
    std::shared_ptr<Window> window = std::make_shared<Window>(width, height, fullscreen);
    m_windows.insert({ hwnd, window });

    if (m_windows.size() == 1)
    {
        m_root = hwnd;
    }

    return window;
}

std::shared_ptr<Window> OSWin::GetWindow(HWND hwnd)
{
    auto iter = m_windows.find(hwnd);
    if (iter == m_windows.end())
    {
        return nullptr;
    }

    return iter->second;
}
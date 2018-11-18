#include "stdafx.h"
#include "WindowsMessageCenter.h"
#include "WindowsPlatformTypes.h"

using namespace Luz;

LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    Platform::s_messageBus.Notify({ hwnd, msg, wparam, lparam });

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

WindowsMessage::WindowsMessage()
{

}

WindowsMessage::WindowsMessage(HWND inH, UINT inM, WPARAM wp, LPARAM lp) :
    hwnd(inH), msg(inM), wparam(wp), lparam(lp)
{

}

WindowsMessageCenter::WindowsMessageCenter()
{

}

WindowsMessageCenter::~WindowsMessageCenter()
{
    m_events.clear();
}

void WindowsMessageCenter::PeekMessages()
{
    MSG msg;
    if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        // WM_QUIT will never be received via WindowsProc function.
        // Check deliberately here.
        if (msg.message == WM_QUIT) 
        {
            Notify({ 0, msg.message, 0, 0 });
        }
    }
}

WindowsEvent* WindowsMessageCenter::GetEvent(const UINT& msg)
{
    return FindOrCreateEvent(msg);
}

WindowsEvent* WindowsMessageCenter::FindOrCreateEvent(const UINT& msg)
{
    WindowsEvent* result = FindEvent(msg);
    if (!result)
    {
        m_events.emplace(msg, std::move(WindowsEvent()));
        result = &m_events[msg];
    }

    return result;
}

WindowsEvent* WindowsMessageCenter::FindEvent(const UINT& msg)
{
    WindowsEvent* result = nullptr;

    auto iter = m_events.find(msg);
    if (iter != m_events.end())
    {
        result = &iter->second;
    }

    return result;
}

void WindowsMessageCenter::Notify(const WindowsMessage wm)
{
    WindowsEvent* windowsEvent = FindEvent(wm.msg);
    if (windowsEvent)
    {
        windowsEvent->Broadcast(wm);
    }
}
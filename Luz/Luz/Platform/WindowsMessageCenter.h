#pragma once
#ifndef WINDOWSMESSAGECENTER_H
#define WINDOWSMESSAGECENTER_H

#include <Windows.h>

#ifndef DELEGATE_H
#include "Delegate.h"
#endif

LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

struct WindowsMessage
{
    HWND	hwnd;
    UINT	msg;
    WPARAM	wparam;
    LPARAM	lparam;
};

__interface IWindowsMessageCenterCallback
{
    virtual void HandleWindowsMessage(const WindowsMessage& wm) = 0;
};

typedef Luz::Event<void(const WindowsMessage&)> WindowsEvent;

class WindowsMessageCenter
{
public:
    WindowsMessageCenter();
    
    ~WindowsMessageCenter();
   
    void PeekMessages();

    void Notify(const WindowsMessage wm);
    
    WindowsEvent* GetEvent(const UINT& msg);

private:
    std::unordered_map<UINT, WindowsEvent> m_events;

    WindowsEvent* FindOrCreateEvent(const UINT& msg);
    WindowsEvent* FindEvent(const UINT& msg);
};

#endif
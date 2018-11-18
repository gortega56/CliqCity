#pragma once
#ifndef WINDOWSMESSAGECENTER_H
#define WINDOWSMESSAGECENTER_H

#include <Windows.h>
#include "Platform.h" // TODO: Extract Notification from Platform

LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

struct WindowsMessage : public Luz::Notification
{
    HWND	hwnd;
    UINT	msg;
    WPARAM	wparam;
    LPARAM	lparam;

    WindowsMessage();
    WindowsMessage(HWND, UINT, WPARAM, LPARAM);

    DEFAULT_COPY(WindowsMessage)
    DEFAULT_MOVE(WindowsMessage)
};

__interface IWindowsMessageCenterCallback
{
    virtual void HandleWindowsMessage(const WindowsMessage& wm) = 0;
};

typedef Luz::Event<void(const Luz::Notification&)> WindowsEvent;

class WindowsMessageCenter
{
public:
    
    static std::shared_ptr<WindowsMessageCenter> Create();
    static void Destroy(std::shared_ptr<WindowsMessageCenter>& mc);

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
#pragma once
#ifndef WINDOWSMESSAGECENTER_H
#define WINDOWSMESSAGECENTER_H

#include <Windows.h>
#include <unordered_map>
#include <set>
#include <functional>

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

class WindowsMessageCenter
{
public:
    struct Receiver
    {
        std::string name;
        std::function<void(const WindowsMessage&)> func;
    };
    
    static std::shared_ptr<WindowsMessageCenter> Create();
    static void Destroy(std::shared_ptr<WindowsMessageCenter>& mc);

    WindowsMessageCenter();
    ~WindowsMessageCenter();
   
    void PeekMessages();
    void RegisterReceiver(const UINT& msg, std::string name, std::function<void(const WindowsMessage&)> func);
    void RemoveReceiver(const UINT& msg, std::string name);
    void NotifyReceivers(const WindowsMessage& wm);


private:
    typedef std::vector<Receiver> ReceiverCollection;
    std::unordered_map<UINT, ReceiverCollection> mReceivers;

};

#endif
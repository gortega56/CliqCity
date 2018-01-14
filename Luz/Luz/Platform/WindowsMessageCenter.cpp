#include "stdafx.h"
#include "WindowsMessageCenter.h"

using namespace Luz;

namespace
{
    std::vector<std::shared_ptr<WindowsMessageCenter>> gMessageCenters;
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    for (auto mc : gMessageCenters)
    {
        mc->NotifyReceivers({ hwnd, msg, wparam, lparam });
    }

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
    mReceivers.clear();
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
            NotifyReceivers({ 0, msg.message, 0, 0 });
        }
    }
}

void WindowsMessageCenter::RegisterReceiver(const UINT& msg, std::string name, std::function<void(const WindowsMessage&)> func)
{
    if (func == nullptr)
    {
        // TO DO: Throw exception
        return;
    }

    auto map_iter = mReceivers.find(msg);
    if (map_iter == mReceivers.end())
    {
        mReceivers.insert({ msg, std::vector<Receiver>() });
        
        auto& receivers = mReceivers[msg];        
        receivers.push_back({ name, func });
    }
    else
    {
        auto& receivers = map_iter->second;
        auto iter = std::find_if(receivers.begin(), receivers.end(), [name](const Receiver& r) { return r.name == name; });
        if (iter == receivers.end())
        {
            receivers.push_back({ name, func });
        }
        else
        {
            // Replace or throw exception?
        }
    }
}

void WindowsMessageCenter::RemoveReceiver(const UINT& msg, std::string name)
{

}

void WindowsMessageCenter::NotifyReceivers(const WindowsMessage& wm)
{
    auto& iter = mReceivers.find(wm.msg);
    if (iter == mReceivers.end())
    {
        return;
    }

    auto& receivers = iter->second;
    for (auto& r : receivers)
    {
        assert(r.func);
        r.func(wm);
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
        m_events.insert({ msg, WindowsEvent() });
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

std::shared_ptr<WindowsMessageCenter> WindowsMessageCenter::Create()
{
    gMessageCenters.emplace_back(std::make_shared<WindowsMessageCenter>());
    
    return gMessageCenters.back();
}

void WindowsMessageCenter::Destroy(std::shared_ptr<WindowsMessageCenter>& mc)
{
    auto iter = std::find_if(gMessageCenters.begin(), gMessageCenters.end(), [&](std::shared_ptr<WindowsMessageCenter> wm) { return mc == wm; });
    if (iter != gMessageCenters.end())
    {
        gMessageCenters.erase(iter);
    }

    mc.reset();
}
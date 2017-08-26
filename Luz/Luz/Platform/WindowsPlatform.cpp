#include "stdafx.h"
#include "WindowsPlatform.h"
#include "WindowsMessageCenter.h"

namespace MS
{
    Windows::Windows(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) :
        m_prevHinstance(hPrevInstance), m_lpCmdLine(lpCmdLine), m_nShowCmd(nShowCmd)
    {
        Windows::sm_hinstance = hInstance;
    };
    
    Windows::~Windows()
    {

    };

    bool Windows::Initialize()
    {
        m_messagePump = WindowsMessageCenter::Create();
        m_messagePump->RegisterReceiver(WM_QUIT, "WinEngine_QUIT", [&](const WindowsMessage& wm) { m_shouldQuit = true; });
        m_messagePump->RegisterReceiver(WM_DESTROY, "WinEngine_DESTROY", [](const WindowsMessage& wm) { PostQuitMessage(0); });
        m_messagePump->RegisterReceiver(WM_CLOSE, "WinEngine_CLOSE", [&](const WindowsMessage& wm)
        {
            // TODO: Figure out multiple windows
            m_shouldQuit = true;
        });

        return true;
    }

    void Windows::Update(double delta)
    {
        m_messagePump->PeekMessages();
    }

    void Windows::Shutdown()
    {
        WindowsMessageCenter::Destroy(m_messagePump);
    }

    HINSTANCE Windows::GetHInstance() 
    { 
        return sm_hinstance; 
    }
}
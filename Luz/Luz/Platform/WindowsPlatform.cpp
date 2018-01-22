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
        std::weak_ptr<Windows> weak = shared_from_this();
        auto quit = [weak](const Luz::Notification& wm)
        {
            if (auto shared = weak.lock())
            {
                shared->m_shouldQuit = true;
            }
        };

        m_messagePump = WindowsMessageCenter::Create();
        m_messagePump->GetEvent(WM_QUIT)->Bind(quit);
        m_messagePump->GetEvent(WM_CLOSE)->Bind(quit);
        m_messagePump->GetEvent(WM_DESTROY)->Bind([](const Luz::Notification& wm) { PostQuitMessage(0); });

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
#pragma once
#ifndef WINDOWSPLATFORM_H
#define WINDOWSPLATFORM_H

#ifndef PLATFORM_H
#include "Platform.h"
#endif // !PLATFORM_H

class WindowsMessageCenter;

namespace MS
{
    class Windows : public Platform, public std::enable_shared_from_this<Windows>
    {
    public:
        Windows(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);
        ~Windows();

        bool Initialize() override;
        void BeginUpdate(double delta) override;
        void EndUpdate(double delta) override;
        void Shutdown() override;

        static HINSTANCE GetHInstance();

        std::shared_ptr<WindowsMessageCenter> GetMessageCenter() { return m_messagePump; }

    private:
        static HINSTANCE sm_hinstance;
        HINSTANCE m_prevHinstance;
        LPSTR m_lpCmdLine;
        int m_nShowCmd;

        std::shared_ptr<WindowsMessageCenter> m_messagePump;

        NO_COPY(Windows)
        NO_MOVE(Windows)
    };
}

#endif // !WINDOWSPLATFORM_H


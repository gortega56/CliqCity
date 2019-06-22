#include "stdafx.h"
#include "PlatformWindow.h"
#include "Platform.h"

#ifdef WINXX

#include "PlatformTypes_Win64.h"

namespace Platform
{
	extern const unsigned int s_maxWindows = 2;

	Window_Win64 s_pWindows[s_maxWindows];

	static std::atomic_int8_t s_nWindows = ATOMIC_VAR_INIT(0);

	WindowHandle MakeWindow(const char* pCaption, unsigned int width, unsigned int height, bool bFullScreen)
	{
		WNDCLASSEX wc;

		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WinProc;
		wc.cbClsExtra = NULL;
		wc.cbWndExtra = NULL;
		wc.hInstance = s_hInstance;
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = L"PlatformWindow";
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

		if (bFullScreen)
		{
			HWND hwnd = { 0 };
			HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
			MONITORINFO mi = { sizeof(mi) };
			GetMonitorInfo(hmon, &mi);

			width = mi.rcMonitor.right - mi.rcMonitor.left;
			height = mi.rcMonitor.bottom - mi.rcMonitor.top;
			wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		}
		else
		{
			RECT R = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
			AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
			width = R.right - R.left;
			height = R.bottom - R.top;
		}

		if (!RegisterClassEx(&wc))
		{
			MessageBox(nullptr, L"RegisterClass Failed", nullptr, 0);
		}

		wchar_t caption[MAX_PATH];
		MultiByteToWideChar(CP_UTF8, 0, pCaption, -1, caption, MAX_PATH);

		HWND hwnd = CreateWindowEx(NULL,
			L"PlatformWindow",
			caption,
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
			GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2,
			GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2,
			width, height,
			NULL,
			NULL,
			s_hInstance,
			NULL);

		if (!hwnd)
		{
			MessageBox(NULL, L"Error creating window",
				L"Error", MB_OK | MB_ICONERROR);
			return -1;
		}

		ShowWindow(hwnd, SW_SHOW);
		UpdateWindow(hwnd);

		int8_t handle = s_nWindows.fetch_add(1);

		Window_Win64& window = s_pWindows[handle];
		window.Width = width;
		window.Height = height;
		window.Aspect = static_cast<float>(width) / static_cast<float>(height);
		window.bFullScreen = bFullScreen;
		window.hwnd = hwnd;

		return handle;
	}

	void FreeWindow(const WindowHandle handle)
	{
		if (s_pWindows[handle].hwnd) DestroyWindow(s_pWindows[handle].hwnd);
	}

	bool GetWindow(const WindowHandle handle, Window& window)
	{
		if (0 <= handle && handle < s_nWindows.load())
		{
			Window_Win64* pWindow = &s_pWindows[handle];
			window.Width = pWindow->Width;
			window.Height = pWindow->Height;
			window.Aspect = pWindow->Aspect;
			window.bFullScreen = pWindow->bFullScreen;
			return true;
		}

		return false;
	}

    bool GetDisplay(const WindowHandle handle, Window& window)
    {
        if (0 <= handle && handle < s_nWindows.load())
        {            
            RECT rect;
            bool success = ::GetClientRect(s_pWindows[handle].hwnd, &rect);
            LUZASSERT(success);

            window.Width = rect.right - rect.left;
            window.Height = rect.bottom - rect.top;
            window.Aspect = static_cast<float>(window.Width) / static_cast<float>(window.Height);
            window.bFullScreen = s_pWindows[handle].bFullScreen;
            return true;
        }

        return false;
    }

    void GetWindow(const WindowHandle handle, Rect& rect)
    {
        LUZASSERT(0 <= handle && handle < s_nWindows.load());

        Window_Win64* pWindow = &s_pWindows[handle];
        rect.X = 0;
        rect.Y = 0;
        rect.Width = pWindow->Width;
        rect.Height = pWindow->Height;
    }

    void GetDisplay(const WindowHandle handle, Rect& rect)
    {
        LUZASSERT(0 <= handle && handle < s_nWindows.load());

        RECT r;
        bool success = ::GetClientRect(s_pWindows[handle].hwnd, &r);
        LUZASSERT(success);

        rect.X = static_cast<unsigned short>(r.left);
        rect.Y = static_cast<unsigned short>(r.bottom);
        rect.Width = static_cast<unsigned short>(r.right - r.left);
        rect.Height = static_cast<unsigned short>(r.bottom - r.top);
    }

	void Shutdown_Windows()
	{
		for (unsigned int i = 0, n = s_nWindows.load(); i < n; ++i)
		{
			if (s_pWindows[i].hwnd) DestroyWindow(s_pWindows[i].hwnd);
		}
	}
}

#endif
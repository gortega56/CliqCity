#include "stdafx.h"
#include "windows_utils.h"

HWND MakeWindowInternal(HINSTANCE hInstance, WNDPROC winProc, LPCWSTR className, LPCWSTR caption, int width, int height, bool fullscreen)
{
    WNDCLASSEX wc;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = winProc;
    wc.cbClsExtra = NULL;
    wc.cbWndExtra = NULL;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = className;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (fullscreen)
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
        RECT R = { 0, 0, width, height };
        AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
        width = R.right - R.left;
        height = R.bottom - R.top;
    }

    if (!RegisterClassEx(&wc))
    {
        MessageBox(nullptr, L"RegisterClass Failed", nullptr, 0);
    }

    HWND hwnd = CreateWindowEx(NULL,
        className,
        caption,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2,
        GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2,
        width, height,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (!hwnd)
    {
        MessageBox(NULL, L"Error creating window",
            L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    return hwnd;
}

#include "stdafx.h"
#include "Platform.h"

#ifdef WINXX
#include "WindowsPlatformTypes.h"
#include "Input.h"

namespace Platform
{    
    static Input s_input = Input();

    int Initialize(int n, ...)
    {
        va_list args;
        va_start(args, n);
        s_hInstance = va_arg(args, HINSTANCE);
        s_hPrevInstance = va_arg(args, HINSTANCE);
        s_lpCmdLine = va_arg(args, LPSTR);
        s_nShowCmd = va_arg(args, int);
        va_end(args);

        auto quit = [](const WindowsMessage& wm)
        {
            s_quit.store(true);
        };

        s_messageBus.GetEvent(WM_QUIT)->Bind(quit);
        s_messageBus.GetEvent(WM_CLOSE)->Bind(quit);
        s_messageBus.GetEvent(WM_DESTROY)->Bind([](const WindowsMessage& wm) { PostQuitMessage(0); });
    
        int success = s_input.Initialize();
        LUZASSERT(success);

        return success;
    }

    void Shutdown()
    {

    }

    void BeginUpdate(double time, double delta)
    {
        s_input.Update(delta);
        s_messageBus.PeekMessages();
    }

    void EndUpdate(double time, double delta)
    {

    }

    bool Running()
    {
        return !s_quit.load();
    }

    std::shared_ptr<::Window> CreateWindow0(std::string caption, i32 width, i32 height, bool fullscreen)
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
        wc.lpszClassName = L"BaseWindow";
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
            L"BaseWindow",
            std::wstring(caption.begin(), caption.end()).c_str(),
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
            return false;
        }

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);

        return std::make_shared<Window>(hwnd, width, height, fullscreen);
    }

    int CreateConsole()
    {
        BOOL success = AllocConsole();
        if (success)
        {
            freopen_s(&s_console.Cin, "CONIN$", "r", stdin);
            freopen_s(&s_console.Cout, "CONOUT$", "w", stdout);
            freopen_s(&s_console.Cerr, "CONOUT$", "w", stderr);

            std::wcin.clear();
            std::wcout.clear();
            std::wcerr.clear();

            std::cin.clear();
            std::cout.clear();
            std::cerr.clear();
        }

        return (success == 1);
    }

    int ClearConsole()
    {
        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hStdOut == INVALID_HANDLE_VALUE) return 0;

        /* Get the number of cells in the current buffer */
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return 0;
        DWORD cellCount = csbi.dwSize.X *csbi.dwSize.Y;

        /* Fill the entire buffer with spaces */
        DWORD count;
        COORD homeCoords = { 0, 0 };
        if (!FillConsoleOutputCharacter(
            hStdOut,
            (TCHAR) ' ',
            cellCount,
            homeCoords,
            &count
        )) return 0;

        /* Fill the entire buffer with the current colors and attributes */
        if (!FillConsoleOutputAttribute(
            hStdOut,
            csbi.wAttributes,
            cellCount,
            homeCoords,
            &count
        )) return 0;

        /* Move the cursor home */
        SetConsoleCursorPosition(hStdOut, homeCoords);

        return 1;
    }

    void DestroyConsole()
    {
        if (s_console.Cin) {  fclose(s_console.Cin); }
        if (s_console.Cout) { fclose(s_console.Cout); }
        if (s_console.Cerr) { fclose(s_console.Cerr); }
        FreeConsole();
    }

    ScreenPoint GetMousePosition()
    {
        return s_input.mousePosition;
    }

    bool IsMouseActive()
    {
        return s_input.IsMouseActive();
    }

    void SetMouseActive(bool active)
    {
        s_input.SetMouseActive(active);
    }

    bool GetKeyDown(KeyCode key)
    {
        return s_input.GetKeyDown(key);
    }

    bool GetKeyUp(KeyCode key)
    {
        return s_input.GetKeyUp(key);
    }

    bool GetKey(KeyCode key)
    {
        return s_input.GetKey(key);
    }

    bool GetMouseButtonDown(MouseButton button)
    {
        return s_input.GetMouseButtonDown(button);
    }

    bool GetMouseButtonUp(MouseButton button)
    {
        return s_input.GetMouseButtonUp(button);
    }

    bool GetMouseButton(MouseButton button)
    {
        return s_input.GetMouseButton(button);
    }

    bool GetGamepadButtonDown(GamepadButton buttonMask, bool isAdditive, short id)
    {
        return s_input.GetGamepadButtonDown(buttonMask, isAdditive, id);
    }

    bool GetGamepadButtonUp(GamepadButton buttonMask, bool isAdditive, short id)
    {
        return s_input.GetGamepadButtonUp(buttonMask, isAdditive, id);
    }

    bool GetGamepadButton(GamepadButton buttonMask, bool isAdditive, short id)
    {
        return s_input.GetGamepadButton(buttonMask, isAdditive, id);
    }

    float GetGamepadAxis(GamepadAxis axis, short id)
    {
        return s_input.GetGamepadAxis(axis, id);
    }
}

#endif
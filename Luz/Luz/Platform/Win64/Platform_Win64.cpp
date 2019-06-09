#include "stdafx.h"
#include "Platform.h"

#ifdef WINXX
#include "PlatformTypes_Win64.h"
#include "PlatformEvents.h"
#include "PlatformInput.h"

namespace Platform
{    
    int Initialize(int n, ...)
    {
        va_list args;
        va_start(args, n);
        s_hInstance = va_arg(args, HINSTANCE);
        s_hPrevInstance = va_arg(args, HINSTANCE);
        s_lpCmdLine = va_arg(args, LPSTR);
        s_nShowCmd = va_arg(args, int);
        va_end(args);

		Initialize_Events();

		Initialize_Input();

		auto quit = [](const Message& wm)
        {
            s_quit.store(true);
        };

        GetEvent(PLATFORM_EVENT_TYPE_WINDOW_QUIT)->Bind(quit);
        GetEvent(PLATFORM_EVENT_TYPE_WINDOW_CLOSE)->Bind(quit);
        GetEvent(PLATFORM_EVENT_TYPE_WINDOW_DESTROY)->Bind([](const Message& wm) { PostQuitMessage(0); });

        return 1;
    }

    void Shutdown()
    {
        Shutdown_Windows();
    }

    void BeginUpdate(double time, double delta)
    {
		// Route os messages at the beginning of the frame
		Update_Events(delta);
	}

    void EndUpdate(double time, double delta)
	{
		Update_Input(delta);
    }

    bool Running()
    {
        return !s_quit.load();
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
}

#endif
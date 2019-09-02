#pragma once
#ifndef PLATFORMTYPES_WIN64_H
#define PLATFORMTYPES_WIN64_H

#pragma region Platform Independent

#ifndef PLATFORM_H
#include "Platform.h"
#endif

#ifndef PLATFORMWINDOW_H
#include "PlatformWindow.h"
#endif
#pragma endregion

#pragma region Platform Dependent

#include <Windows.h>
#include <wrl.h>
#include <Windowsx.h>
#include <xinput.h>

#pragma endregion

namespace Platform
{
	LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

    extern HINSTANCE s_hInstance;

    extern HINSTANCE s_hPrevInstance;

    extern LPSTR s_lpCmdLine;

    extern int s_nShowCmd;

    extern std::atomic_bool s_quit;

	struct Window_Win64 : public Window
	{
		HWND hwnd;
	};

	extern Window_Win64 s_pWindows[];

    struct Console
    {
        FILE* Cin;
        FILE* Cout;
        FILE* Cerr;
    };

    extern Console s_console;

	UINT GetKeyCode(const KeyType eKey);

	KeyType GetKeyType(const UINT code);
}

#endif
#include "stdafx.h"
#include "PlatformEvents.h"
#include "Platform.h"

#ifdef WINXX

#include "Platform/Win64/PlatformTypes_Win64.h"

#define PREV_KEY_STATE_BIT 0x40000000

using namespace Luz;

namespace Platform
{
	static Event s_pEvents[PLATFORM_EVENT_TYPE_COUNT];

	static UINT GetEventCode(const EventType eEvent)
	{
		switch (eEvent)
		{
			// Window events
		case(PLATFORM_EVENT_TYPE_WINDOW_ACTIVATEAPP): return WM_ACTIVATEAPP;
		case(PLATFORM_EVENT_TYPE_WINDOW_CANCELMODE): return WM_CANCELMODE;
		case(PLATFORM_EVENT_TYPE_WINDOW_CHILDACTIVATE): return WM_CHILDACTIVATE;
		case(PLATFORM_EVENT_TYPE_WINDOW_CLOSE): return WM_CLOSE;
		case(PLATFORM_EVENT_TYPE_WINDOW_COMPACTING): return WM_COMPACTING;
		case(PLATFORM_EVENT_TYPE_WINDOW_CREATE): return WM_CREATE;
		case(PLATFORM_EVENT_TYPE_WINDOW_DESTROY): return WM_DESTROY;
		case(PLATFORM_EVENT_TYPE_WINDOW_DPICHANGED): return WM_DPICHANGED;
		case(PLATFORM_EVENT_TYPE_WINDOW_ENABLE): return WM_ENABLE;
		case(PLATFORM_EVENT_TYPE_WINDOW_ENTERSIZEMOVE): return WM_ENTERSIZEMOVE;
		case(PLATFORM_EVENT_TYPE_WINDOW_EXITSIZEMOVE): return WM_EXITSIZEMOVE;
		case(PLATFORM_EVENT_TYPE_WINDOW_GETICON): return WM_GETICON;
		case(PLATFORM_EVENT_TYPE_WINDOW_GETMINMAXINFO): return WM_GETMINMAXINFO;
		case(PLATFORM_EVENT_TYPE_WINDOW_INPUTLANGCHANGE): return WM_INPUTLANGCHANGE;
		case(PLATFORM_EVENT_TYPE_WINDOW_INPUTLANGCHANGEREQUEST): return WM_INPUTLANGCHANGEREQUEST;
		case(PLATFORM_EVENT_TYPE_WINDOW_MOVE): return WM_MOVE;
		case(PLATFORM_EVENT_TYPE_WINDOW_MOVING): return WM_MOVING;
		case(PLATFORM_EVENT_TYPE_WINDOW_NCACTIVATE): return WM_NCACTIVATE;
		case(PLATFORM_EVENT_TYPE_WINDOW_NCCALCSIZE): return WM_NCCALCSIZE;
		case(PLATFORM_EVENT_TYPE_WINDOW_NCCREATE): return WM_NCCREATE;
		case(PLATFORM_EVENT_TYPE_WINDOW_NCDESTROY): return WM_NCDESTROY;
		case(PLATFORM_EVENT_TYPE_WINDOW_NULL): return WM_NULL;
		case(PLATFORM_EVENT_TYPE_WINDOW_QUERYDRAGICON): return WM_QUERYDRAGICON;
		case(PLATFORM_EVENT_TYPE_WINDOW_QUERYOPEN): return WM_QUERYOPEN;
		case(PLATFORM_EVENT_TYPE_WINDOW_QUIT): return WM_QUIT;
		case(PLATFORM_EVENT_TYPE_WINDOW_SHOWWINDOW): return WM_SHOWWINDOW;
		case(PLATFORM_EVENT_TYPE_WINDOW_SIZE): return WM_SIZE;
		case(PLATFORM_EVENT_TYPE_WINDOW_SIZING): return WM_SIZING;
		case(PLATFORM_EVENT_TYPE_WINDOW_STYLECHANGED): return WM_STYLECHANGED;
		case(PLATFORM_EVENT_TYPE_WINDOW_STYLECHANGING): return WM_STYLECHANGING;
		case(PLATFORM_EVENT_TYPE_WINDOW_THEMECHANGED): return WM_THEMECHANGED;
		case(PLATFORM_EVENT_TYPE_WINDOW_USERCHANGED): return WM_USERCHANGED;
		case(PLATFORM_EVENT_TYPE_WINDOW_WINDOWPOSCHANGED): return WM_WINDOWPOSCHANGED;
		case(PLATFORM_EVENT_TYPE_WINDOW_WINDOWPOSCHANGING): return WM_WINDOWPOSCHANGING;
			// Keyboard events
		case(PLATFORM_EVENT_TYPE_KEYBOARD_ACTIVATE): return WM_ACTIVATE;
		case(PLATFORM_EVENT_TYPE_KEYBOARD_APPCOMMAND): return WM_APPCOMMAND;
		case(PLATFORM_EVENT_TYPE_KEYBOARD_CHAR): return WM_CHAR;
		case(PLATFORM_EVENT_TYPE_KEYBOARD_DEADCHAR): return WM_DEADCHAR;
		case(PLATFORM_EVENT_TYPE_KEYBOARD_HOTKEY): return WM_HOTKEY;
		case(PLATFORM_EVENT_TYPE_KEYBOARD_KEYDOWN): return WM_KEYDOWN;
		case(PLATFORM_EVENT_TYPE_KEYBOARD_KEYUP): return WM_KEYUP;
		case(PLATFORM_EVENT_TYPE_KEYBOARD_KILLFOCUS): return WM_KILLFOCUS;
		case(PLATFORM_EVENT_TYPE_KEYBOARD_SETFOCUS): return WM_SETFOCUS;
		case(PLATFORM_EVENT_TYPE_KEYBOARD_SYSDEADCHAR): return WM_SYSDEADCHAR;
		case(PLATFORM_EVENT_TYPE_KEYBOARD_SYSKEYDOWN): return WM_SYSKEYDOWN;
		case(PLATFORM_EVENT_TYPE_KEYBOARD_SYSKEYUP): return WM_SYSKEYUP;
		case(PLATFORM_EVENT_TYPE_KEYBOARD_UNICHAR): return WM_UNICHAR;
			// Mouse events												
		case(PLATFORM_EVENT_TYPE_MOUSE_CAPTURECHANGED): return WM_CAPTURECHANGED;
		case(PLATFORM_EVENT_TYPE_MOUSE_LBUTTONDBLCLK): return WM_LBUTTONDBLCLK;
		case(PLATFORM_EVENT_TYPE_MOUSE_LBUTTONDOWN): return WM_LBUTTONDOWN;
		case(PLATFORM_EVENT_TYPE_MOUSE_LBUTTONUP): return WM_LBUTTONUP;
		case(PLATFORM_EVENT_TYPE_MOUSE_MBUTTONDBLCLK): return WM_MBUTTONDBLCLK;
		case(PLATFORM_EVENT_TYPE_MOUSE_MBUTTONDOWN): return WM_MBUTTONDOWN;
		case(PLATFORM_EVENT_TYPE_MOUSE_MBUTTONUP): return WM_MBUTTONUP;
		case(PLATFORM_EVENT_TYPE_MOUSE_ACTIVATE): return WM_MOUSEACTIVATE;
		case(PLATFORM_EVENT_TYPE_MOUSE_HOVER): return WM_MOUSEHOVER;
		case(PLATFORM_EVENT_TYPE_MOUSE_HWHEEL): return WM_MOUSEHWHEEL;
		case(PLATFORM_EVENT_TYPE_MOUSE_LEAVE): return WM_MOUSELEAVE;
		case(PLATFORM_EVENT_TYPE_MOUSE_MOVE): return WM_MOUSEMOVE;
		case(PLATFORM_EVENT_TYPE_MOUSE_WHEEL): return WM_MOUSEWHEEL;
		case(PLATFORM_EVENT_TYPE_MOUSE_NCHITTEST): return WM_NCHITTEST;
		case(PLATFORM_EVENT_TYPE_MOUSE_NCLBUTTONDBLCLK): return WM_NCLBUTTONDBLCLK;
		case(PLATFORM_EVENT_TYPE_MOUSE_NCLBUTTONDOWN): return WM_NCLBUTTONDOWN;
		case(PLATFORM_EVENT_TYPE_MOUSE_NCLBUTTONUP): return WM_NCLBUTTONUP;
		case(PLATFORM_EVENT_TYPE_MOUSE_NCMBUTTONDBLCLK): return WM_NCMBUTTONDBLCLK;
		case(PLATFORM_EVENT_TYPE_MOUSE_NCMBUTTONDOWN): return WM_NCMBUTTONDOWN;
		case(PLATFORM_EVENT_TYPE_MOUSE_NCMBUTTONUP): return WM_NCMBUTTONUP;
		case(PLATFORM_EVENT_TYPE_MOUSE_NCHOVER): return WM_NCMOUSEHOVER;
		case(PLATFORM_EVENT_TYPE_MOUSE_NCLEAVE): return WM_NCMOUSELEAVE;
		case(PLATFORM_EVENT_TYPE_MOUSE_NCMOVE): return WM_NCMOUSEMOVE;
		case(PLATFORM_EVENT_TYPE_MOUSE_NCRBUTTONDBLCLK): return WM_NCRBUTTONDBLCLK;
		case(PLATFORM_EVENT_TYPE_MOUSE_NCRBUTTONDOWN): return WM_NCRBUTTONDOWN;
		case(PLATFORM_EVENT_TYPE_MOUSE_NCRBUTTONUP): return WM_NCRBUTTONUP;
		case(PLATFORM_EVENT_TYPE_MOUSE_NCXBUTTONDBLCLK): return WM_NCXBUTTONDBLCLK;
		case(PLATFORM_EVENT_TYPE_MOUSE_NCXBUTTONDOWN): return WM_NCXBUTTONDOWN;
		case(PLATFORM_EVENT_TYPE_MOUSE_NCXBUTTONUP): return WM_NCXBUTTONUP;
		case(PLATFORM_EVENT_TYPE_MOUSE_RBUTTONDBLCLK): return WM_RBUTTONDBLCLK;
		case(PLATFORM_EVENT_TYPE_MOUSE_RBUTTONDOWN): return WM_RBUTTONDOWN;
		case(PLATFORM_EVENT_TYPE_MOUSE_RBUTTONUP): return WM_RBUTTONUP;
		case(PLATFORM_EVENT_TYPE_MOUSE_XBUTTONDBLCLK): return WM_XBUTTONDBLCLK;
		case(PLATFORM_EVENT_TYPE_MOUSE_XBUTTONDOWN): return WM_XBUTTONDOWN;
		case(PLATFORM_EVENT_TYPE_MOUSE_XBUTTONUP): return WM_XBUTTONUP;
		default: return -1;
		}
	}

	static EventType GetEventType(UINT code)
	{
		switch (code)
		{
			// Window events
		case(WM_ACTIVATEAPP): return PLATFORM_EVENT_TYPE_WINDOW_ACTIVATEAPP;
		case(WM_CANCELMODE): return PLATFORM_EVENT_TYPE_WINDOW_CANCELMODE;
		case(WM_CHILDACTIVATE): return PLATFORM_EVENT_TYPE_WINDOW_CHILDACTIVATE;
		case(WM_CLOSE): return PLATFORM_EVENT_TYPE_WINDOW_CLOSE;
		case(WM_COMPACTING): return PLATFORM_EVENT_TYPE_WINDOW_COMPACTING;
		case(WM_CREATE): return PLATFORM_EVENT_TYPE_WINDOW_CREATE;
		case(WM_DESTROY): return PLATFORM_EVENT_TYPE_WINDOW_DESTROY;
		case(WM_DPICHANGED): return PLATFORM_EVENT_TYPE_WINDOW_DPICHANGED;
		case(WM_ENABLE): return PLATFORM_EVENT_TYPE_WINDOW_ENABLE;
		case(WM_ENTERSIZEMOVE): return PLATFORM_EVENT_TYPE_WINDOW_ENTERSIZEMOVE;
		case(WM_EXITSIZEMOVE): return PLATFORM_EVENT_TYPE_WINDOW_EXITSIZEMOVE;
		case(WM_GETICON): return PLATFORM_EVENT_TYPE_WINDOW_GETICON;
		case(WM_GETMINMAXINFO): return PLATFORM_EVENT_TYPE_WINDOW_GETMINMAXINFO;
		case(WM_INPUTLANGCHANGE): return PLATFORM_EVENT_TYPE_WINDOW_INPUTLANGCHANGE;
		case(WM_INPUTLANGCHANGEREQUEST): return PLATFORM_EVENT_TYPE_WINDOW_INPUTLANGCHANGEREQUEST;
		case(WM_MOVE): return PLATFORM_EVENT_TYPE_WINDOW_MOVE;
		case(WM_MOVING): return PLATFORM_EVENT_TYPE_WINDOW_MOVING;
		case(WM_NCACTIVATE): return PLATFORM_EVENT_TYPE_WINDOW_NCACTIVATE;
		case(WM_NCCALCSIZE): return PLATFORM_EVENT_TYPE_WINDOW_NCCALCSIZE;
		case(WM_NCCREATE): return PLATFORM_EVENT_TYPE_WINDOW_NCCREATE;
		case(WM_NCDESTROY): return PLATFORM_EVENT_TYPE_WINDOW_NCDESTROY;
		case(WM_NULL): return PLATFORM_EVENT_TYPE_WINDOW_NULL;
		case(WM_QUERYDRAGICON): return PLATFORM_EVENT_TYPE_WINDOW_QUERYDRAGICON;
		case(WM_QUERYOPEN): return PLATFORM_EVENT_TYPE_WINDOW_QUERYOPEN;
		case(WM_QUIT): return PLATFORM_EVENT_TYPE_WINDOW_QUIT;
		case(WM_SHOWWINDOW): return PLATFORM_EVENT_TYPE_WINDOW_SHOWWINDOW;
		case(WM_SIZE): return PLATFORM_EVENT_TYPE_WINDOW_SIZE;
		case(WM_SIZING): return PLATFORM_EVENT_TYPE_WINDOW_SIZING;
		case(WM_STYLECHANGED): return PLATFORM_EVENT_TYPE_WINDOW_STYLECHANGED;
		case(WM_STYLECHANGING): return PLATFORM_EVENT_TYPE_WINDOW_STYLECHANGING;
		case(WM_THEMECHANGED): return PLATFORM_EVENT_TYPE_WINDOW_THEMECHANGED;
		case(WM_USERCHANGED): return PLATFORM_EVENT_TYPE_WINDOW_USERCHANGED;
		case(WM_WINDOWPOSCHANGED): return PLATFORM_EVENT_TYPE_WINDOW_WINDOWPOSCHANGED;
		case(WM_WINDOWPOSCHANGING): return PLATFORM_EVENT_TYPE_WINDOW_WINDOWPOSCHANGING;
			// Keyboard events
		case(WM_ACTIVATE): return PLATFORM_EVENT_TYPE_KEYBOARD_ACTIVATE;
		case(WM_APPCOMMAND): return PLATFORM_EVENT_TYPE_KEYBOARD_APPCOMMAND;
		case(WM_CHAR): return PLATFORM_EVENT_TYPE_KEYBOARD_CHAR;
		case(WM_DEADCHAR): return PLATFORM_EVENT_TYPE_KEYBOARD_DEADCHAR;
		case(WM_HOTKEY): return PLATFORM_EVENT_TYPE_KEYBOARD_HOTKEY;
		case(WM_KEYDOWN): return PLATFORM_EVENT_TYPE_KEYBOARD_KEYDOWN;
		case(WM_KEYUP): return PLATFORM_EVENT_TYPE_KEYBOARD_KEYUP;
		case(WM_KILLFOCUS): return PLATFORM_EVENT_TYPE_KEYBOARD_KILLFOCUS;
		case(WM_SETFOCUS): return PLATFORM_EVENT_TYPE_KEYBOARD_SETFOCUS;
		case(WM_SYSDEADCHAR): return PLATFORM_EVENT_TYPE_KEYBOARD_SYSDEADCHAR;
		case(WM_SYSKEYDOWN): return PLATFORM_EVENT_TYPE_KEYBOARD_SYSKEYDOWN;
		case(WM_SYSKEYUP): return PLATFORM_EVENT_TYPE_KEYBOARD_SYSKEYUP;
		case(WM_UNICHAR): return PLATFORM_EVENT_TYPE_KEYBOARD_UNICHAR;
			// Mouse events												
		case(WM_CAPTURECHANGED): return PLATFORM_EVENT_TYPE_MOUSE_CAPTURECHANGED;
		case(WM_LBUTTONDBLCLK): return PLATFORM_EVENT_TYPE_MOUSE_LBUTTONDBLCLK;
		case(WM_LBUTTONDOWN): return PLATFORM_EVENT_TYPE_MOUSE_LBUTTONDOWN;
		case(WM_LBUTTONUP): return PLATFORM_EVENT_TYPE_MOUSE_LBUTTONUP;
		case(WM_MBUTTONDBLCLK): return PLATFORM_EVENT_TYPE_MOUSE_MBUTTONDBLCLK;
		case(WM_MBUTTONDOWN): return PLATFORM_EVENT_TYPE_MOUSE_MBUTTONDOWN;
		case(WM_MBUTTONUP): return PLATFORM_EVENT_TYPE_MOUSE_MBUTTONUP;
		case(WM_MOUSEACTIVATE): return PLATFORM_EVENT_TYPE_MOUSE_ACTIVATE;
		case(WM_MOUSEHOVER): return PLATFORM_EVENT_TYPE_MOUSE_HOVER;
		case(WM_MOUSEHWHEEL): return PLATFORM_EVENT_TYPE_MOUSE_HWHEEL;
		case(WM_MOUSELEAVE): return PLATFORM_EVENT_TYPE_MOUSE_LEAVE;
		case(WM_MOUSEMOVE): return PLATFORM_EVENT_TYPE_MOUSE_MOVE;
		case(WM_MOUSEWHEEL): return PLATFORM_EVENT_TYPE_MOUSE_WHEEL;
		case(WM_NCHITTEST): return PLATFORM_EVENT_TYPE_MOUSE_NCHITTEST;
		case(WM_NCLBUTTONDBLCLK): return PLATFORM_EVENT_TYPE_MOUSE_NCLBUTTONDBLCLK;
		case(WM_NCLBUTTONDOWN): return PLATFORM_EVENT_TYPE_MOUSE_NCLBUTTONDOWN;
		case(WM_NCLBUTTONUP): return PLATFORM_EVENT_TYPE_MOUSE_NCLBUTTONUP;
		case(WM_NCMBUTTONDBLCLK): return PLATFORM_EVENT_TYPE_MOUSE_NCMBUTTONDBLCLK;
		case(WM_NCMBUTTONDOWN): return PLATFORM_EVENT_TYPE_MOUSE_NCMBUTTONDOWN;
		case(WM_NCMBUTTONUP): return PLATFORM_EVENT_TYPE_MOUSE_NCMBUTTONUP;
		case(WM_NCMOUSEHOVER): return PLATFORM_EVENT_TYPE_MOUSE_NCHOVER;
		case(WM_NCMOUSELEAVE): return PLATFORM_EVENT_TYPE_MOUSE_NCLEAVE;
		case(WM_NCMOUSEMOVE): return PLATFORM_EVENT_TYPE_MOUSE_NCMOVE;
		case(WM_NCRBUTTONDBLCLK): return PLATFORM_EVENT_TYPE_MOUSE_NCRBUTTONDBLCLK;
		case(WM_NCRBUTTONDOWN): return PLATFORM_EVENT_TYPE_MOUSE_NCRBUTTONDOWN;
		case(WM_NCRBUTTONUP): return PLATFORM_EVENT_TYPE_MOUSE_NCRBUTTONUP;
		case(WM_NCXBUTTONDBLCLK): return PLATFORM_EVENT_TYPE_MOUSE_NCXBUTTONDBLCLK;
		case(WM_NCXBUTTONDOWN): return PLATFORM_EVENT_TYPE_MOUSE_NCXBUTTONDOWN;
		case(WM_NCXBUTTONUP): return PLATFORM_EVENT_TYPE_MOUSE_NCXBUTTONUP;
		case(WM_RBUTTONDBLCLK): return PLATFORM_EVENT_TYPE_MOUSE_RBUTTONDBLCLK;
		case(WM_RBUTTONDOWN): return PLATFORM_EVENT_TYPE_MOUSE_RBUTTONDOWN;
		case(WM_RBUTTONUP): return PLATFORM_EVENT_TYPE_MOUSE_RBUTTONUP;
		case(WM_XBUTTONDBLCLK): return PLATFORM_EVENT_TYPE_MOUSE_XBUTTONDBLCLK;
		case(WM_XBUTTONDOWN): return PLATFORM_EVENT_TYPE_MOUSE_XBUTTONDOWN;
		case(WM_XBUTTONUP): return PLATFORM_EVENT_TYPE_MOUSE_XBUTTONUP;
		default: return PLATFORM_EVENT_TYPE_UNKNOWN;
		}
	}

	typedef void (*InitializeMessage)(Message*, ::HWND, UINT, WPARAM, LPARAM);

	static InitializeMessage s_pMessageFuncTable[PLATFORM_EVENT_TYPE_COUNT];

	static void CreateWindowSizeMessage(Message* pMessage, HWND hwnd, UINT code, WPARAM wparam, LPARAM lparam)
	{
		if (code == WM_SIZE)
		{
			pMessage->WindowSize.Width = LOWORD(lparam);
			pMessage->WindowSize.Height = HIWORD(lparam);
			pMessage->WindowSize.bMaximized = (wparam == SIZE_MAXIMIZED);
			pMessage->WindowSize.bMinimized = (wparam == SIZE_MINIMIZED);
			pMessage->WindowSize.bRestored = (wparam == SIZE_RESTORED);
			pMessage->WindowSize.bDragEnter = 0;
			pMessage->WindowSize.bDragExit = 0;
		}
		else if (code == WM_ENTERSIZEMOVE)
		{
			pMessage->WindowSize.Width = LOWORD(lparam);
			pMessage->WindowSize.Height = HIWORD(lparam);
			pMessage->WindowSize.bMaximized = 0;
			pMessage->WindowSize.bMinimized = 0;
			pMessage->WindowSize.bRestored = 0;
			pMessage->WindowSize.bDragEnter = 1;
			pMessage->WindowSize.bDragExit = 0;
		}
		else if (code == WM_EXITSIZEMOVE)
		{
			pMessage->WindowSize.Width = LOWORD(lparam);
			pMessage->WindowSize.Height = HIWORD(lparam);
			pMessage->WindowSize.bMaximized = 0;
			pMessage->WindowSize.bMinimized = 0;
			pMessage->WindowSize.bRestored = 0;
			pMessage->WindowSize.bDragEnter = 0;
			pMessage->WindowSize.bDragExit = 1;
		}
		else
		{
			LUZASSERT(0);
		}
	}

	static void CreateKeyboardMessage(Message* pMessage, HWND hwnd, UINT code, WPARAM wparam, LPARAM lparam)
	{
		UINT keyCode = static_cast<UINT>(wparam);

		if (code == WM_KEYUP || code == WM_SYSKEYUP)
		{
			pMessage->Keyboard.eKey = GetKeyType(keyCode);
			pMessage->Keyboard.bRecurrent = 0;
		}
		else if (code == WM_KEYDOWN || code == WM_SYSKEYDOWN)
		{
			pMessage->Keyboard.eKey = GetKeyType(keyCode);
			pMessage->Keyboard.bRecurrent = (lparam & PREV_KEY_STATE_BIT);
		}
		else
		{
			LUZASSERT(0);
		}
	}

	static void CreateMouseClickMessage(Message* pMessage, HWND hwnd, UINT code, WPARAM wparam, LPARAM lparam)
	{
		pMessage->MouseClick.Code = GET_XBUTTON_WPARAM(wparam);
	}

	static void CreateMouseMoveMessage(Message* pMessage, HWND hwnd, UINT code, WPARAM wparam, LPARAM lparam)
	{
		pMessage->MouseMove.PositionX = GET_X_LPARAM(lparam);
		pMessage->MouseMove.PositionY = GET_Y_LPARAM(lparam);
	}

	static void CreateMouseWheelMessage(Message* pMessage, HWND hwnd, UINT code, WPARAM wparam, LPARAM lparam)
	{
		pMessage->MouseWheel.Delta = static_cast<float>GET_WHEEL_DELTA_WPARAM(wparam);
	}

	static void CreateUnimplementedMessage(Message* pMessage, HWND hwnd, UINT code, WPARAM wparam, LPARAM lparam)
	{

	}

	static void BroadcastMessage(HWND hwnd, UINT code, WPARAM wparam, LPARAM lparam)
	{
		EventType eEvent = GetEventType(code);
		
		if (eEvent == PLATFORM_EVENT_TYPE_UNKNOWN)
		{
			return;
		}

		Message message;

		s_pMessageFuncTable[eEvent](&message, hwnd, code, wparam, lparam);

		message.eEventType = eEvent;

		s_pEvents[eEvent].Broadcast(message);
	}

	Event* GetEvent(const EventType eEvent)
	{
		return &s_pEvents[eEvent];
	}

	void Initialize_Events()
	{
		for (int i = 0; i < PLATFORM_EVENT_TYPE_COUNT; ++i)
		{
			s_pMessageFuncTable[i] = CreateUnimplementedMessage;
		}

		s_pMessageFuncTable[GetEventType(WM_SIZE)] = CreateWindowSizeMessage;
		s_pMessageFuncTable[GetEventType(WM_ENTERSIZEMOVE)] = CreateWindowSizeMessage;
		s_pMessageFuncTable[GetEventType(WM_EXITSIZEMOVE)] = CreateWindowSizeMessage;

		s_pMessageFuncTable[GetEventType(WM_KEYUP)] = CreateKeyboardMessage;
		s_pMessageFuncTable[GetEventType(WM_KEYDOWN)] = CreateKeyboardMessage;
		s_pMessageFuncTable[GetEventType(WM_SYSKEYUP)] = CreateKeyboardMessage;
		s_pMessageFuncTable[GetEventType(WM_SYSKEYDOWN)] = CreateKeyboardMessage;

		s_pMessageFuncTable[GetEventType(WM_LBUTTONDOWN)] = CreateMouseClickMessage;
		s_pMessageFuncTable[GetEventType(WM_MBUTTONDOWN)] = CreateMouseClickMessage;
		s_pMessageFuncTable[GetEventType(WM_RBUTTONDOWN)] = CreateMouseClickMessage;
		s_pMessageFuncTable[GetEventType(WM_XBUTTONDOWN)] = CreateMouseClickMessage;
		s_pMessageFuncTable[GetEventType(WM_LBUTTONUP)] = CreateMouseClickMessage;
		s_pMessageFuncTable[GetEventType(WM_MBUTTONUP)] = CreateMouseClickMessage;
		s_pMessageFuncTable[GetEventType(WM_RBUTTONUP)] = CreateMouseClickMessage;
		s_pMessageFuncTable[GetEventType(WM_XBUTTONUP)] = CreateMouseClickMessage;

		s_pMessageFuncTable[GetEventType(WM_MOUSEMOVE)] = CreateMouseMoveMessage;
		s_pMessageFuncTable[GetEventType(WM_MOUSEWHEEL)] = CreateMouseWheelMessage;
	}

	void Update_Events(double delta)
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// WM_QUIT will never be received via WindowsProc function.
		// Check deliberately here.
		if (msg.message == WM_QUIT)
		{
			BroadcastMessage(msg.hwnd, msg.message, msg.wParam, msg.lParam);
		}
	}

    typedef LRESULT(*WinProcOverride)(HWND, UINT, WPARAM, LPARAM);
    static WinProcOverride s_pWinProcOverride = nullptr;

    void Register_EventFunction(void* pFunction)
    {
        s_pWinProcOverride = static_cast<WinProcOverride>(pFunction);
    }

	LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
        if (s_pWinProcOverride)
        {
           s_pWinProcOverride(hwnd, msg, wparam, lparam);
        }

		BroadcastMessage(hwnd, msg, wparam, lparam);

		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
}

#endif
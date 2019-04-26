#include <stdafx.h>
#include "WindowsPlatformTypes.h"

namespace Platform
{
    HINSTANCE s_hInstance = 0;

    HINSTANCE s_hPrevInstance = 0;

    LPSTR s_lpCmdLine = 0;

    int s_nShowCmd = 0;

    WindowsMessageCenter s_messageBus = WindowsMessageCenter();

    std::atomic_bool s_quit = ATOMIC_VAR_INIT(0);

    Window::Window(HWND hwnd, i32 width, i32 height, bool fullscreen) 
        : ::Window(width, height, fullscreen)
        , m_hwnd(hwnd)
    {

    }

    Window::~Window()
    {
        DestroyWindow(m_hwnd);
    };

    HWND Window::Handle() const
    {
        return m_hwnd;
    }

    Console s_console;

    UINT GetWindowsMessage(const NotificationType type)
    {
        switch (type)
        {
            case PLATFORM_NOTIFICATION_ACTIVATEAPP: return WM_ACTIVATEAPP;
            case PLATFORM_NOTIFICATION_CANCELMODE: return WM_CANCELMODE;
            case PLATFORM_NOTIFICATION_CHILDACTIVATE: return WM_CHILDACTIVATE;
            case PLATFORM_NOTIFICATION_CLOSE: return WM_CLOSE;
            case PLATFORM_NOTIFICATION_COMPACTING: return WM_COMPACTING;
            case PLATFORM_NOTIFICATION_CREATE: return WM_CREATE;
            case PLATFORM_NOTIFICATION_DESTROY: return WM_DESTROY;
            case PLATFORM_NOTIFICATION_DPICHANGED: return WM_DPICHANGED;
            case PLATFORM_NOTIFICATION_ENABLE: return WM_ENABLE;
            case PLATFORM_NOTIFICATION_ENTERSIZEMOVE: return WM_ENTERSIZEMOVE;
            case PLATFORM_NOTIFICATION_EXITSIZEMOVE: return WM_EXITSIZEMOVE;
            case PLATFORM_NOTIFICATION_GETICON: return WM_GETICON;
            case PLATFORM_NOTIFICATION_GETMINMAXINFO: return WM_GETMINMAXINFO;
            case PLATFORM_NOTIFICATION_INPUTLANGCHANGE: return WM_INPUTLANGCHANGE;
            case PLATFORM_NOTIFICATION_INPUTLANGCHANGEREQUEST: return WM_INPUTLANGCHANGEREQUEST;
            case PLATFORM_NOTIFICATION_MOVE: return WM_MOVE;
            case PLATFORM_NOTIFICATION_MOVING: return WM_MOVING;
            case PLATFORM_NOTIFICATION_NCACTIVATE: return WM_NCACTIVATE;
            case PLATFORM_NOTIFICATION_NCCALCSIZE: return WM_NCCALCSIZE;
            case PLATFORM_NOTIFICATION_NCCREATE: return WM_NCCREATE;
            case PLATFORM_NOTIFICATION_NCDESTROY: return WM_NCDESTROY;
            case PLATFORM_NOTIFICATION_NULL: return WM_NULL;
            case PLATFORM_NOTIFICATION_QUERYDRAGICON: return WM_QUERYDRAGICON;
            case PLATFORM_NOTIFICATION_QUERYOPEN: return WM_QUERYOPEN;
            case PLATFORM_NOTIFICATION_QUIT: return WM_QUIT;
            case PLATFORM_NOTIFICATION_SHOWWINDOW: return WM_SHOWWINDOW;
            case PLATFORM_NOTIFICATION_SIZE: return WM_SIZE;
            case PLATFORM_NOTIFICATION_SIZING: return WM_SIZING;
            case PLATFORM_NOTIFICATION_STYLECHANGED: return WM_STYLECHANGED;
            case PLATFORM_NOTIFICATION_STYLECHANGING: return WM_STYLECHANGING;
            case PLATFORM_NOTIFICATION_THEMECHANGED: return WM_THEMECHANGED;
            case PLATFORM_NOTIFICATION_USERCHANGED: return WM_USERCHANGED;
            case PLATFORM_NOTIFICATION_WINDOWPOSCHANGED: return WM_WINDOWPOSCHANGED;
            case PLATFORM_NOTIFICATION_WINDOWPOSCHANGING: return WM_WINDOWPOSCHANGING;
            default: break;
        }

        return 0;
    }

    UINT GetWindowsMouseInputType(const MouseInputType type)
    {
        switch (type)
        {
        case PLATFORM_MOUSE_INPUT_TYPE_CAPTURECHANGED: return WM_CAPTURECHANGED;
        case PLATFORM_MOUSE_INPUT_TYPE_LBUTTONDBLCLK: return WM_LBUTTONDBLCLK;
        case PLATFORM_MOUSE_INPUT_TYPE_LBUTTONDOWN: return WM_LBUTTONDOWN;
        case PLATFORM_MOUSE_INPUT_TYPE_LBUTTONUP: return WM_LBUTTONUP;
        case PLATFORM_MOUSE_INPUT_TYPE_MBUTTONDBLCLK: return WM_MBUTTONDBLCLK;
        case PLATFORM_MOUSE_INPUT_TYPE_MBUTTONDOWN: return WM_MBUTTONDOWN;
        case PLATFORM_MOUSE_INPUT_TYPE_MBUTTONUP: return WM_MBUTTONUP;
        case PLATFORM_MOUSE_INPUT_TYPE_MOUSEACTIVATE: return WM_MOUSEACTIVATE;
        case PLATFORM_MOUSE_INPUT_TYPE_MOUSEHOVER: return WM_MOUSEHOVER;
        case PLATFORM_MOUSE_INPUT_TYPE_MOUSEHWHEEL: return WM_MOUSEHWHEEL;
        case PLATFORM_MOUSE_INPUT_TYPE_MOUSELEAVE: return WM_MOUSELEAVE;
        case PLATFORM_MOUSE_INPUT_TYPE_MOUSEMOVE: return WM_MOUSEMOVE;
        case PLATFORM_MOUSE_INPUT_TYPE_MOUSEWHEEL: return WM_MOUSEWHEEL;
        case PLATFORM_MOUSE_INPUT_TYPE_NCHITTEST: return WM_NCHITTEST;
        case PLATFORM_MOUSE_INPUT_TYPE_NCLBUTTONDBLCLK: return WM_NCLBUTTONDBLCLK;
        case PLATFORM_MOUSE_INPUT_TYPE_NCLBUTTONDOWN: return WM_NCLBUTTONDOWN;
        case PLATFORM_MOUSE_INPUT_TYPE_NCLBUTTONUP: return WM_NCLBUTTONUP;
        case PLATFORM_MOUSE_INPUT_TYPE_NCMBUTTONDBLCLK: return WM_NCMBUTTONDBLCLK;
        case PLATFORM_MOUSE_INPUT_TYPE_NCMBUTTONDOWN: return WM_NCMBUTTONDOWN;
        case PLATFORM_MOUSE_INPUT_TYPE_NCMBUTTONUP: return WM_NCMBUTTONUP;
        case PLATFORM_MOUSE_INPUT_TYPE_NCMOUSEHOVER: return WM_NCMOUSEHOVER;
        case PLATFORM_MOUSE_INPUT_TYPE_NCMOUSELEAVE: return WM_NCMOUSELEAVE;
        case PLATFORM_MOUSE_INPUT_TYPE_NCMOUSEMOVE: return WM_NCMOUSEMOVE;
        case PLATFORM_MOUSE_INPUT_TYPE_NCRBUTTONDBLCLK: return WM_NCRBUTTONDBLCLK;
        case PLATFORM_MOUSE_INPUT_TYPE_NCRBUTTONDOWN: return WM_NCRBUTTONDOWN;
        case PLATFORM_MOUSE_INPUT_TYPE_NCRBUTTONUP: return WM_NCRBUTTONUP;
        case PLATFORM_MOUSE_INPUT_TYPE_NCXBUTTONDBLCLK: return WM_NCXBUTTONDBLCLK;
        case PLATFORM_MOUSE_INPUT_TYPE_NCXBUTTONDOWN: return WM_NCXBUTTONDOWN;
        case PLATFORM_MOUSE_INPUT_TYPE_NCXBUTTONUP: return WM_NCXBUTTONUP;
        case PLATFORM_MOUSE_INPUT_TYPE_RBUTTONDBLCLK: return WM_RBUTTONDBLCLK;
        case PLATFORM_MOUSE_INPUT_TYPE_RBUTTONDOWN: return WM_RBUTTONDOWN;
        case PLATFORM_MOUSE_INPUT_TYPE_RBUTTONUP: return WM_RBUTTONUP;
        case PLATFORM_MOUSE_INPUT_TYPE_XBUTTONDBLCLK: return WM_XBUTTONDBLCLK;
        case PLATFORM_MOUSE_INPUT_TYPE_XBUTTONDOWN: return WM_XBUTTONDOWN;
        case PLATFORM_MOUSE_INPUT_TYPE_XBUTTONUP: return WM_XBUTTONUP;
        }

        return 0;
    }

    UINT GetKeyboardInputType(const KeyboardInputType type)
    {
        switch (type)
        {
        case PLATFORM_KEYBOARD_INPUT_TYPE_ACTIVATE: return WM_ACTIVATE;
        case PLATFORM_KEYBOARD_INPUT_TYPE_APPCOMMAND: return WM_APPCOMMAND;
        case PLATFORM_KEYBOARD_INPUT_TYPE_CHAR: return WM_CHAR;
        case PLATFORM_KEYBOARD_INPUT_TYPE_DEADCHAR: return WM_DEADCHAR;
        case PLATFORM_KEYBOARD_INPUT_TYPE_HOTKEY: return WM_HOTKEY;
        case PLATFORM_KEYBOARD_INPUT_TYPE_KEYDOWN: return WM_KEYDOWN;
        case PLATFORM_KEYBOARD_INPUT_TYPE_KEYUP: return WM_KEYUP;
        case PLATFORM_KEYBOARD_INPUT_TYPE_KILLFOCUS: return WM_KILLFOCUS;
        case PLATFORM_KEYBOARD_INPUT_TYPE_SETFOCUS: return WM_SETFOCUS;
        case PLATFORM_KEYBOARD_INPUT_TYPE_SYSDEADCHAR: return WM_SYSDEADCHAR;
        case PLATFORM_KEYBOARD_INPUT_TYPE_SYSKEYDOWN: return WM_SYSKEYDOWN;
        case PLATFORM_KEYBOARD_INPUT_TYPE_SYSKEYUP: return WM_SYSKEYUP;
        case PLATFORM_KEYBOARD_INPUT_TYPE_UNICHAR: return WM_UNICHAR;
        }

        return 0;
    }

    NotificationType GetNotificationType(const UINT message)
    {
        switch (message)
        {
        case WM_ACTIVATEAPP: return PLATFORM_NOTIFICATION_ACTIVATEAPP;
        case WM_CANCELMODE: return PLATFORM_NOTIFICATION_CANCELMODE;
        case WM_CHILDACTIVATE: return PLATFORM_NOTIFICATION_CHILDACTIVATE;
        case WM_CLOSE: return PLATFORM_NOTIFICATION_CLOSE;
        case WM_COMPACTING: return PLATFORM_NOTIFICATION_COMPACTING;
        case WM_CREATE: return PLATFORM_NOTIFICATION_CREATE;
        case WM_DESTROY: return PLATFORM_NOTIFICATION_DESTROY;
        case WM_DPICHANGED: return PLATFORM_NOTIFICATION_DPICHANGED;
        case WM_ENABLE: return PLATFORM_NOTIFICATION_ENABLE;
        case WM_ENTERSIZEMOVE: return PLATFORM_NOTIFICATION_ENTERSIZEMOVE;
        case WM_EXITSIZEMOVE: return PLATFORM_NOTIFICATION_EXITSIZEMOVE;
        case WM_GETICON: return PLATFORM_NOTIFICATION_GETICON;
        case WM_GETMINMAXINFO: return PLATFORM_NOTIFICATION_GETMINMAXINFO;
        case WM_INPUTLANGCHANGE: return PLATFORM_NOTIFICATION_INPUTLANGCHANGE;
        case WM_INPUTLANGCHANGEREQUEST: return PLATFORM_NOTIFICATION_INPUTLANGCHANGEREQUEST;
        case WM_MOVE: return PLATFORM_NOTIFICATION_MOVE;
        case WM_MOVING: return PLATFORM_NOTIFICATION_MOVING;
        case WM_NCACTIVATE: return PLATFORM_NOTIFICATION_NCACTIVATE;
        case WM_NCCALCSIZE: return PLATFORM_NOTIFICATION_NCCALCSIZE;
        case WM_NCCREATE: return PLATFORM_NOTIFICATION_NCCREATE;
        case WM_NCDESTROY: return PLATFORM_NOTIFICATION_NCDESTROY;
        case WM_NULL: return PLATFORM_NOTIFICATION_NULL;
        case WM_QUERYDRAGICON: return PLATFORM_NOTIFICATION_QUERYDRAGICON;
        case WM_QUERYOPEN: return PLATFORM_NOTIFICATION_QUERYOPEN;
        case WM_QUIT: return PLATFORM_NOTIFICATION_QUIT;
        case WM_SHOWWINDOW: return PLATFORM_NOTIFICATION_SHOWWINDOW;
        case WM_SIZE: return PLATFORM_NOTIFICATION_SIZE;
        case WM_SIZING: return PLATFORM_NOTIFICATION_SIZING;
        case WM_STYLECHANGED: return PLATFORM_NOTIFICATION_STYLECHANGED;
        case WM_STYLECHANGING: return PLATFORM_NOTIFICATION_STYLECHANGING;
        case WM_THEMECHANGED: return PLATFORM_NOTIFICATION_THEMECHANGED;
        case WM_USERCHANGED: return PLATFORM_NOTIFICATION_USERCHANGED;
        case WM_WINDOWPOSCHANGED: return PLATFORM_NOTIFICATION_WINDOWPOSCHANGED;
        case WM_WINDOWPOSCHANGING: return PLATFORM_NOTIFICATION_WINDOWPOSCHANGING;
        default: break;
        }

        return PLATFORM_NOTIFICATION_UNKNOWN;
    }

    MouseInputType GetMouseInputType(const UINT message)
    {
        switch (message)
        {
        case WM_CAPTURECHANGED: return PLATFORM_MOUSE_INPUT_TYPE_CAPTURECHANGED;
        case WM_LBUTTONDBLCLK: return PLATFORM_MOUSE_INPUT_TYPE_LBUTTONDBLCLK;
        case WM_LBUTTONDOWN: return PLATFORM_MOUSE_INPUT_TYPE_LBUTTONDOWN;
        case WM_LBUTTONUP: return PLATFORM_MOUSE_INPUT_TYPE_LBUTTONUP;
        case WM_MBUTTONDBLCLK: return PLATFORM_MOUSE_INPUT_TYPE_MBUTTONDBLCLK;
        case WM_MBUTTONDOWN: return PLATFORM_MOUSE_INPUT_TYPE_MBUTTONDOWN;
        case WM_MBUTTONUP: return PLATFORM_MOUSE_INPUT_TYPE_MBUTTONUP;
        case WM_MOUSEACTIVATE: return PLATFORM_MOUSE_INPUT_TYPE_MOUSEACTIVATE;
        case WM_MOUSEHOVER: return PLATFORM_MOUSE_INPUT_TYPE_MOUSEHOVER;
        case WM_MOUSEHWHEEL: return PLATFORM_MOUSE_INPUT_TYPE_MOUSEHWHEEL;
        case WM_MOUSELEAVE: return PLATFORM_MOUSE_INPUT_TYPE_MOUSELEAVE;
        case WM_MOUSEMOVE: return PLATFORM_MOUSE_INPUT_TYPE_MOUSEMOVE;
        case WM_MOUSEWHEEL: return PLATFORM_MOUSE_INPUT_TYPE_MOUSEWHEEL;
        case WM_NCHITTEST: return PLATFORM_MOUSE_INPUT_TYPE_NCHITTEST;
        case WM_NCLBUTTONDBLCLK: return PLATFORM_MOUSE_INPUT_TYPE_NCLBUTTONDBLCLK;
        case WM_NCLBUTTONDOWN: return PLATFORM_MOUSE_INPUT_TYPE_NCLBUTTONDOWN;
        case WM_NCLBUTTONUP: return PLATFORM_MOUSE_INPUT_TYPE_NCLBUTTONUP;
        case WM_NCMBUTTONDBLCLK: return PLATFORM_MOUSE_INPUT_TYPE_NCMBUTTONDBLCLK;
        case WM_NCMBUTTONDOWN: return PLATFORM_MOUSE_INPUT_TYPE_NCMBUTTONDOWN;
        case WM_NCMBUTTONUP: return PLATFORM_MOUSE_INPUT_TYPE_NCMBUTTONUP;
        case WM_NCMOUSEHOVER: return PLATFORM_MOUSE_INPUT_TYPE_NCMOUSEHOVER;
        case WM_NCMOUSELEAVE: return PLATFORM_MOUSE_INPUT_TYPE_NCMOUSELEAVE;
        case WM_NCMOUSEMOVE: return PLATFORM_MOUSE_INPUT_TYPE_NCMOUSEMOVE;
        case WM_NCRBUTTONDBLCLK: return PLATFORM_MOUSE_INPUT_TYPE_NCRBUTTONDBLCLK;
        case WM_NCRBUTTONDOWN: return PLATFORM_MOUSE_INPUT_TYPE_NCRBUTTONDOWN;
        case WM_NCRBUTTONUP: return PLATFORM_MOUSE_INPUT_TYPE_NCRBUTTONUP;
        case WM_NCXBUTTONDBLCLK: return PLATFORM_MOUSE_INPUT_TYPE_NCXBUTTONDBLCLK;
        case WM_NCXBUTTONDOWN: return PLATFORM_MOUSE_INPUT_TYPE_NCXBUTTONDOWN;
        case WM_NCXBUTTONUP: return PLATFORM_MOUSE_INPUT_TYPE_NCXBUTTONUP;
        case WM_RBUTTONDBLCLK: return PLATFORM_MOUSE_INPUT_TYPE_RBUTTONDBLCLK;
        case WM_RBUTTONDOWN: return PLATFORM_MOUSE_INPUT_TYPE_RBUTTONDOWN;
        case WM_RBUTTONUP: return PLATFORM_MOUSE_INPUT_TYPE_RBUTTONUP;
        case WM_XBUTTONDBLCLK: return PLATFORM_MOUSE_INPUT_TYPE_XBUTTONDBLCLK;
        case WM_XBUTTONDOWN: return PLATFORM_MOUSE_INPUT_TYPE_XBUTTONDOWN;
        case WM_XBUTTONUP: return PLATFORM_MOUSE_INPUT_TYPE_XBUTTONUP;
        }

        return PLATFORM_MOUSE_INPUT_TYPE_UNKNOWN;
    }

    KeyboardInputType GetKeyboardInputType(const UINT message)
    {
        switch (message)
        {
        case WM_ACTIVATE: return PLATFORM_KEYBOARD_INPUT_TYPE_ACTIVATE;
        case WM_APPCOMMAND: return PLATFORM_KEYBOARD_INPUT_TYPE_APPCOMMAND;
        case WM_CHAR: return PLATFORM_KEYBOARD_INPUT_TYPE_CHAR;
        case WM_DEADCHAR: return PLATFORM_KEYBOARD_INPUT_TYPE_DEADCHAR;
        case WM_HOTKEY: return PLATFORM_KEYBOARD_INPUT_TYPE_HOTKEY;
        case WM_KEYDOWN: return PLATFORM_KEYBOARD_INPUT_TYPE_KEYDOWN;
        case WM_KEYUP: return PLATFORM_KEYBOARD_INPUT_TYPE_KEYUP;
        case WM_KILLFOCUS: return PLATFORM_KEYBOARD_INPUT_TYPE_KILLFOCUS;
        case WM_SETFOCUS: return PLATFORM_KEYBOARD_INPUT_TYPE_SETFOCUS;
        case WM_SYSDEADCHAR: return PLATFORM_KEYBOARD_INPUT_TYPE_SYSDEADCHAR;
        case WM_SYSKEYDOWN: return PLATFORM_KEYBOARD_INPUT_TYPE_SYSKEYDOWN;
        case WM_SYSKEYUP: return PLATFORM_KEYBOARD_INPUT_TYPE_SYSKEYUP;
        case WM_UNICHAR: return PLATFORM_KEYBOARD_INPUT_TYPE_UNICHAR;
        }

        return PLATFORM_KEYBOARD_INPUT_TYPE_UNKNOWN;
    }

	UINT GetKeyCode(const KeyType eKey)
	{
		switch (eKey)
		{
		case(KEYTYPE_LBUTTON):    return VK_LBUTTON;
		case(KEYTYPE_RBUTTON):    return VK_RBUTTON;
		case(KEYTYPE_CANCEL):    return VK_CANCEL;
		case(KEYTYPE_MBUTTON):    return VK_MBUTTON;
		case(KEYTYPE_XBUTTON1):    return VK_XBUTTON1;
		case(KEYTYPE_XBUTTON2):    return VK_XBUTTON2;
		case(KEYTYPE_BACK):    return VK_BACK;
		case(KEYTYPE_TAB):    return VK_TAB;
		case(KEYTYPE_CLEAR):    return VK_CLEAR;
		case(KEYTYPE_RETURN):    return VK_RETURN;
		case(KEYTYPE_SHIFT):    return VK_SHIFT;
		case(KEYTYPE_CONTROL):    return VK_CONTROL;
		case(KEYTYPE_MENU):    return VK_MENU;
		case(KEYTYPE_PAUSE):    return VK_PAUSE;
		case(KEYTYPE_CAPITAL):    return VK_CAPITAL;
		case(KEYTYPE_KANA):    return VK_KANA;
		case(KEYTYPE_HANGUEL):    return VK_HANGUL;
		case(KEYTYPE_HANGUL):    return VK_HANGUL;
		case(KEYTYPE_JUNJA):    return VK_JUNJA;
		case(KEYTYPE_FINAL):    return VK_FINAL;
		case(KEYTYPE_HANJA):    return VK_HANJA;
		case(KEYTYPE_KANJI):    return VK_KANJI;
		case(KEYTYPE_ESCAPE):    return VK_ESCAPE;
		case(KEYTYPE_CONVERT):    return VK_CONVERT;
		case(KEYTYPE_NONCONVERT):    return VK_NONCONVERT;
		case(KEYTYPE_ACCEPT):    return VK_ACCEPT;
		case(KEYTYPE_MODECHANGE):    return VK_MODECHANGE;
		case(KEYTYPE_SPACE):    return VK_SPACE;
		case(KEYTYPE_PRIOR):    return VK_PRIOR;
		case(KEYTYPE_NEXT):    return VK_NEXT;
		case(KEYTYPE_END):    return VK_END;
		case(KEYTYPE_HOME):    return VK_HOME;
		case(KEYTYPE_LEFT):    return VK_LEFT;
		case(KEYTYPE_UP):    return VK_UP;
		case(KEYTYPE_RIGHT):    return VK_RIGHT;
		case(KEYTYPE_DOWN):    return VK_DOWN;
		case(KEYTYPE_SELECT):    return VK_SELECT;
		case(KEYTYPE_PRINT):    return VK_PRINT;
		case(KEYTYPE_EXECUTE):    return VK_EXECUTE;
		case(KEYTYPE_SNAPSHOT):    return VK_SNAPSHOT;
		case(KEYTYPE_INSERT):    return VK_INSERT;
		case(KEYTYPE_DELETE):    return VK_DELETE;
		case(KEYTYPE_HELP):    return VK_HELP;
		case(KEYTYPE_0):    return 0x30;
		case(KEYTYPE_1):    return 0x31;
		case(KEYTYPE_2):    return 0x32;
		case(KEYTYPE_3):    return 0x33;
		case(KEYTYPE_4):    return 0x34;
		case(KEYTYPE_5):    return 0x35;
		case(KEYTYPE_6):    return 0x36;
		case(KEYTYPE_7):    return 0x37;
		case(KEYTYPE_8):    return 0x38;
		case(KEYTYPE_9):    return 0x39;
		case(KEYTYPE_A):    return 0x41;
		case(KEYTYPE_B):    return 0x42;
		case(KEYTYPE_C):    return 0x43;
		case(KEYTYPE_D):    return 0x44;
		case(KEYTYPE_E):    return 0x45;
		case(KEYTYPE_F):    return 0x46;
		case(KEYTYPE_G):    return 0x47;
		case(KEYTYPE_H):    return 0x48;
		case(KEYTYPE_I):    return 0x49;
		case(KEYTYPE_J):    return 0x4a;
		case(KEYTYPE_K):    return 0x4b;
		case(KEYTYPE_L):    return 0x4c;
		case(KEYTYPE_M):    return 0x4d;
		case(KEYTYPE_N):    return 0x4e;
		case(KEYTYPE_O):    return 0x4f;
		case(KEYTYPE_P):    return 0x50;
		case(KEYTYPE_Q):    return 0x51;
		case(KEYTYPE_R):    return 0x52;
		case(KEYTYPE_S):    return 0x53;
		case(KEYTYPE_T):    return 0x54;
		case(KEYTYPE_U):    return 0x55;
		case(KEYTYPE_V):    return 0x56;
		case(KEYTYPE_W):    return 0x57;
		case(KEYTYPE_X):    return 0x58;
		case(KEYTYPE_Y):    return 0x59;
		case(KEYTYPE_Z):    return 0x5a;
		case(KEYTYPE_LWIN):    return VK_LWIN;
		case(KEYTYPE_RWIN):    return VK_RWIN;
		case(KEYTYPE_APPS):    return VK_APPS;
		case(KEYTYPE_SLEEP):    return VK_SLEEP;
		case(KEYTYPE_NUMPAD0):    return VK_NUMPAD0;
		case(KEYTYPE_NUMPAD1):    return VK_NUMPAD1;
		case(KEYTYPE_NUMPAD2):    return VK_NUMPAD2;
		case(KEYTYPE_NUMPAD3):    return VK_NUMPAD3;
		case(KEYTYPE_NUMPAD4):    return VK_NUMPAD4;
		case(KEYTYPE_NUMPAD5):    return VK_NUMPAD5;
		case(KEYTYPE_NUMPAD6):    return VK_NUMPAD6;
		case(KEYTYPE_NUMPAD7):    return VK_NUMPAD7;
		case(KEYTYPE_NUMPAD8):    return VK_NUMPAD8;
		case(KEYTYPE_NUMPAD9):    return VK_NUMPAD9;
		case(KEYTYPE_MULTIPLY):    return VK_MULTIPLY;
		case(KEYTYPE_ADD):    return VK_ADD;
		case(KEYTYPE_SEPARATOR):    return VK_SEPARATOR;
		case(KEYTYPE_SUBTRACT):    return VK_SUBTRACT;
		case(KEYTYPE_DECIMAL):    return VK_DECIMAL;
		case(KEYTYPE_DIVIDE):    return VK_DIVIDE;
		case(KEYTYPE_F1):    return VK_F1;
		case(KEYTYPE_F2):    return VK_F2;
		case(KEYTYPE_F3):    return VK_F3;
		case(KEYTYPE_F4):    return VK_F4;
		case(KEYTYPE_F5):    return VK_F5;
		case(KEYTYPE_F6):    return VK_F6;
		case(KEYTYPE_F7):    return VK_F7;
		case(KEYTYPE_F8):    return VK_F8;
		case(KEYTYPE_F9):    return VK_F9;
		case(KEYTYPE_F10):    return VK_F10;
		case(KEYTYPE_F11):    return VK_F11;
		case(KEYTYPE_F12):    return VK_F12;
		case(KEYTYPE_F13):    return VK_F13;
		case(KEYTYPE_F14):    return VK_F14;
		case(KEYTYPE_F15):    return VK_F15;
		case(KEYTYPE_F16):    return VK_F16;
		case(KEYTYPE_F17):    return VK_F17;
		case(KEYTYPE_F18):    return VK_F18;
		case(KEYTYPE_F19):    return VK_F19;
		case(KEYTYPE_F20):    return VK_F20;
		case(KEYTYPE_F21):    return VK_F21;
		case(KEYTYPE_F22):    return VK_F22;
		case(KEYTYPE_F23):    return VK_F23;
		case(KEYTYPE_F24):    return VK_F24;
		case(KEYTYPE_NUMLOCK):    return VK_NUMLOCK;
		case(KEYTYPE_SCROLL):    return VK_SCROLL;
		case(KEYTYPE_LSHIFT):    return VK_LSHIFT;
		case(KEYTYPE_RSHIFT):    return VK_RSHIFT;
		case(KEYTYPE_LCONTROL):    return VK_LCONTROL;
		case(KEYTYPE_RCONTROL):    return VK_RCONTROL;
		case(KEYTYPE_LMENU):    return VK_LMENU;
		case(KEYTYPE_RMENU):    return VK_RMENU;
		case(KEYTYPE_BROWSER_BACK):    return VK_BROWSER_BACK;
		case(KEYTYPE_BROWSER_FORWARD):    return VK_BROWSER_FORWARD;
		case(KEYTYPE_BROWSER_REFRESH):    return VK_BROWSER_REFRESH;
		case(KEYTYPE_BROWSER_STOP):    return VK_BROWSER_STOP;
		case(KEYTYPE_BROWSER_SEARCH):    return VK_BROWSER_SEARCH;
		case(KEYTYPE_BROWSER_FAVORITES):    return VK_BROWSER_FAVORITES;
		case(KEYTYPE_BROWSER_HOME):    return VK_BROWSER_HOME;
		case(KEYTYPE_VOLUME_MUTE):    return VK_VOLUME_MUTE;
		case(KEYTYPE_VOLUME_DOWN):    return VK_VOLUME_DOWN;
		case(KEYTYPE_VOLUME_UP):    return VK_VOLUME_UP;
		case(KEYTYPE_MEDIA_NEXT_TRACK):    return VK_MEDIA_NEXT_TRACK;
		case(KEYTYPE_MEDIA_PREV_TRACK):    return VK_MEDIA_PREV_TRACK;
		case(KEYTYPE_MEDIA_STOP):    return VK_MEDIA_STOP;
		case(KEYTYPE_MEDIA_PLAY_PAUSE):    return VK_MEDIA_PLAY_PAUSE;
		case(KEYTYPE_LAUNCH_MAIL):    return VK_LAUNCH_MAIL;
		case(KEYTYPE_LAUNCH_MEDIA_SELECT):    return VK_LAUNCH_MEDIA_SELECT;
		case(KEYTYPE_LAUNCH_APP1):    return VK_LAUNCH_APP1;
		case(KEYTYPE_LAUNCH_APP2):    return VK_LAUNCH_APP2;
		case(KEYTYPE_OEM_1):    return VK_OEM_1;
		case(KEYTYPE_OEM_PLUS):    return VK_OEM_PLUS;
		case(KEYTYPE_OEM_COMMA):    return VK_OEM_COMMA;
		case(KEYTYPE_OEM_MINUS):    return VK_OEM_MINUS;
		case(KEYTYPE_OEM_PERIOD):    return VK_OEM_PERIOD;
		case(KEYTYPE_OEM_2):    return VK_OEM_2;
		case(KEYTYPE_OEM_3):    return VK_OEM_3;
		case(KEYTYPE_OEM_4):    return VK_OEM_4;
		case(KEYTYPE_OEM_5):    return VK_OEM_5;
		case(KEYTYPE_OEM_6):    return VK_OEM_6;
		case(KEYTYPE_OEM_7):    return VK_OEM_7;
		case(KEYTYPE_OEM_8):    return VK_OEM_8;
		case(KEYTYPE_OEM_102):    return VK_OEM_102;
		case(KEYTYPE_PROCESSKEY):    return VK_PROCESSKEY;
		case(KEYTYPE_PACKET):    return VK_PACKET;
		case(KEYTYPE_ATTN):    return VK_ATTN;
		case(KEYTYPE_CRSEL):    return VK_CRSEL;
		case(KEYTYPE_EXSEL):    return VK_EXSEL;
		case(KEYTYPE_EREOF):    return VK_EREOF;
		case(KEYTYPE_PLAY):    return VK_PLAY;
		case(KEYTYPE_ZOOM):    return VK_ZOOM;
		case(KEYTYPE_NONAME):    return VK_NONAME;
		case(KEYTYPE_PA1):    return VK_PA1;
		case(KEYTYPE_OEM_CLEAR):    return VK_OEM_CLEAR;
		default:    return 0;
		}
	}

	KeyType GetKeyType(const UINT code)
	{
		switch (code)
		{
		case(VK_LBUTTON):    return KEYTYPE_LBUTTON;
		case(VK_RBUTTON):    return KEYTYPE_RBUTTON;
		case(VK_CANCEL):    return KEYTYPE_CANCEL;
		case(VK_MBUTTON):    return KEYTYPE_MBUTTON;
		case(VK_XBUTTON1):    return KEYTYPE_XBUTTON1;
		case(VK_XBUTTON2):    return KEYTYPE_XBUTTON2;
		case(VK_BACK):    return KEYTYPE_BACK;
		case(VK_TAB):    return KEYTYPE_TAB;
		case(VK_CLEAR):    return KEYTYPE_CLEAR;
		case(VK_RETURN):    return KEYTYPE_RETURN;
		case(VK_SHIFT):    return KEYTYPE_SHIFT;
		case(VK_CONTROL):    return KEYTYPE_CONTROL;
		case(VK_MENU):    return KEYTYPE_MENU;
		case(VK_PAUSE):    return KEYTYPE_PAUSE;
		case(VK_CAPITAL):    return KEYTYPE_CAPITAL;
		case(VK_KANA):    return KEYTYPE_KANA;
		//case(VK_HANGUL):    return KEYTYPE_HANGUL;
		case(VK_JUNJA):    return KEYTYPE_JUNJA;
		case(VK_FINAL):    return KEYTYPE_FINAL;
		case(VK_HANJA):    return KEYTYPE_HANJA;
		//case(VK_KANJI):    return KEYTYPE_KANJI;
		case(VK_ESCAPE):    return KEYTYPE_ESCAPE;
		case(VK_CONVERT):    return KEYTYPE_CONVERT;
		case(VK_NONCONVERT):    return KEYTYPE_NONCONVERT;
		case(VK_ACCEPT):    return KEYTYPE_ACCEPT;
		case(VK_MODECHANGE):    return KEYTYPE_MODECHANGE;
		case(VK_SPACE):    return KEYTYPE_SPACE;
		case(VK_PRIOR):    return KEYTYPE_PRIOR;
		case(VK_NEXT):    return KEYTYPE_NEXT;
		case(VK_END):    return KEYTYPE_END;
		case(VK_HOME):    return KEYTYPE_HOME;
		case(VK_LEFT):    return KEYTYPE_LEFT;
		case(VK_UP):    return KEYTYPE_UP;
		case(VK_RIGHT):    return KEYTYPE_RIGHT;
		case(VK_DOWN):    return KEYTYPE_DOWN;
		case(VK_SELECT):    return KEYTYPE_SELECT;
		case(VK_PRINT):    return KEYTYPE_PRINT;
		case(VK_EXECUTE):    return KEYTYPE_EXECUTE;
		case(VK_SNAPSHOT):    return KEYTYPE_SNAPSHOT;
		case(VK_INSERT):    return KEYTYPE_INSERT;
		case(VK_DELETE):    return KEYTYPE_DELETE;
		case(VK_HELP):    return KEYTYPE_HELP;
		case(0x30):    return KEYTYPE_0;
		case(0x31):    return KEYTYPE_1;
		case(0x32):    return KEYTYPE_2;
		case(0x33):    return KEYTYPE_3;
		case(0x34):    return KEYTYPE_4;
		case(0x35):    return KEYTYPE_5;
		case(0x36):    return KEYTYPE_6;
		case(0x37):    return KEYTYPE_7;
		case(0x38):    return KEYTYPE_8;
		case(0x39):    return KEYTYPE_9;
		case(0x41):    return KEYTYPE_A;
		case(0x42):    return KEYTYPE_B;
		case(0x43):    return KEYTYPE_C;
		case(0x44):    return KEYTYPE_D;
		case(0x45):    return KEYTYPE_E;
		case(0x46):    return KEYTYPE_F;
		case(0x47):    return KEYTYPE_G;
		case(0x48):    return KEYTYPE_H;
		case(0x49):    return KEYTYPE_I;
		case(0x4a):    return KEYTYPE_J;
		case(0x4b):    return KEYTYPE_K;
		case(0x4c):    return KEYTYPE_L;
		case(0x4d):    return KEYTYPE_M;
		case(0x4e):    return KEYTYPE_N;
		case(0x4f):    return KEYTYPE_O;
		case(0x50):    return KEYTYPE_P;
		case(0x51):    return KEYTYPE_Q;
		case(0x52):    return KEYTYPE_R;
		case(0x53):    return KEYTYPE_S;
		case(0x54):    return KEYTYPE_T;
		case(0x55):    return KEYTYPE_U;
		case(0x56):    return KEYTYPE_V;
		case(0x57):    return KEYTYPE_W;
		case(0x58):    return KEYTYPE_X;
		case(0x59):    return KEYTYPE_Y;
		case(0x5a):    return KEYTYPE_Z;
		case(VK_LWIN):    return KEYTYPE_LWIN;
		case(VK_RWIN):    return KEYTYPE_RWIN;
		case(VK_APPS):    return KEYTYPE_APPS;
		case(VK_SLEEP):    return KEYTYPE_SLEEP;
		case(VK_NUMPAD0):    return KEYTYPE_NUMPAD0;
		case(VK_NUMPAD1):    return KEYTYPE_NUMPAD1;
		case(VK_NUMPAD2):    return KEYTYPE_NUMPAD2;
		case(VK_NUMPAD3):    return KEYTYPE_NUMPAD3;
		case(VK_NUMPAD4):    return KEYTYPE_NUMPAD4;
		case(VK_NUMPAD5):    return KEYTYPE_NUMPAD5;
		case(VK_NUMPAD6):    return KEYTYPE_NUMPAD6;
		case(VK_NUMPAD7):    return KEYTYPE_NUMPAD7;
		case(VK_NUMPAD8):    return KEYTYPE_NUMPAD8;
		case(VK_NUMPAD9):    return KEYTYPE_NUMPAD9;
		case(VK_MULTIPLY):    return KEYTYPE_MULTIPLY;
		case(VK_ADD):    return KEYTYPE_ADD;
		case(VK_SEPARATOR):    return KEYTYPE_SEPARATOR;
		case(VK_SUBTRACT):    return KEYTYPE_SUBTRACT;
		case(VK_DECIMAL):    return KEYTYPE_DECIMAL;
		case(VK_DIVIDE):    return KEYTYPE_DIVIDE;
		case(VK_F1):    return KEYTYPE_F1;
		case(VK_F2):    return KEYTYPE_F2;
		case(VK_F3):    return KEYTYPE_F3;
		case(VK_F4):    return KEYTYPE_F4;
		case(VK_F5):    return KEYTYPE_F5;
		case(VK_F6):    return KEYTYPE_F6;
		case(VK_F7):    return KEYTYPE_F7;
		case(VK_F8):    return KEYTYPE_F8;
		case(VK_F9):    return KEYTYPE_F9;
		case(VK_F10):    return KEYTYPE_F10;
		case(VK_F11):    return KEYTYPE_F11;
		case(VK_F12):    return KEYTYPE_F12;
		case(VK_F13):    return KEYTYPE_F13;
		case(VK_F14):    return KEYTYPE_F14;
		case(VK_F15):    return KEYTYPE_F15;
		case(VK_F16):    return KEYTYPE_F16;
		case(VK_F17):    return KEYTYPE_F17;
		case(VK_F18):    return KEYTYPE_F18;
		case(VK_F19):    return KEYTYPE_F19;
		case(VK_F20):    return KEYTYPE_F20;
		case(VK_F21):    return KEYTYPE_F21;
		case(VK_F22):    return KEYTYPE_F22;
		case(VK_F23):    return KEYTYPE_F23;
		case(VK_F24):    return KEYTYPE_F24;
		case(VK_NUMLOCK):    return KEYTYPE_NUMLOCK;
		case(VK_SCROLL):    return KEYTYPE_SCROLL;
		case(VK_LSHIFT):    return KEYTYPE_LSHIFT;
		case(VK_RSHIFT):    return KEYTYPE_RSHIFT;
		case(VK_LCONTROL):    return KEYTYPE_LCONTROL;
		case(VK_RCONTROL):    return KEYTYPE_RCONTROL;
		case(VK_LMENU):    return KEYTYPE_LMENU;
		case(VK_RMENU):    return KEYTYPE_RMENU;
		case(VK_BROWSER_BACK):    return KEYTYPE_BROWSER_BACK;
		case(VK_BROWSER_FORWARD):    return KEYTYPE_BROWSER_FORWARD;
		case(VK_BROWSER_REFRESH):    return KEYTYPE_BROWSER_REFRESH;
		case(VK_BROWSER_STOP):    return KEYTYPE_BROWSER_STOP;
		case(VK_BROWSER_SEARCH):    return KEYTYPE_BROWSER_SEARCH;
		case(VK_BROWSER_FAVORITES):    return KEYTYPE_BROWSER_FAVORITES;
		case(VK_BROWSER_HOME):    return KEYTYPE_BROWSER_HOME;
		case(VK_VOLUME_MUTE):    return KEYTYPE_VOLUME_MUTE;
		case(VK_VOLUME_DOWN):    return KEYTYPE_VOLUME_DOWN;
		case(VK_VOLUME_UP):    return KEYTYPE_VOLUME_UP;
		case(VK_MEDIA_NEXT_TRACK):    return KEYTYPE_MEDIA_NEXT_TRACK;
		case(VK_MEDIA_PREV_TRACK):    return KEYTYPE_MEDIA_PREV_TRACK;
		case(VK_MEDIA_STOP):    return KEYTYPE_MEDIA_STOP;
		case(VK_MEDIA_PLAY_PAUSE):    return KEYTYPE_MEDIA_PLAY_PAUSE;
		case(VK_LAUNCH_MAIL):    return KEYTYPE_LAUNCH_MAIL;
		case(VK_LAUNCH_MEDIA_SELECT):    return KEYTYPE_LAUNCH_MEDIA_SELECT;
		case(VK_LAUNCH_APP1):    return KEYTYPE_LAUNCH_APP1;
		case(VK_LAUNCH_APP2):    return KEYTYPE_LAUNCH_APP2;
		case(VK_OEM_1):    return KEYTYPE_OEM_1;
		case(VK_OEM_PLUS):    return KEYTYPE_OEM_PLUS;
		case(VK_OEM_COMMA):    return KEYTYPE_OEM_COMMA;
		case(VK_OEM_MINUS):    return KEYTYPE_OEM_MINUS;
		case(VK_OEM_PERIOD):    return KEYTYPE_OEM_PERIOD;
		case(VK_OEM_2):    return KEYTYPE_OEM_2;
		case(VK_OEM_3):    return KEYTYPE_OEM_3;
		case(VK_OEM_4):    return KEYTYPE_OEM_4;
		case(VK_OEM_5):    return KEYTYPE_OEM_5;
		case(VK_OEM_6):    return KEYTYPE_OEM_6;
		case(VK_OEM_7):    return KEYTYPE_OEM_7;
		case(VK_OEM_8):    return KEYTYPE_OEM_8;
		case(VK_OEM_102):    return KEYTYPE_OEM_102;
		case(VK_PROCESSKEY):    return KEYTYPE_PROCESSKEY;
		case(VK_PACKET):    return KEYTYPE_PACKET;
		case(VK_ATTN):    return KEYTYPE_ATTN;
		case(VK_CRSEL):    return KEYTYPE_CRSEL;
		case(VK_EXSEL):    return KEYTYPE_EXSEL;
		case(VK_EREOF):    return KEYTYPE_EREOF;
		case(VK_PLAY):    return KEYTYPE_PLAY;
		case(VK_ZOOM):    return KEYTYPE_ZOOM;
		case(VK_NONAME):    return KEYTYPE_NONAME;
		case(VK_PA1):    return KEYTYPE_PA1;
		case(VK_OEM_CLEAR):    return KEYTYPE_OEM_CLEAR;
		default:    return KEYTYPE_UNKNOWN;
		}
	}
}
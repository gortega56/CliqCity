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
}
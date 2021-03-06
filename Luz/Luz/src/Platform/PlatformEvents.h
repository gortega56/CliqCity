#pragma once
#ifndef PLATFORMEVENTS_H
#define PLATFORMEVENTS_H

#ifndef DELEGATE_H
#include "Delegate.h"
#endif

#ifndef PLATFORMTYPES_H
#include "PlatformTypes.h"
#endif

namespace Platform
{	
	enum EventType
	{
		// Window events
		PLATFORM_EVENT_TYPE_WINDOW_ACTIVATEAPP,
		PLATFORM_EVENT_TYPE_WINDOW_CANCELMODE,
		PLATFORM_EVENT_TYPE_WINDOW_CHILDACTIVATE,
		PLATFORM_EVENT_TYPE_WINDOW_CLOSE,
		PLATFORM_EVENT_TYPE_WINDOW_COMPACTING,
		PLATFORM_EVENT_TYPE_WINDOW_CREATE,
		PLATFORM_EVENT_TYPE_WINDOW_DESTROY,
		PLATFORM_EVENT_TYPE_WINDOW_DPICHANGED,
		PLATFORM_EVENT_TYPE_WINDOW_ENABLE,
		PLATFORM_EVENT_TYPE_WINDOW_ENTERSIZEMOVE,
		PLATFORM_EVENT_TYPE_WINDOW_EXITSIZEMOVE,
		PLATFORM_EVENT_TYPE_WINDOW_GETICON,
		PLATFORM_EVENT_TYPE_WINDOW_GETMINMAXINFO,
		PLATFORM_EVENT_TYPE_WINDOW_INPUTLANGCHANGE,
		PLATFORM_EVENT_TYPE_WINDOW_INPUTLANGCHANGEREQUEST,
		PLATFORM_EVENT_TYPE_WINDOW_MOVE,
		PLATFORM_EVENT_TYPE_WINDOW_MOVING,
		PLATFORM_EVENT_TYPE_WINDOW_NCACTIVATE,
		PLATFORM_EVENT_TYPE_WINDOW_NCCALCSIZE,
		PLATFORM_EVENT_TYPE_WINDOW_NCCREATE,
		PLATFORM_EVENT_TYPE_WINDOW_NCDESTROY,
		PLATFORM_EVENT_TYPE_WINDOW_NULL,
		PLATFORM_EVENT_TYPE_WINDOW_QUERYDRAGICON,
		PLATFORM_EVENT_TYPE_WINDOW_QUERYOPEN,
		PLATFORM_EVENT_TYPE_WINDOW_QUIT,
		PLATFORM_EVENT_TYPE_WINDOW_SHOWWINDOW,
		PLATFORM_EVENT_TYPE_WINDOW_SIZE,
		PLATFORM_EVENT_TYPE_WINDOW_SIZING,
		PLATFORM_EVENT_TYPE_WINDOW_STYLECHANGED,
		PLATFORM_EVENT_TYPE_WINDOW_STYLECHANGING,
		PLATFORM_EVENT_TYPE_WINDOW_THEMECHANGED,
		PLATFORM_EVENT_TYPE_WINDOW_USERCHANGED,
		PLATFORM_EVENT_TYPE_WINDOW_WINDOWPOSCHANGED,
		PLATFORM_EVENT_TYPE_WINDOW_WINDOWPOSCHANGING,

		// Keyboard events
		PLATFORM_EVENT_TYPE_KEYBOARD_ACTIVATE,
		PLATFORM_EVENT_TYPE_KEYBOARD_APPCOMMAND,
		PLATFORM_EVENT_TYPE_KEYBOARD_CHAR,
		PLATFORM_EVENT_TYPE_KEYBOARD_DEADCHAR,
		PLATFORM_EVENT_TYPE_KEYBOARD_HOTKEY,
		PLATFORM_EVENT_TYPE_KEYBOARD_KEYDOWN,
		PLATFORM_EVENT_TYPE_KEYBOARD_KEYUP,
		PLATFORM_EVENT_TYPE_KEYBOARD_KILLFOCUS,
		PLATFORM_EVENT_TYPE_KEYBOARD_SETFOCUS,
		PLATFORM_EVENT_TYPE_KEYBOARD_SYSDEADCHAR,
		PLATFORM_EVENT_TYPE_KEYBOARD_SYSKEYDOWN,
		PLATFORM_EVENT_TYPE_KEYBOARD_SYSKEYUP,
		PLATFORM_EVENT_TYPE_KEYBOARD_UNICHAR,

		// Mouse events
		PLATFORM_EVENT_TYPE_MOUSE_CAPTURECHANGED,
		PLATFORM_EVENT_TYPE_MOUSE_LBUTTONDBLCLK,
		PLATFORM_EVENT_TYPE_MOUSE_LBUTTONDOWN,
		PLATFORM_EVENT_TYPE_MOUSE_LBUTTONUP,
		PLATFORM_EVENT_TYPE_MOUSE_MBUTTONDBLCLK,
		PLATFORM_EVENT_TYPE_MOUSE_MBUTTONDOWN,
		PLATFORM_EVENT_TYPE_MOUSE_MBUTTONUP,
		PLATFORM_EVENT_TYPE_MOUSE_ACTIVATE,
		PLATFORM_EVENT_TYPE_MOUSE_HOVER,
		PLATFORM_EVENT_TYPE_MOUSE_HWHEEL,
		PLATFORM_EVENT_TYPE_MOUSE_LEAVE,
		PLATFORM_EVENT_TYPE_MOUSE_MOVE,
		PLATFORM_EVENT_TYPE_MOUSE_WHEEL,
		PLATFORM_EVENT_TYPE_MOUSE_NCHITTEST,
		PLATFORM_EVENT_TYPE_MOUSE_NCLBUTTONDBLCLK,
		PLATFORM_EVENT_TYPE_MOUSE_NCLBUTTONDOWN,
		PLATFORM_EVENT_TYPE_MOUSE_NCLBUTTONUP,
		PLATFORM_EVENT_TYPE_MOUSE_NCMBUTTONDBLCLK,
		PLATFORM_EVENT_TYPE_MOUSE_NCMBUTTONDOWN,
		PLATFORM_EVENT_TYPE_MOUSE_NCMBUTTONUP,
		PLATFORM_EVENT_TYPE_MOUSE_NCHOVER,
		PLATFORM_EVENT_TYPE_MOUSE_NCLEAVE,
		PLATFORM_EVENT_TYPE_MOUSE_NCMOVE,
		PLATFORM_EVENT_TYPE_MOUSE_NCRBUTTONDBLCLK,
		PLATFORM_EVENT_TYPE_MOUSE_NCRBUTTONDOWN,
		PLATFORM_EVENT_TYPE_MOUSE_NCRBUTTONUP,
		PLATFORM_EVENT_TYPE_MOUSE_NCXBUTTONDBLCLK,
		PLATFORM_EVENT_TYPE_MOUSE_NCXBUTTONDOWN,
		PLATFORM_EVENT_TYPE_MOUSE_NCXBUTTONUP,
		PLATFORM_EVENT_TYPE_MOUSE_RBUTTONDBLCLK,
		PLATFORM_EVENT_TYPE_MOUSE_RBUTTONDOWN,
		PLATFORM_EVENT_TYPE_MOUSE_RBUTTONUP,
		PLATFORM_EVENT_TYPE_MOUSE_XBUTTONDBLCLK,
		PLATFORM_EVENT_TYPE_MOUSE_XBUTTONDOWN,
		PLATFORM_EVENT_TYPE_MOUSE_XBUTTONUP,
		PLATFORM_EVENT_TYPE_COUNT,
		PLATFORM_EVENT_TYPE_UNKNOWN
	};

	struct Message
	{
		EventType eEventType;

		union
		{
			struct WindowSize
			{
				unsigned short Width;
				unsigned short Height;
				unsigned char bMaximized : 1;
				unsigned char bMinimized : 1;
				unsigned char bRestored : 1;
				unsigned char bDragEnter : 1;
				unsigned char bDragExit : 1;
			} WindowSize;

			struct Keyboard
			{
				KeyType eKey;
				unsigned char bRecurrent : 1;
			} Keyboard;

			struct MouseMove
			{
				unsigned short PositionX;
				unsigned short PositionY;
			} MouseMove;

			struct MouseClick
			{
				typedef short ButtonCode;
				ButtonCode Code;
			} MouseClick;

			struct MouseWheel
			{
				float Delta;
			} MouseWheel;
		};
	};

	typedef Luz::Event<void(const Message&)> Event;

	LUZ_API Event* GetEvent(const EventType eEvent);

	void Initialize_Events();

	void Update_Events(double delta);

    void Register_EventFunction(void* pFunction);
}

#endif
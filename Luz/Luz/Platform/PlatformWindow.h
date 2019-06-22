#pragma once
#ifndef PLATFORMWINDOW_H
#define PLATFORMWINDOW_H

namespace Platform
{
	struct LUZ_API Window
	{
		unsigned int Width;
		unsigned int Height;
		float Aspect;
		bool bFullScreen;
	};

    struct Rect;

	typedef int8_t WindowHandle;

	LUZ_API WindowHandle MakeWindow(const char* pCaption, unsigned int width, unsigned int height, bool bFullScreen);

	LUZ_API void FreeWindow(const WindowHandle handle);

	LUZ_API bool GetWindow(const WindowHandle handle, Window& window);

    LUZ_API bool GetDisplay(const WindowHandle handle, Window& window);

    LUZ_API void GetWindow(const WindowHandle handle, Rect& rect);

    LUZ_API void GetDisplay(const WindowHandle handle, Rect& rect);

	void Shutdown_Windows();
}

#endif
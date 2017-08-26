#pragma once
#ifndef WINDOWS_UTILS_H
#define WINDOWS_UTILS_H

HWND MakeWindowInternal(HINSTANCE hInstance, WNDPROC winProc, LPCWSTR className, LPCWSTR caption, int width, int height, bool fullscreen);

#endif // !WINDOWS_UTILS_H


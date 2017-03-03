#pragma once
#ifndef OSWIN_H
#define OSWIN_H

#include <memory>
#include <unordered_map>
#include <Windows.h>

struct Options;

class Window;
class WindowsMessageCenter;

class OSWin
{
public:
    OSWin();
    ~OSWin();
    
    bool Initialize(const Options& options);
    void Update(double dt);
    void Shutdown();


    HINSTANCE HInstance() { return m_hInstance; }
    
    std::shared_ptr<WindowsMessageCenter> MessageCenter() { return m_messagePump; }

    std::shared_ptr<Window> MakeWindow(LPCWSTR caption, int width, int height, bool fullscreen);
    std::shared_ptr<Window> GetWindow(HWND hwnd);

    HWND RootWindowHandle() { return m_root; }

    bool ShouldQuit() { return m_shouldQuit; }

private:
    HINSTANCE m_hInstance;
    HWND m_root;
    std::shared_ptr<WindowsMessageCenter> m_messagePump;
    std::unordered_map<HWND, std::shared_ptr<Window>> m_windows;
    bool m_shouldQuit;

    OSWin(const OSWin& other) = delete;
    OSWin& operator=(const OSWin& other) = delete;
};

#endif
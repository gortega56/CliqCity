#include "stdafx.h"
#include "Window.h"
#include "Platform.h"

std::shared_ptr<Window> Window::Create(std::string caption, i32 width, i32 height, bool fullscreen)
{
    return Platform::CreateWindow0(caption, width, height, fullscreen);
}


Window::Window(int width, int height, bool fullscreen) : m_width(width), m_height(height), m_fullScreen(fullscreen)
{

}

Window::~Window()
{

}



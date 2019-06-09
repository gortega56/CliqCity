#pragma once
#ifndef PLATFORMIMGUI
#define PLATFORMIMGUI

namespace Platform
{
    typedef int8_t WindowHandle;

    LUZ_API bool Initialize_Imgui(const WindowHandle hWindow);

    LUZ_API void Update_Imgui();

    LUZ_API void Shutdown_Imgui();
}

#endif
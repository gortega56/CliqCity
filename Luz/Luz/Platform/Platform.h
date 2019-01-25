#pragma once
#ifndef PLATFORM_H
#define PLATFORM_H

#ifndef PLATFORMTYPES_H
#include "PlatformTypes.h"
#endif

#ifndef LUZEXPORT_H
#include "LuzExport.h"
#endif

#if _WIN64
#define WINXX
#elif _WIN32
#define _WINXX
#elif _APPLE_
#define OSX
#endif

namespace Luz
{
    struct Notification
    {
        enum class Identifier : uint64_t
        {
            NONE = 0
        };

        Identifier ID;

        Notification();
        Notification(Identifier id);
        ~Notification() = default;

        DEFAULT_COPY(Notification)
        DEFAULT_MOVE(Notification)
    };
}

class Window;

namespace Platform
{
    LUZ_API int Initialize(int n, ...);
    
    LUZ_API void Shutdown();

    LUZ_API void BeginUpdate(double time, double delta);
    
    LUZ_API void EndUpdate(double time, double delta);

    LUZ_API bool Running();

    LUZ_API std::shared_ptr<::Window> CreateWindow0(std::string caption, i32 width, i32 height, bool fullscreen);

    LUZ_API int CreateConsole();

    LUZ_API int ClearConsole();

    LUZ_API void DestroyConsole();

    LUZ_API ScreenPoint GetMousePosition();

    LUZ_API bool IsMouseActive();

    LUZ_API void SetMouseActive(bool active);

    LUZ_API bool GetKeyDown(KeyCode key);

    LUZ_API bool GetKeyUp(KeyCode key);

    LUZ_API bool GetKey(KeyCode key);

    LUZ_API float GetMouseWheelDelta();

    LUZ_API bool GetMouseButtonDown(MouseButton button);

    LUZ_API bool GetMouseButtonUp(MouseButton button);

    LUZ_API bool GetMouseButton(MouseButton button);

    LUZ_API bool GetGamepadButtonDown(GamepadButton buttonMask, bool isAdditive = true, short id = -1);

    LUZ_API bool GetGamepadButtonUp(GamepadButton buttonMask, bool isAdditive = true, short id = -1);

    LUZ_API bool GetGamepadButton(GamepadButton buttonMask, bool isAdditive = true, short id = -1);

    LUZ_API float GetGamepadAxis(GamepadAxis axis, short id);
}

#endif

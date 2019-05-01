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

    LUZ_API int CreateConsole();

    LUZ_API int ClearConsole();

    LUZ_API void DestroyConsole();
}

#endif

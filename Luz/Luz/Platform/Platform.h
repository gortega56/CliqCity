#pragma once
#ifndef PLATFORM_H
#define PLATFORM_H

#if _WIN64 || _WIN32
#include <Windows.h>
#elif __APPLE__
#endif

#ifndef LUZEXPORT_H
#include "LuzExport.h"
#endif

namespace Luz
{
    class Input;

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

class Platform
{
public:
    LUZ_API Platform();
    virtual LUZ_API ~Platform();

    virtual LUZ_API bool Initialize();
    virtual LUZ_API void BeginUpdate(double delta);
    virtual LUZ_API void EndUpdate(double delta);
    virtual LUZ_API void Shutdown();

    inline bool LUZ_API ShouldQuit() const { return m_shouldQuit; }

#if _WIN64 || _WIN32
    static std::shared_ptr<Platform> LUZ_API Create(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);
#elif __APPLE__
    static std::shared_ptr<Platform> LUZ_API Create(int argc, char* argv[]);
#endif

    std::shared_ptr<Luz::Input> LUZ_API GetInput();

protected:
    std::shared_ptr<Luz::Input> m_input;
    bool m_shouldQuit;

    NO_COPY(Platform)
    NO_MOVE(Platform)
};

#endif

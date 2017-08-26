#pragma once
#ifndef PLATFORM_H
#define PLATFORM_H

#if _WIN64 || _WIN32
#include <Windows.h>
#elif __APPLE__
#endif

class Platform
{
public:
    Platform() : m_shouldQuit(false) {};
    virtual ~Platform() {};

    virtual bool Initialize() { return false; }
    virtual void Update(double delta) {}
    virtual void Shutdown() {}

    inline bool ShouldQuit() { return m_shouldQuit; }

#if _WIN64 || _WIN32
    static std::shared_ptr<Platform> Create(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);
#elif __APPLE__
    static std::shared_ptr<Platform> Create(int argc, char* argv[]);
#endif

protected:
    bool m_shouldQuit;

    NO_COPY(Platform)
    NO_MOVE(Platform)
};

#endif

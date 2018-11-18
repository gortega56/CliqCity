#pragma once
#ifndef ENGINE_H
#define ENGINE_H

#ifndef LZASSERT_H
#include "LzAssert.h"
#endif

#ifndef TIMER_H
#include "Timer.h"
#endif

#ifndef IAPPLICATION_H
#include "IApplication.h"
#endif

#ifndef PLATFORM_H
#include "Platform/Platform.h"
#endif

namespace Engine
{
    template<class Application, class... Args>
    void Run(int n, Args&&... args)
    {
        Timer<seconds> timer;
        timer.Reset();

        int success = 0;
        success = Platform::Initialize(n, std::forward<Args>(args)...);
        LUZASSERT(success);

        Application app;
        success = app.Initialize();
        LUZASSERT(success);

        double t = 0.0;
        double dt = 0.01;
        double accumulator = 0.0;

        while (Platform::Running())
        {
            timer.Tick();

            double totalTime = timer.Total();
            double frameTime = timer.Delta();
            Platform::BeginUpdate(totalTime, frameTime);

            if (frameTime > 0.25)
            {
                frameTime = 0.25;
            }

            accumulator += frameTime;

            while (accumulator >= dt)
            {
                app.FixedUpdate(dt);
                t += dt;
                accumulator -= dt;
            }

            app.Update(dt);

            Platform::EndUpdate(totalTime, frameTime);
        }

        app.Shutdown();
        Platform::Shutdown();
    }
}

#define DECLARE_WINMAIN(Application)                                                                \
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)     \
{                                                                                                   \
    Engine::Run<Application>(4, hInstance, hPrevInstance, lpCmdLine, nShowCmd);                     \
    return 0;                                                                                       \
}

#define DECLARE_APPLEMAIN(Application)          \
int main(int argc, char* argv[])                \
{                                               \
    Engine::Run<Application>(2, argc, argv);    \
    return 0;                                   \
}

#endif
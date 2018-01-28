#pragma once
#ifndef ENGINE_H
#define ENGINE_H

#ifndef TIMER_H
#include "Timer.h"
#endif

#ifndef IAPPLICATION_H
#include "IApplication.h"
#endif

#ifndef PLATFORM_H
#include "Platform.h"
#endif

#include <memory>

template<class OpSys>
class TEngine : public std::enable_shared_from_this<TEngine<OpSys>>
{
public:

#if _WIN64 || _WIN32
    template<class Application> static void Run(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);
#elif __APPLE__
    template<class Application> static void Run(int argc, char* argv[]);
#endif

    TEngine() {}
    ~TEngine() {}

    bool Initialize();
    void BeginUpdate(double ft);
    void EndUpdate(double ft);
    void Shutdown();

    inline double Total()  { return m_timer.Total(); }
    inline double Delta() { return m_timer.Delta(); }
    inline std::shared_ptr<OpSys> OS() { return m_os; }

private:
    std::shared_ptr<OpSys> m_os;
    Timer<seconds> m_timer;

    template<class Application>
    static void Run(TEngine<OpSys>& engine);
};



#if _WIN64 || _WIN32
template<class OpSys>
template<class Application>
void TEngine<OpSys>::Run(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{

    TEngine<OpSys> engine;
    engine.m_os = std::static_pointer_cast<OpSys>(OpSys::Create(hInstance, hPrevInstance, lpCmdLine, nShowCmd));
    Run<Application>(engine);
}
#elif __APPLE__
template<class OpSys>
template<class Application>
void TEngine<OpSys>::Run(int argc, char* argv[])
{

}
#endif

template<class OpSys>
template<class Application>
void TEngine<OpSys>::Run(TEngine<OpSys>& engine)
{
    engine.Initialize();
    engine.m_timer.Reset();

    Application app(&engine);
    if (app.Initialize())
    {
        double t = 0.0;
        double dt = 0.01;

        double accumulator = 0.0;

        while (!engine.m_os->ShouldQuit())
        {
            engine.m_timer.Tick();

            double frameTime = engine.m_timer.Delta();
            engine.BeginUpdate(frameTime);

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

            engine.EndUpdate(frameTime);
        }
    }

    app.Shutdown();
    engine.Shutdown();
}

template<class OpSys>
bool TEngine<OpSys>::Initialize()
{
    if (m_os->Initialize())
    {
        return true;
    }

    return false;
}

template<class OpSys>
void TEngine<OpSys>::Shutdown()
{
    m_os->Shutdown();
}

template<class OpSys>
void TEngine<OpSys>::BeginUpdate(double ft)
{
    m_os->BeginUpdate(ft);
}

template<class OpSys>
void TEngine<OpSys>::EndUpdate(double ft)
{
    m_os->EndUpdate(ft);
}

#define DECLARE_WINMAIN(Application)                                                            \
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) \
{                                                                                               \
    Engine::Run<Application>(hInstance, hPrevInstance, lpCmdLine, nShowCmd);                    \
    return 0;                                                                                   \
}

#define DECLARE_APPLEMAIN(Application)      \
int main(int argc, char* argv[])            \
{                                           \
    Engine::Run<Application>(argc, argv);   \
    return 0;                               \
}


#if _WIN64 || _WIN32

#ifndef WINDOWSPLATFORM_H
#include "WindowsPlatform.h"
#endif

typedef TEngine<MS::Windows> Engine;

#elif __APPLE__

#endif

#endif
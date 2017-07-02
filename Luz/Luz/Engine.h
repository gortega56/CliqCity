#pragma once
#ifndef ENGINE_H
#define ENGINE_H

#ifndef TIMER_H
#include "Timer.h"
#endif

#ifndef IAPPLICATION_H
#include "IApplication.h"
#endif

#ifndef WINDOW_H
#include "Window.h"
#endif

#ifndef CONSOLE_H
#include "Console.h"
#endif

#include <memory>

struct Options;

template<class OpSys, class GFX>
class TEngine : public std::enable_shared_from_this<TEngine<OpSys, GFX>>
{
public:
    inline double Total()  { return m_timer.Total(); }
    inline double Delta() { return m_timer.Delta(); }

    TEngine() {}
    ~TEngine() {}

    template<class Application>
    inline static void Run(const Options& options)
    {
        TEngine<OpSys, GFX> engine;
        engine.Initialize(options);
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
                engine.Update(frameTime);

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
            }
        }
        
        app.Shutdown();
        engine.Shutdown();
    }

    int Initialize(const Options& options)
    {
        m_os = std::make_shared<OpSys>();
        m_os->Initialize(options);

        m_graphics = std::make_shared<GFX>(m_os.get(), 1, 3);
        m_graphics->Initialize(options.WindowWidth, options.WindowHeight, options.FullScreen);

        if (options.UseConsole)
        {
            Console::Initialize(&m_console);
        }

        return 1;
    }

    int Shutdown()
    {
        m_graphics->Shutdown();
        m_os->Shutdown();

        return 1;
    }

    void Update(double ft)
    {
        m_os->Update(ft);
    }

    std::shared_ptr<OpSys> OS() { return m_os; }
    std::shared_ptr<GFX> Graphics() { return m_graphics; }

private:
    std::shared_ptr<OpSys> m_os;
    std::shared_ptr<GFX> m_graphics;
    Timer<seconds> m_timer;

    Console m_console;
};

#ifdef _WIN64

#ifndef OSWIN_H
#include "OSWin.h"
#endif

#ifndef DX12RENDERER_H
#include "DirectX12\dx12_renderer.h"
#endif

typedef TEngine<OSWin, Dx12::Renderer> Engine;
typedef Dx12::Renderer Renderer;

#ifndef DX12_SHADER_H
#include "DirectX12\Shader.h"
#endif

typedef Dx12::Shader Shader;
typedef Dx12::InputLayout InputLayout;

#ifndef DX12_PIPELINESTATE_H
#include "DirectX12\PipelineState.h"
#endif

typedef Dx12::GraphicsPipeline GraphicsPipeline;

#ifndef DX12_ROOTSIGNATURE_H
#include "DirectX12\RootSignature.h"
#endif

typedef Dx12::RootSignature RootSignature;

#ifndef DX12_GPUSTATE_H
#include "DirectX12\GpuState.h"
#endif

typedef Dx12::RasterizerState RasterizerState;
typedef Dx12::DepthStencilState DepthStencilState;
typedef Dx12::BlendState BlendState;

#elif _WIN32

#ifndef OSWIN_H
#include "OSWin.h"
#endif

#ifndef DX12RENDERER_H
#include "DirectX12\dx12_renderer.h"
#endif

typedef TEngine<OSWin, Dx12::Renderer> Engine;

typedef Dx12::Renderer Renderer;

#ifndef DX12_SHADER_H
#include "DirectX12\Shader.h"
#endif

typedef Dx12::Shader Shader;
typedef Dx12::InputLayout InputLayout;

#ifndef DX12_PIPELINESTATE_H
#include "DirectX12\PipelineState.h"
#endif

typedef Dx12::GraphicsPipeline GraphicsPipeline;

#ifndef DX12_ROOTSIGNATURE_H
#include "DirectX12\RootSignature.h"
#endif

typedef Dx12::RootSignature RootSignature;

#ifndef DX12_GPUSTATE_H
#include "DirectX12\GpuState.h"
#endif

typedef Dx12::RasterizerState RasterizerState;
typedef Dx12::DepthStencilState DepthStencilState;
typedef Dx12::BlendState BlendState;

#elif __APPLE__

#endif

#endif
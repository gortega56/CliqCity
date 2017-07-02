#pragma once
#ifndef IAPPLICATION_H
#define IAPPLICATION_H

template<class OS, class GFX>
class TEngine;

#ifdef _WIN64

#ifndef OSWIN_H
#include "OSWin.h"
#endif

//#ifndef DX12RENDERER_H
//#include "Graphics\DirectX12\dx12_renderer.h"
//#endif

namespace Dx12
{
    class Renderer;
}

typedef TEngine<OSWin, Dx12::Renderer> Engine;

#elif _WIN32

#ifndef OSWIN_H
#include "OSWin.h"
#endif

#ifndef DX12RENDERER_H
#include "Graphics\DirectX12\dx12_renderer.h"
#endif

typedef TEngine<OSWin, Dx12::Renderer> Engine;

#elif __APPLE__
#endif

class IApplication
{
public:
    Engine* m_engine;

    IApplication() {};
    IApplication(Engine* engine) : m_engine(engine) {}
    virtual ~IApplication() { };

    virtual bool Initialize() = 0;
    virtual int Shutdown() = 0;

    virtual void Update(double delta) = 0;
    virtual void FixedUpdate(double delta) = 0;

private:

    IApplication(const IApplication& other) = delete;
    IApplication(const IApplication&& other) = delete;
};

#endif APPLICATION_H
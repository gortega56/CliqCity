#pragma once
#ifndef IAPPLICATION_H
#define IAPPLICATION_H

template<class OpSys>
class TEngine;

#if _WIN64 || _WIN32

#ifndef WINDOWSPLATFORM_H
#include "Platform\WindowsPlatform.h"
#endif

typedef TEngine<MS::Windows> Engine;

#elif __APPLE__
#endif

class LUZ_API IApplication
{
public:
    Engine* m_engine;

    IApplication();
    IApplication(Engine* engine);
    virtual ~IApplication();

    virtual bool Initialize() = 0;
    virtual int Shutdown() = 0;

    virtual void Update(double delta) = 0;
    virtual void FixedUpdate(double delta) = 0;

    NO_COPY(IApplication)
    NO_MOVE(IApplication)
};

#endif APPLICATION_H
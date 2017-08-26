#pragma once
#ifndef IAPPLICATION_H
#define IAPPLICATION_H

template<class OpSys>
class TEngine;

#if _WIN64 || _WIN32

#ifndef WINDOWSPLATFORM_H
#include "WindowsPlatform.h"
#endif

typedef TEngine<MS::Windows> Engine;

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
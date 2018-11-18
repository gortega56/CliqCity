#pragma once
#ifndef IAPPLICATION_H
#define IAPPLICATION_H

#ifndef LUZEXPORT_H
#include "LuzExport.h"
#endif

class LUZ_API IApplication
{
public:
    IApplication();
    virtual ~IApplication();

    virtual bool Initialize() = 0;
    virtual int Shutdown() = 0;

    virtual void Update(double delta) = 0;
    virtual void FixedUpdate(double delta) = 0;

    NO_COPY(IApplication)
    NO_MOVE(IApplication)
};

#endif APPLICATION_H
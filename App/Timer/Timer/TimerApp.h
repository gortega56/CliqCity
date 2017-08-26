#pragma once
#include <memory>
#ifndef GENERALMACROS_H
#include "GeneralMacros.h"
#endif

#ifndef IAPPLICATION_H
#include "IApplication.h"
#endif

#ifndef CONSOLE_H
#include "Console.h"
#endif

class TimerApp :
	public IApplication
{
public:
    using IApplication::IApplication;

	TimerApp();
	virtual ~TimerApp();

    bool Initialize() override;
    int Shutdown() override;

    void Update(double delta) override;
    void FixedUpdate(double delta) override;

    Console m_console;
};


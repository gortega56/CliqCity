#pragma once
#ifndef IAPPLICATION_H
#include "IApplication.h"
#endif

#ifndef LUZ_H
#include "Luz.h"
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
};


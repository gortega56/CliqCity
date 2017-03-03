#pragma once
#include "IApplication.h"

class TimerApp :
	public IApplication
{
public:
    using IApplication::IApplication;

	TimerApp();
	virtual ~TimerApp();

    int Initialize() override;
    int Shutdown() override;

    void Update(double delta) override;
    void FixedUpdate(double delta) override;
};


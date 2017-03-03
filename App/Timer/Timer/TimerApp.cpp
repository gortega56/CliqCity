#include "TimerApp.h"
#include "Engine.h"
#include <iostream>
#include "Renderer.h"

TimerApp::TimerApp()
{
    
}


TimerApp::~TimerApp()
{
}

int TimerApp::Initialize()
{
    std::cout << "Init" << std::endl;

    return 0;
}

int TimerApp::Shutdown()
{
    std::cout << "Shutdown" << std::endl;

    return 0;
}

void TimerApp::Update(double delta)
{
    double t = m_engine->Total();
    double dt = m_engine->Delta();

    std::cout << "Time: " << t << "\tDelta: " << dt << std::endl;

}

void TimerApp::FixedUpdate(double delta)
{

}
#include "stdafx.h"
#include "Platform.h"
#include "Input.h"

using namespace Luz;

Notification::Notification() : Notification(Identifier::NONE)
{

}

Notification::Notification(Identifier id) : ID(id)
{

}

Platform::Platform() : 
    m_input(std::make_shared<Input>()),
    m_shouldQuit(false)
{

}

Platform::~Platform()
{

}

bool Platform::Initialize() 
{ 
    m_input->Initialize(this);

    return true; 
}

void Platform::BeginUpdate(double delta)
{
}

void Platform::EndUpdate(double delta)
{
    m_input->Update(delta);
}

void Platform::Shutdown() 
{

}

std::shared_ptr<Luz::Input> Platform::GetInput()
{
    return m_input;
}
#if _WIN64 || _WIN32
#include "WindowsPlatform.h"

std::shared_ptr<Platform> Platform::Create(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    return std::make_shared<MS::Windows>(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
}

#elif __APPLE__
std::shared_ptr<Platform> Platform::Create(int argc, char* argv[])
{
    return nullptr;
}
#endif

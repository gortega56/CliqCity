#include "stdafx.h"
#include "Platform.h"

Platform::Platform() : m_shouldQuit(false)
{

}

Platform::~Platform()
{

}

bool Platform::Initialize() 
{ 
    return false; 
}

void Platform::Update(double delta)
{

}

void Platform::Shutdown() 
{

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

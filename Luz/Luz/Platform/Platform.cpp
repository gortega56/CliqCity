#include "stdafx.h"
#include "Platform.h"

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

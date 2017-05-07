#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif

#include <windows.h>
#include "MeshApplication.h"
#include "Engine.h"
#include "Options.h"

int WINAPI WinMain(HINSTANCE hInstance,    //Main windows function
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd)
{
    Options options;
    options.Win32.HInstance = hInstance;
    options.Win32.PrevHInstance = hPrevInstance;
    options.Win32.CommandLine = lpCmdLine;
    options.Win32.ShowCommand = nShowCmd;
    options.Caption = "Mesh Application";
    options.WindowWidth = 1600;
    options.WindowHeight = 900;
    options.FullScreen = false;
    options.UseConsole = true;
    
    Engine::Run<MeshApplication>(options);

    return 0;
}
#include "TimerApp.h"
#include <Windows.h>
#include "Options.h"
#include "Engine.h"
#include "Console.h"

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
	options.Caption = "Timer Application";
	options.WindowWidth = 1600;
	options.WindowHeight = 900;
	options.FullScreen = false;
    options.UseConsole = true;

    Engine::Run<TimerApp>(options);

	return 0;
}

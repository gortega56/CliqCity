#include "TimerApp.h"
#include <Windows.h>
#include "Engine.h"

#if _WIN64 || _WIN32
DECLARE_WINMAIN(TimerApp)
#elif __APPLE__
DECLARE_APPLEMAIN(TimerApp)
#endif

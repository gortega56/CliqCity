#include "SceneViewer.h"

#if _WIN64 || _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif

#define NOMINMAX
#include <Windows.h>

#include "Engine.h"
DECLARE_WINMAIN(SceneViewer)
#elif __APPLE__
DECLARE_APPLEMAIN(SceneViewer)
#endif
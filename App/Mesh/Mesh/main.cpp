#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif

#define NOMINMAX
#include "SceneViewer.h"
#include "Engine.h"

#if _WIN64 || _WIN32
#include <Windows.h>
DECLARE_WINMAIN(SceneViewer)
#elif __APPLE__
DECLARE_APPLEMAIN(MeshApplication)
#endif
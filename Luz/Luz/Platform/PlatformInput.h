#pragma once
#ifndef PLATFORMINPUT_H
#define PLATFORMINPUT_H

#ifndef PLATFORMTYPES_H
#include "PlatformTypes.h"
#endif

namespace Platform
{
	LUZ_API ScreenPoint GetMousePosition();

	LUZ_API void SetMousePosition(const int x, const int y);

	LUZ_API bool IsMouseActive();

	LUZ_API void SetMouseActive(bool active);

	LUZ_API bool GetKeyDown(KeyType key);

	LUZ_API bool GetKeyUp(KeyType key);

	LUZ_API bool GetKey(KeyType key);

	LUZ_API float GetMouseWheelDelta();

	LUZ_API bool GetMouseButtonDown(MouseButton button);

	LUZ_API bool GetMouseButtonUp(MouseButton button);

	LUZ_API bool GetMouseButton(MouseButton button);

	LUZ_API bool GetGamepadButtonDown(GamepadButton buttonMask, bool isAdditive = true, short id = -1);
	
	LUZ_API bool GetGamepadButtonUp(GamepadButton buttonMask, bool isAdditive = true, short id = -1);
	
	LUZ_API bool GetGamepadButton(GamepadButton buttonMask, bool isAdditive = true, short id = -1);
	
	LUZ_API float GetGamepadAxis(GamepadAxis axis, short id);

	void Initialize_Input();

	void Update_Input(double delta);
}

#endif

#pragma once
#ifndef INPUT_H
#define INPUT_H

#ifndef PLATFORMTYPES_H
#include "PlatformTypes.h"
#endif

#define PREV_KEY_STATE_BIT 0x40000000

class PlatformInput;

namespace Luz
{
    struct Notification;

	class Input
	{
	private:
        friend class PlatformInput;

		short	mCurrMouseState;
		short	mPrevMouseState;
		bool    mIsMouseActive;

		std::unordered_set<Platform::Input::KeyCode>	mKeysDown;
		std::unordered_set<Platform::Input::KeyCode>	mKeysUp;
		std::unordered_set<Platform::Input::KeyCode>	mKeysPressed;

	public:
		Platform::Input::ScreenPoint mousePosition;
		Platform::Input::ScreenPoint prevMousePosition;
		Platform::Input::ScreenPoint delta;

		Input();
		~Input();

		int Initialize();
		void Update(double delta);
		
        bool LUZ_API IsMouseActive() const { return mIsMouseActive; }
		void LUZ_API SetMouseActive(bool active) { mIsMouseActive = active; }

		bool LUZ_API GetKeyDown(Platform::Input::KeyCode key);
		bool LUZ_API GetKeyUp(Platform::Input::KeyCode key);
		bool LUZ_API GetKey(Platform::Input::KeyCode key);
		bool LUZ_API GetMouseButtonDown(Platform::Input::MouseButton button);
		bool LUZ_API GetMouseButtonUp(Platform::Input::MouseButton button);
		bool LUZ_API GetMouseButton(Platform::Input::MouseButton button);
		bool LUZ_API GetGamepadButtonDown(Platform::Input::GamepadButton buttonMask, bool isAdditive = true, short id = -1);
		bool LUZ_API GetGamepadButtonUp(Platform::Input::GamepadButton buttonMask, bool isAdditive = true, short id = -1);
		bool LUZ_API GetGamepadButton(Platform::Input::GamepadButton buttonMask, bool isAdditive = true, short id = -1);
		float LUZ_API GetGamepadAxis(Platform::Input::GamepadAxis axis, short id);

        void OnPlatformNotification(const Luz::Notification& notification);
	};
}

#endif

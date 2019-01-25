#pragma once
#ifndef INPUT_H
#define INPUT_H

#ifndef PLATFORMTYPES_H
#include "PlatformTypes.h"
#endif

#ifndef WINDOWSPLATFORMTYPES_H
#include "WindowsPlatformTypes.h"
#endif

struct WindowsMessage;

namespace Platform
{
	class Input
	{
	public:
		ScreenPoint mousePosition;
		ScreenPoint prevMousePosition;
		ScreenPoint delta;
        float mouseWheel;
        float prevMouseWheel;
        float mouseWheelDelta;

		Input();
		~Input();

		int Initialize();
		void Update(double delta);
		
        bool IsMouseActive() const { return mIsMouseActive; }
		void SetMouseActive(bool active) { mIsMouseActive = active; }

		bool GetKeyDown(KeyCode key);
		bool GetKeyUp(KeyCode key);
		bool GetKey(KeyCode key);
		bool GetMouseButtonDown(MouseButton button);
		bool GetMouseButtonUp(MouseButton button);
		bool GetMouseButton(MouseButton button);
		bool GetGamepadButtonDown(GamepadButton buttonMask, bool isAdditive = true, short id = -1);
		bool GetGamepadButtonUp(GamepadButton buttonMask, bool isAdditive = true, short id = -1);
		bool GetGamepadButton(GamepadButton buttonMask, bool isAdditive = true, short id = -1);
		float GetGamepadAxis(GamepadAxis axis, short id);
        
        void OnWindowsMessage(const WindowsMessage& wm);
        void OnKeyDown(WPARAM wparam, LPARAM lparam);
        void OnKeyUp(WPARAM wparam, LPARAM lparam);
        void OnMouseDown(MouseButton button, WPARAM wparam, LPARAM lparam);
        void OnMouseUp(MouseButton button, WPARAM wparam, LPARAM lparam);
        void OnMouseMove(WPARAM wparam, LPARAM lparam);
        void OnMouseWheel(WPARAM wparam, LPARAM lparam);

    private:
        short mCurrMouseState;
        short mPrevMouseState;
        bool mIsMouseActive;

        std::unordered_set<KeyCode>	mKeysDown;
        std::unordered_set<KeyCode>	mKeysUp;
        std::unordered_set<KeyCode>	mKeysPressed;

        XINPUT_STATE mCurrGamepadState[XUSER_MAX_COUNT];
        XINPUT_STATE mPrevGamepadState[XUSER_MAX_COUNT];
	};
}

#endif

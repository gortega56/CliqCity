#include "stdafx.h"
#include "PlatformInput.h"
#include "PlatformEvents.h"
#include "Platform.h"

#ifdef WINXX

#include "PlatformTypes_Win64.h"

#define PREV_KEY_STATE_BIT 0x40000000

namespace Platform
{
	typedef unsigned char KeyState;

	struct MouseState
	{
		ScreenPoint Position;
		float Wheel;
		short Button;
		bool bIsActive = true;
	};

	struct InputState
	{
		MouseState MouseState;
		KeyState pKeysDown[KEYTYPE_COUNT];
		KeyState pKeysUp[KEYTYPE_COUNT];
		XINPUT_STATE pGamepadState[XUSER_MAX_COUNT];
	};

	struct FrameState
	{
		static const unsigned int s_nInputStates = 2;
		unsigned int iFrame = s_nInputStates - 1;
		unsigned int nFrames = 0;
	};

	static InputState s_pInputStates[FrameState::s_nInputStates];

	static FrameState s_frameState;

	static inline unsigned int GetPreviousFrame()
	{
		return (s_frameState.nFrames - 1) % FrameState::s_nInputStates;
	}

	static void OnKeyDown(const Message message)
	{
		// if the previous state is set to 1
		// the keydown event is recurrent
		// and there is no need for processing it
		if (message.Keyboard.bRecurrent)
		{
			return;
		}

		KeyType eKey = message.Keyboard.eKey;

		s_pInputStates[s_frameState.iFrame].pKeysDown[eKey] = 1;
	}

	static void OnKeyUp(const Message message)
	{
		KeyType eKey = message.Keyboard.eKey;

		s_pInputStates[s_frameState.iFrame].pKeysDown[eKey] = 0;
		s_pInputStates[s_frameState.iFrame].pKeysUp[eKey] = 1;
	}

	static void OnMouseDown(MouseButton button, const Message message)
	{
		short mouseButtonState = 0;

		if (button == MOUSEBUTTON_X)
		{
			// GET_XBUTTON_WPARAM returns n for XBUTTON n
			short xButton = message.MouseClick.Code;

			// shift xButton to 4 bits to the left
			// before adding to curr state
			mouseButtonState |= xButton << 4;
		}
		else
		{
			mouseButtonState |= button;
		}

		s_pInputStates[s_frameState.iFrame].MouseState.Button = mouseButtonState;
	}

	static void OnMouseUp(MouseButton button, const Message message)
	{
		short mouseButtonState = 0;

		if (button == MOUSEBUTTON_X)
		{
			// GET_XBUTTON_WPARAM returns n for XBUTTON n
			short xButton = message.MouseClick.Code;

			// shift xButton to 4 bits to the left
			// before adding to curr state
			mouseButtonState &= ~(xButton << 4);
		}
		else
		{
			mouseButtonState &= ~button;
		}

		s_pInputStates[s_frameState.iFrame].MouseState.Button = mouseButtonState;
	}

	static void OnMouseMove(const Message message)
	{
		s_pInputStates[s_frameState.iFrame].MouseState.Position.x = message.MouseMove.PositionX;
		s_pInputStates[s_frameState.iFrame].MouseState.Position.y = message.MouseMove.PositionY;
	}

	static void OnMouseWheel(const Message message)
	{
		s_pInputStates[s_frameState.iFrame].MouseState.Wheel += message.MouseWheel.Delta;
	}

	static void OnEventMessage(const Message& message)
	{
		switch (message.eEventType)
		{
		case PLATFORM_EVENT_TYPE_MOUSE_LBUTTONDOWN:
			OnMouseDown(MOUSEBUTTON_LEFT, message);
			break;
		case PLATFORM_EVENT_TYPE_MOUSE_MBUTTONDOWN:
			OnMouseDown(MOUSEBUTTON_MIDDLE, message);
			break;
		case PLATFORM_EVENT_TYPE_MOUSE_RBUTTONDOWN:
			OnMouseDown(MOUSEBUTTON_RIGHT, message);
			break;
		case PLATFORM_EVENT_TYPE_MOUSE_XBUTTONDOWN:
			OnMouseDown(MOUSEBUTTON_X, message);
			break;
		case PLATFORM_EVENT_TYPE_MOUSE_LBUTTONUP:
			OnMouseUp(MOUSEBUTTON_LEFT, message);
			break;
		case PLATFORM_EVENT_TYPE_MOUSE_MBUTTONUP:
			OnMouseUp(MOUSEBUTTON_MIDDLE, message);
			break;
		case PLATFORM_EVENT_TYPE_MOUSE_RBUTTONUP:
			OnMouseUp(MOUSEBUTTON_RIGHT, message);
			break;
		case PLATFORM_EVENT_TYPE_MOUSE_XBUTTONUP:
			OnMouseUp(MOUSEBUTTON_X, message);
			break;
		case PLATFORM_EVENT_TYPE_MOUSE_MOVE:
			OnMouseMove(message);
			break;
		case PLATFORM_EVENT_TYPE_MOUSE_WHEEL:
			OnMouseWheel(message);
			break;
		case PLATFORM_EVENT_TYPE_KEYBOARD_KEYDOWN:
		case PLATFORM_EVENT_TYPE_KEYBOARD_SYSKEYDOWN: // key down if alt key is presed
			OnKeyDown(message);
			break;
		case PLATFORM_EVENT_TYPE_KEYBOARD_KEYUP:
		case PLATFORM_EVENT_TYPE_KEYBOARD_SYSKEYUP: // key up if alt key is pressed
			OnKeyUp(message);
			break;
		default:
			break;
		}
	}

	ScreenPoint GetMousePosition()
	{
		return s_pInputStates[s_frameState.iFrame].MouseState.Position;
	}

	void SetMousePosition(const int x, const int y)
	{
		::SetCursorPos(x, y);
	}

	bool IsMouseActive()
	{
		return s_pInputStates[s_frameState.iFrame].MouseState.bIsActive;
	}

	void SetMouseActive(bool active)
	{
		s_pInputStates[s_frameState.iFrame].MouseState.bIsActive = active;
	}

	bool GetKeyDown(KeyType eKey)
	{
		return s_pInputStates[s_frameState.iFrame].pKeysDown[eKey];
	}

	bool GetKeyUp(KeyType eKey)
	{
		return s_pInputStates[s_frameState.iFrame].pKeysUp[eKey];
	}

	bool GetKey(KeyType eKey)
	{
		bool wasDown = s_pInputStates[GetPreviousFrame()].pKeysDown[eKey];
		bool isDown = s_pInputStates[s_frameState.iFrame].pKeysDown[eKey];

		return isDown || wasDown;
	}

	float GetMouseWheelDelta()
	{
		return (s_pInputStates[s_frameState.iFrame].MouseState.Wheel - s_pInputStates[GetPreviousFrame()].MouseState.Wheel) / (float)WHEEL_DELTA;
	}

	bool GetMouseButtonDown(MouseButton button)
	{
		MouseState* pCurr = &s_pInputStates[s_frameState.iFrame].MouseState;
		if (!pCurr->bIsActive)
		{
			return false;
		}

		MouseState* pPrev = &s_pInputStates[GetPreviousFrame()].MouseState;
		bool wasNotPressed = (pPrev->Button & button) == 0;
		bool isPressed = (pCurr->Button & button) == button;

		return wasNotPressed && isPressed;
	}

	bool GetMouseButtonUp(MouseButton button)
	{
		MouseState* pCurr = &s_pInputStates[s_frameState.iFrame].MouseState;
		if (!pCurr->bIsActive)
		{
			return false;
		}

		MouseState* pPrev = &s_pInputStates[GetPreviousFrame()].MouseState;
		bool wasPressed = (pPrev->Button & button) == button;
		bool isNotPressed = (pCurr->Button & button) == 0;

		return wasPressed && isNotPressed;
	}

	bool GetMouseButton(MouseButton button)
	{
		MouseState* pCurr = &s_pInputStates[s_frameState.iFrame].MouseState;
		if (!pCurr->bIsActive)
		{
			return false;
		}

		bool isPressed = (pCurr->Button & button) == button;

		return isPressed;
	}

	bool GetGamepadButtonDown(GamepadButton buttonMask, bool isAdditive, short id)
	{
		bool justPressed = false;
		if (id == -1)
		{
			for (int i = 0; i < XUSER_MAX_COUNT; i++)
			{
				justPressed |= GetGamepadButtonDown(buttonMask, isAdditive, i);
			}

			return justPressed;
		}

		auto currGamepad = s_pInputStates[s_frameState.iFrame].pGamepadState[id].Gamepad;
		auto prevGamepad = s_pInputStates[GetPreviousFrame()].pGamepadState[id].Gamepad;

		auto totalKeys = 0;
		auto pressedKeys = 0;

		// if is additive, we stop as soon as a button in the mask is was not pressed this frame
		// otherwise we return true.
		// if is not additive, we stop as soon as a button in the mas was pressed this frame
		// otherwise we return false.
		for (int button = GAMEPADBUTTON_UP; button <= GAMEPADBUTTON_Y; button <<= 1)
		{
			// button not in flag
			if ((button & buttonMask) == 0)
			{
				continue;
			}

			auto wasButtonPressed = (prevGamepad.wButtons & button) == button;
			auto isButtonPressed = (currGamepad.wButtons & button) == button;

			justPressed |= !wasButtonPressed && isButtonPressed;

			// if additive, return true if any button from the mask was just pressed
			if (isAdditive && justPressed)
			{
				return true;
			}

			// if not additive, accumulate counters
			if (!isAdditive)
			{
				totalKeys += 1;
				if (isButtonPressed)
				{
					pressedKeys += 1;
				}
			}
		}

		// if is addictive, return true if all keys are pressed and one of the keys was just pressed
		if (!isAdditive)
		{
			return justPressed && pressedKeys == totalKeys;
		}

		// otherwise [if addictive] return false
		return false;
	}

	bool GetGamepadButtonUp(GamepadButton buttonMask, bool isAdditive, short id)
	{
		bool justReleased = false;
		if (id == -1)
		{
			for (int i = 0; i < XUSER_MAX_COUNT; i++)
			{
				justReleased |= GetGamepadButtonUp(buttonMask, isAdditive, i);
			}

			return justReleased;
		}

		auto currGamepad = s_pInputStates[s_frameState.iFrame].pGamepadState[id].Gamepad;
		auto prevGamepad = s_pInputStates[GetPreviousFrame()].pGamepadState[id].Gamepad;

		auto totalKeys = 0;
		auto pressedKeys = 0;

		// if is additive, we stop as soon as a button in the mask is was not pressed this frame
		// otherwise we return true.
		// if is not additive, we stop as soon as a button in the mas was pressed this frame
		// otherwise we return false.
		for (int button = GAMEPADBUTTON_UP; button <= GAMEPADBUTTON_Y; button <<= 1)
		{
			// button not in flag
			if ((button & buttonMask) == 0)
			{
				continue;
			}

			auto wasButtonPressed = (prevGamepad.wButtons & button) == button;
			auto isButtonPressed = (currGamepad.wButtons & button) == button;

			justReleased |= wasButtonPressed && !isButtonPressed;

			// if additive, return true if any button from the mask was just pressed
			if (isAdditive && justReleased)
			{
				return true;
			}

			// if not additive, accumulate counters
			if (!isAdditive)
			{
				totalKeys += 1;
				if (wasButtonPressed)
				{
					pressedKeys += 1;
				}
			}
		}

		// if is addictive, return true if all keys are pressed and one of the keys was just released
		if (!isAdditive)
		{
			return justReleased && totalKeys == pressedKeys;
		}

		// otherwise [if addictive] return false
		return false;
	}

	bool GetGamepadButton(GamepadButton buttonMask, bool isAdditive, short id)
	{
		bool isPressed = false;
		if (id == -1)
		{
			for (int i = 0; i < XUSER_MAX_COUNT; i++)
			{
				isPressed |= GetGamepadButton(buttonMask, isAdditive, i);
			}

			return isPressed;
		}

		auto gamepad = s_pInputStates[s_frameState.iFrame].pGamepadState[id].Gamepad;

		// if is not additive, we stop as soon as a button in the mask is not pressed
		// otherwise we return true.
		// if is additive, we stop as soon as a button in the mas IS pressed
		// otherwise we return false.
		for (int button = GAMEPADBUTTON_UP; button <= GAMEPADBUTTON_Y; button <<= 1)
		{
			// button not in flag
			if ((button & buttonMask) == 0)
			{
				continue;
			}

			// button is pressed
			auto isButtonPressed = (gamepad.wButtons & button) == button;

			if (!isButtonPressed && !isAdditive)
			{
				//char str[100];
				//sprintf_s(str, "not additive %x %x\n", gamepad.wButtons, button);
				//OutputDebugStringA(str);

				return false;
			}

			if (isButtonPressed && isAdditive)
			{
				return true;
			}
		}

		// if it is additive and no button was pressed return false,
		// if it is not additive and all buttons were pressed return true
		return !isAdditive;
	}

	float GetGamepadAxis(GamepadAxis axis, short id)
	{
		float rawValue;
		float deadzone;
		float rawLimit;

		auto gamepad = s_pInputStates[s_frameState.iFrame].pGamepadState[id].Gamepad;
		switch (axis)
		{
		case GAMEPADAXIS_LEFT_THUMB_X:
			rawValue = gamepad.sThumbLX;
			deadzone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
			rawLimit = 32767;
			break;
		case GAMEPADAXIS_LEFT_THUMB_Y:
			rawValue = gamepad.sThumbLY;
			deadzone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
			rawLimit = 32767;
			break;
		case GAMEPADAXIS_RIGHT_THUMB_X:
			rawValue = gamepad.sThumbRX;
			deadzone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
			rawLimit = 32767;
			break;
		case GAMEPADAXIS_RIGHT_THUMB_Y:
			rawValue = gamepad.sThumbRY;
			deadzone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
			rawLimit = 32767;
			break;
		case GAMEPADAXIS_LEFT_TRIGGER:
			rawValue = gamepad.bLeftTrigger;
			deadzone = XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
			rawLimit = 255;
			break;
		case GAMEPADAXIS_RIGHT_TRIGGER:
			rawValue = gamepad.bRightTrigger;
			deadzone = XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
			rawLimit = 255;
			break;
		default:
			return 0;
		}

		float sign = rawValue > 0 ? 1.0f : -1.0f;
		float magnitude = rawValue * sign;

		//check if the controller is outside a circular dead zone
		if (magnitude < deadzone)
		{
			return 0;
		}

		//clip the magnitude at its expected maximum value
		if (magnitude > rawLimit) rawValue = rawLimit * sign;

		//adjust magnitude relative to the end of the dead zone
		rawValue -= deadzone * sign;

		//giving a magnitude value of 0.0 to 1.0
		return rawValue / (rawLimit - deadzone);
	}

	void Initialize_Input()
	{
		// Mouse Down
		GetEvent(PLATFORM_EVENT_TYPE_MOUSE_LBUTTONDOWN)->Bind<Platform::OnEventMessage>();
		GetEvent(PLATFORM_EVENT_TYPE_MOUSE_MBUTTONDOWN)->Bind<Platform::OnEventMessage>();
		GetEvent(PLATFORM_EVENT_TYPE_MOUSE_RBUTTONDOWN)->Bind<Platform::OnEventMessage>();
		GetEvent(PLATFORM_EVENT_TYPE_MOUSE_XBUTTONDOWN)->Bind<Platform::OnEventMessage>();

		// Mouse Up
		GetEvent(PLATFORM_EVENT_TYPE_MOUSE_LBUTTONUP)->Bind<Platform::OnEventMessage>();
		GetEvent(PLATFORM_EVENT_TYPE_MOUSE_MBUTTONUP)->Bind<Platform::OnEventMessage>();
		GetEvent(PLATFORM_EVENT_TYPE_MOUSE_RBUTTONUP)->Bind<Platform::OnEventMessage>();
		GetEvent(PLATFORM_EVENT_TYPE_MOUSE_XBUTTONUP)->Bind<Platform::OnEventMessage>();

		// Mouse Move
		GetEvent(PLATFORM_EVENT_TYPE_MOUSE_MOVE)->Bind<Platform::OnEventMessage>();

		// Mouse Wheel
		GetEvent(PLATFORM_EVENT_TYPE_MOUSE_WHEEL)->Bind<Platform::OnEventMessage>();

		// Key Down
		GetEvent(PLATFORM_EVENT_TYPE_KEYBOARD_KEYDOWN)->Bind<Platform::OnEventMessage>();
		GetEvent(PLATFORM_EVENT_TYPE_KEYBOARD_SYSKEYDOWN)->Bind<Platform::OnEventMessage>();

		// Key Up
		GetEvent(PLATFORM_EVENT_TYPE_KEYBOARD_KEYUP)->Bind<Platform::OnEventMessage>();
		GetEvent(PLATFORM_EVENT_TYPE_KEYBOARD_SYSKEYUP)->Bind<Platform::OnEventMessage>();
	}

	void Update_Input(double delta)
	{
		// At this point the app has queried any input from 
		// the current frame we need to prepare for the next
		// frame by caching any relevant data
		unsigned int curr = s_frameState.iFrame;

		// Flip for next frame
		s_frameState.nFrames += 1;

		s_frameState.iFrame = s_frameState.nFrames % FrameState::s_nInputStates;

		unsigned int next = s_frameState.iFrame;

		// Copy Mouse State
		memcpy_s(&s_pInputStates[next].MouseState, sizeof(MouseState), &s_pInputStates[curr].MouseState, sizeof(MouseState));

		// Copy Keys Down
		memcpy_s(&s_pInputStates[next].pKeysDown, sizeof(KeyState) * KEYTYPE_COUNT, &s_pInputStates[curr].pKeysDown, sizeof(KeyState) * KEYTYPE_COUNT);

		// Flush Keys Up
		memset(&s_pInputStates[next].pKeysUp, 0, sizeof(KeyState) * KEYTYPE_COUNT);

		for (auto i = 0; i < XUSER_MAX_COUNT; i++)
		{
			s_pInputStates[next].pGamepadState[i] = s_pInputStates[curr].pGamepadState[i];

			ZeroMemory(&s_pInputStates[next].pGamepadState[i], sizeof(XINPUT_STATE));

			// Simply get the state of the controller from XInput.
			auto dwResult = XInputGetState(i, &s_pInputStates[next].pGamepadState[i]);

			continue;

			// CHECK IF CONTROLLER IS CONNECTED
			if (dwResult == ERROR_SUCCESS)
			{
				// Controller is connected 
			}
			else
			{
				// Controller is not connected 
			}
		}
	}
}

#endif
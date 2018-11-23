#include "stdafx.h"
#include "Input.h"
#include "Platform.h"

#include "WindowsPlatformTypes.h"
#include "WindowsMessageCenter.h"

#define PREV_KEY_STATE_BIT 0x40000000

namespace Platform
{

#pragma region Event Handler

    void Input::OnWindowsMessage(const WindowsMessage& wm)
    {
        switch (wm.msg)
        {
        case WM_LBUTTONDOWN:
            OnMouseDown(MOUSEBUTTON_LEFT, wm.wparam, wm.lparam);
            break;
        case WM_MBUTTONDOWN:
            OnMouseDown(MOUSEBUTTON_MIDDLE, wm.wparam, wm.lparam);
            break;
        case WM_RBUTTONDOWN:
            OnMouseDown(MOUSEBUTTON_RIGHT, wm.wparam, wm.lparam);
            break;
        case WM_XBUTTONDOWN:
            OnMouseDown(MOUSEBUTTON_X, wm.wparam, wm.lparam);
            break;
        case WM_LBUTTONUP:
            OnMouseUp(MOUSEBUTTON_LEFT, wm.wparam, wm.lparam);
            break;
        case WM_MBUTTONUP:
            OnMouseUp(MOUSEBUTTON_MIDDLE, wm.wparam, wm.lparam);
            break;
        case WM_RBUTTONUP:
            OnMouseUp(MOUSEBUTTON_RIGHT, wm.wparam, wm.lparam);
            break;
        case WM_XBUTTONUP:
            OnMouseUp(MOUSEBUTTON_X, wm.wparam, wm.lparam);
            break;
        case WM_MOUSEMOVE:
            OnMouseMove(wm.wparam, wm.lparam);
            break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN: // key down if alt key is presed
            OnKeyDown(wm.wparam, wm.lparam);
            break;
        case WM_KEYUP:
        case WM_SYSKEYUP: // key up if alt key is pressed
            OnKeyUp(wm.wparam, wm.lparam);
            break;
        default:
            break;
        }
    }

#pragma endregion

    static KeyCode KeyCodeFromWParam(WPARAM wParam);

    KeyCode KeyCodeFromWParam(WPARAM wParam)
    {
        // any exception from wParam value to keyCode value
        // must be done here
        //switch (wParam)
        //{
        //default:
        //	return static_cast<KeyCode>(wParam);
        //}

        return static_cast<KeyCode>(wParam);
    }

    void Input::OnKeyDown(WPARAM wParam, LPARAM lParam)
    {
        // if the previous state is set to 1
        // the keydown event is recurrent
        // and there is no need for processing it
        if (lParam & PREV_KEY_STATE_BIT)
            return;

        KeyCode code = KeyCodeFromWParam(wParam);

        mKeysDown.insert(code);
    }

    void Input::OnKeyUp(WPARAM wParam, LPARAM lParam)
    {
        KeyCode code = KeyCodeFromWParam(wParam);

        // remove the key from pressed keys
        mKeysPressed.erase(code);

        // and add it to up keys
        mKeysUp.insert(code);
    }

    void Input::OnMouseDown(MouseButton button, WPARAM wParam, LPARAM lParam)
    {
        if (button == MOUSEBUTTON_X)
        {
            // GET_XBUTTON_WPARAM returns n for XBUTTON n
            short xButton = GET_XBUTTON_WPARAM(wParam);

            // shift xButton to 4 bits to the left
            // before adding to curr state
            mCurrMouseState |= xButton << 4;
        }
        else
        {
            mCurrMouseState |= button;
        }
    }

    void Input::OnMouseUp(MouseButton button, WPARAM wParam, LPARAM lParam)
    {
        if (button == MOUSEBUTTON_X)
        {
            // GET_XBUTTON_WPARAM returns n for XBUTTON n
            short xButton = GET_XBUTTON_WPARAM(wParam);

            // shift xButton to 4 bits to the left
            // before adding to curr state
            mCurrMouseState &= ~(xButton << 4);
        }
        else
        {
            mCurrMouseState &= ~button;
        }
    }

    void Input::OnMouseMove(WPARAM wParam, LPARAM lParam)
    {
        mousePosition.x = GET_X_LPARAM(lParam);
        mousePosition.y = GET_Y_LPARAM(lParam);
    }

    Input::Input()
        : mCurrMouseState(0)
        , mPrevMouseState(0)
        , mIsMouseActive(true)
    {

    }

    Input::~Input()
    {

    }

    int Input::Initialize()
    {
        auto messageCenter = &Platform::s_messageBus;

        // Mouse Down
        messageCenter->GetEvent(WM_LBUTTONDOWN)->Bind<Input, &Input::OnWindowsMessage>(this);
        messageCenter->GetEvent(WM_MBUTTONDOWN)->Bind<Input, &Input::OnWindowsMessage>(this);
        messageCenter->GetEvent(WM_RBUTTONDOWN)->Bind<Input, &Input::OnWindowsMessage>(this);
        messageCenter->GetEvent(WM_XBUTTONDOWN)->Bind<Input, &Input::OnWindowsMessage>(this);

        // Mouse Up
        messageCenter->GetEvent(WM_LBUTTONUP)->Bind<Input, &Input::OnWindowsMessage>(this);
        messageCenter->GetEvent(WM_MBUTTONUP)->Bind<Input, &Input::OnWindowsMessage>(this);
        messageCenter->GetEvent(WM_RBUTTONUP)->Bind<Input, &Input::OnWindowsMessage>(this);
        messageCenter->GetEvent(WM_XBUTTONUP)->Bind<Input, &Input::OnWindowsMessage>(this);

        // Mouse Move
        messageCenter->GetEvent(WM_MOUSEMOVE)->Bind<Input, &Input::OnWindowsMessage>(this);

        // Key Down
        messageCenter->GetEvent(WM_KEYDOWN)->Bind<Input, &Input::OnWindowsMessage>(this);
        messageCenter->GetEvent(WM_SYSKEYDOWN)->Bind<Input, &Input::OnWindowsMessage>(this);

        // Key Up
        messageCenter->GetEvent(WM_KEYUP)->Bind<Input, &Input::OnWindowsMessage>(this);
        messageCenter->GetEvent(WM_SYSKEYUP)->Bind<Input, &Input::OnWindowsMessage>(this);

        return 1;
    }

#pragma region Input Checkers

    bool Input::GetKeyDown(KeyCode key)
    {
        return mKeysDown.find(key) != mKeysDown.end();
    }

    bool Input::GetKey(KeyCode key)
    {
        return GetKeyDown(key) || mKeysPressed.find(key) != mKeysPressed.end();
    }

    bool Input::GetKeyUp(KeyCode key)
    {
        return mKeysUp.find(key) != mKeysUp.end();
    }

    bool Input::GetMouseButtonDown(MouseButton button)
    {
        if (!mIsMouseActive)
        {
            return false;
        }

        bool wasNotPressed = (mPrevMouseState & button) == 0;
        bool isPressed = (mCurrMouseState & button) == button;

        return wasNotPressed && isPressed;
    }

    bool Input::GetMouseButtonUp(MouseButton button)
    {
        if (!mIsMouseActive)
        {
            return false;
        }

        bool wasPressed = (mPrevMouseState & button) == button;
        bool isNotPressed = (mCurrMouseState & button) == 0;

        return wasPressed && isNotPressed;
    }

    bool Input::GetMouseButton(MouseButton button)
    {
        if (!mIsMouseActive)
        {
            return false;
        }

        bool isPressed = (mCurrMouseState & button) == button;

        return isPressed;
    }

    bool Input::GetGamepadButtonDown(GamepadButton buttonMask, bool isAdditive, short id)
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

        auto currGamepad = mCurrGamepadState[id].Gamepad;
        auto prevGamepad = mPrevGamepadState[id].Gamepad;

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

    bool Input::GetGamepadButtonUp(GamepadButton buttonMask, bool isAdditive, short id)
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

        auto currGamepad = mCurrGamepadState[id].Gamepad;
        auto prevGamepad = mPrevGamepadState[id].Gamepad;

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

    bool Input::GetGamepadButton(GamepadButton buttonMask, bool isAdditive, short id)
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

        auto gamepad = mCurrGamepadState[id].Gamepad;

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

    float Input::GetGamepadAxis(GamepadAxis axis, short id)
    {
        float rawValue;
        float deadzone;
        float rawLimit;

        auto gamepad = mCurrGamepadState[id].Gamepad;
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

#pragma endregion

#pragma region Input Processing

    void Input::Update(double deltaSeconds)
    {
        // persist all keys pressed at this frame.
        for (const KeyCode &key : mKeysDown)
        {
            mKeysPressed.insert(key);
        }

        // clear keys down and up
        mKeysDown.clear();
        mKeysUp.clear();

        // copy current mouse state to previous mouse state
        delta.x = mousePosition.x - prevMousePosition.x;
        delta.y = mousePosition.y - prevMousePosition.y;
        prevMousePosition = mousePosition;
        mPrevMouseState = mCurrMouseState;

        // copy current joypad state
        for (auto i = 0; i < XUSER_MAX_COUNT; i++)
        {
            mPrevGamepadState[i] = mCurrGamepadState[i];

            ZeroMemory(&mCurrGamepadState[i], sizeof(XINPUT_STATE));

            // Simply get the state of the controller from XInput.
            auto dwResult = XInputGetState(i, &mCurrGamepadState[i]);

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

#pragma endregion
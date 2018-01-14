#pragma once
#include <xinput.h>

#define PREV_KEY_STATE_BIT 0x40000000

class Platform;

namespace Luz
{
	enum KeyCode;
	enum MouseButton;
	enum GamepadButton;
	enum GamepadAxis;

    struct Notification;

	struct LUZ_API ScreenPoint
	{
		int16_t x;
		int16_t y;

		ScreenPoint() {}
		ScreenPoint(int16_t _x, int16_t _y) : x(_x), y(_y) {}
		explicit ScreenPoint(_In_reads_(2) const int16_t *pArray) : x(pArray[0]), y(pArray[1]) {}

		ScreenPoint& operator= (const ScreenPoint& Int2) { x = Int2.x; y = Int2.y; return *this; }
	};

	class LUZ_API Input
	{
	private:
		short	mCurrMouseState;
		short	mPrevMouseState;
		bool    mIsMouseActive;

		XINPUT_STATE mCurrGamepadState[XUSER_MAX_COUNT];
		XINPUT_STATE mPrevGamepadState[XUSER_MAX_COUNT];

		std::unordered_set<KeyCode>	mKeysDown;
		std::unordered_set<KeyCode>	mKeysUp;
		std::unordered_set<KeyCode>	mKeysPressed;

		static KeyCode KeyCodeFromWParam(WPARAM wParam);

		void OnKeyDown(WPARAM wParam, LPARAM lParam);
		void OnKeyUp(WPARAM wParam, LPARAM lParam);
		void OnMouseDown(MouseButton button, WPARAM wParam, LPARAM lParam);
		void OnMouseUp(MouseButton button, WPARAM wParam, LPARAM lParam);
		void OnMouseMove(WPARAM wParam, LPARAM lParam);

	protected:

		int Initialize(std::shared_ptr<Platform> pPlatform);
		void Flush();

		Input();
		~Input();

	public:
		ScreenPoint mousePosition;
		ScreenPoint prevMousePosition;
		ScreenPoint delta;

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

        void OnPlatformNotification(const Luz::Notification& notification);
	};


	enum LUZ_API GamepadButton : int
	{
		GAMEPADBUTTON_UP = 0x0001,
		GAMEPADBUTTON_DOWN = 0x0002,
		GAMEPADBUTTON_LEFT = 0x0004,
		GAMEPADBUTTON_RIGHT = 0x0008,
		GAMEPADBUTTON_START = 0x0010,
		GAMEPADBUTTON_BACK = 0x0020,
		GAMEPADBUTTON_LEFT_THUMB = 0x0040,
		GAMEPADBUTTON_RIGHT_THUMB = 0x0080,
		GAMEPADBUTTON_LEFT_SHOULDER = 0x0100,
		GAMEPADBUTTON_RIGHT_SHOULDER = 0x0200,
		GAMEPADBUTTON_A = 0x1000,
		GAMEPADBUTTON_B = 0x2000,
		GAMEPADBUTTON_X = 0x4000,
		GAMEPADBUTTON_Y = 0x8000,
	};

	inline GamepadButton operator|(GamepadButton a, GamepadButton b)
	{
		return static_cast<GamepadButton>(static_cast<int>(a) | static_cast<int>(b));
	}

	enum LUZ_API GamepadAxis
	{
		GAMEPADAXIS_LEFT_THUMB_X = 0x01,
		GAMEPADAXIS_LEFT_THUMB_Y = 0x02,
		GAMEPADAXIS_RIGHT_THUMB_X = 0x04,
		GAMEPADAXIS_RIGHT_THUMB_Y = 0x08,
		GAMEPADAXIS_LEFT_TRIGGER = 0x10,
		GAMEPADAXIS_RIGHT_TRIGGER = 0x20,
	};

#pragma region Mouse Buttons

	enum LUZ_API MouseButton
	{
		MOUSEBUTTON_LEFT = 0x01,
		MOUSEBUTTON_RIGHT = 0x02,
		MOUSEBUTTON_MIDDLE = 0x04,

		MOUSEBUTTON_X = 0x10,
		MOUSEBUTTON_X1 = 0x20,
		MOUSEBUTTON_X2 = 0x40,
	};

#pragma endregion

#pragma region Key Codes

	// Key codes are based on msdn virtual-key codes
	// https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396

	// TODO: clean keycodes that aren't used
	enum LUZ_API KeyCode {
		//KEYCODE_LBUTTON = 0x01, // Left mouse button
		//KEYCODE_RBUTTON = 0x02, // Right mouse button
		//KEYCODE_CANCEL = 0x03, // Control-break processing
		//KEYCODE_MBUTTON = 0x04, // Middle mouse button (three-button mouse)
		//KEYCODE_XBUTTON1 = 0x05, // X1 mouse button
		//KEYCODE_XBUTTON2 = 0x06, // X2 mouse button

		// 0x07 -> Undefined

		KEYCODE_BACK = 0x08, // BACKSPACE key
		KEYCODE_TAB = 0x09, // TAB key

							// 0x07 -> Reserved

		KEYCODE_CLEAR = 0x0C, // CLEAR key
		KEYCODE_RETURN = 0x0D, // ENTER key

							   // 0x0E-0F -> Undefined 

		KEYCODE_SHIFT = 0x10, // SHIFT key
		KEYCODE_CONTROL = 0x11, // CTRL key
		KEYCODE_MENU = 0x12, // ALT key
		KEYCODE_PAUSE = 0x13, // PAUSE key
		KEYCODE_CAPITAL = 0x14, // CAPS LOCK key
		KEYCODE_KANA = 0x15, // IME Kana mode
		KEYCODE_HANGUEL = 0x15, // IME Hanguel mode (maintained for compatibility; use VK_HANGUL)
		KEYCODE_HANGUL = 0x15, // IME Hangul mode

							   // 0x16 -> Undefined

		KEYCODE_JUNJA = 0x17, // IME Junja mode
		KEYCODE_FINAL = 0x18, // IME final mode
		KEYCODE_HANJA = 0x19, // IME Hanja mode
		KEYCODE_KANJI = 0x19, // IME Kanji mode

							  // 0x1A -> Undefined

		KEYCODE_ESCAPE = 0x1B, // ESC key
		KEYCODE_CONVERT = 0x1C, // IME convert
		KEYCODE_NONCONVERT = 0x1D, // IME nonconvert
		KEYCODE_ACCEPT = 0x1E, // IME accept
		KEYCODE_MODECHANGE = 0x1F, // IME mode change request
		KEYCODE_SPACE = 0x20, // SPACEBAR
		KEYCODE_PRIOR = 0x21, // PAGE UP key
		KEYCODE_NEXT = 0x22, // PAGE DOWN key
		KEYCODE_END = 0x23, // END key
		KEYCODE_HOME = 0x24, // HOME key
		KEYCODE_LEFT = 0x25, // LEFT ARROW key
		KEYCODE_UP = 0x26, // UP ARROW key
		KEYCODE_RIGHT = 0x27, // RIGHT ARROW key
		KEYCODE_DOWN = 0x28, // DOWN ARROW key
		KEYCODE_SELECT = 0x29, // SELECT key
		KEYCODE_PRINT = 0x2A, // PRINT key
		KEYCODE_EXECUTE = 0x2B, // EXECUTE key
		KEYCODE_SNAPSHOT = 0x2C, // PRINT SCREEN key
		KEYCODE_INSERT = 0x2D, // INS key
		KEYCODE_DELETE = 0x2E, // DEL key
		KEYCODE_HELP = 0x2F, // HELP key

							 // 0 - 9 keys
		KEYCODE_0 = 0x30,
		KEYCODE_1 = 0x31,
		KEYCODE_2 = 0x32,
		KEYCODE_3 = 0x33,
		KEYCODE_4 = 0x34,
		KEYCODE_5 = 0x35,
		KEYCODE_6 = 0x36,
		KEYCODE_7 = 0x37,
		KEYCODE_8 = 0x38,
		KEYCODE_9 = 0x39,

		// 0x3A-40 -> Undefined

		// A - Z keys
		KEYCODE_A = 'A', // A Key
		KEYCODE_B = 'B', // B Key
		KEYCODE_C = 'C', // C Key
		KEYCODE_D = 'D', // D Key
		KEYCODE_E = 'E', // E Key
		KEYCODE_F = 'F', // F Key
		KEYCODE_G = 'G', // G Key
		KEYCODE_H = 'H', // H Key
		KEYCODE_I = 'I', // I Key
		KEYCODE_J = 'J', // J Key
		KEYCODE_K = 'K', // K Key
		KEYCODE_L = 'L', // L Key
		KEYCODE_M = 'M', // M Key
		KEYCODE_N = 'N', // N Key
		KEYCODE_O = 'O', // O Key
		KEYCODE_P = 'P', // P Key
		KEYCODE_Q = 'Q', // Q Key
		KEYCODE_R = 'R', // R Key
		KEYCODE_S = 'S', // S Key
		KEYCODE_T = 'T', // T Key
		KEYCODE_U = 'U', // U Key
		KEYCODE_V = 'V', // V Key
		KEYCODE_W = 'W', // W Key
		KEYCODE_X = 'X', // X Key
		KEYCODE_Y = 'Y', // Y Key
		KEYCODE_Z = 'Z', // Z Key

		KEYCODE_LWIN = 0x5B, // Left Windows key (Natural keyboard)
		KEYCODE_RWIN = 0x5C, // Right Windows key (Natural keyboard)
		KEYCODE_APPS = 0x5D, // Applications key (Natural keyboard)

							 // 0x5E -> Reserved

		KEYCODE_SLEEP = 0x5F, // Computer Sleep key
		KEYCODE_NUMPAD0 = 0x60, // Numeric keypad 0 key
		KEYCODE_NUMPAD1 = 0x61, // Numeric keypad 1 key
		KEYCODE_NUMPAD2 = 0x62, // Numeric keypad 2 key
		KEYCODE_NUMPAD3 = 0x63, // Numeric keypad 3 key
		KEYCODE_NUMPAD4 = 0x64, // Numeric keypad 4 key
		KEYCODE_NUMPAD5 = 0x65, // Numeric keypad 5 key
		KEYCODE_NUMPAD6 = 0x66, // Numeric keypad 6 key
		KEYCODE_NUMPAD7 = 0x67, // Numeric keypad 7 key
		KEYCODE_NUMPAD8 = 0x68, // Numeric keypad 8 key
		KEYCODE_NUMPAD9 = 0x69, // Numeric keypad 9 key
		KEYCODE_MULTIPLY = 0x6A, // Multiply key
		KEYCODE_ADD = 0x6B, // Add key
		KEYCODE_SEPARATOR = 0x6C, // Separator key
		KEYCODE_SUBTRACT = 0x6D, // Subtract key
		KEYCODE_DECIMAL = 0x6E, // Decimal key
		KEYCODE_DIVIDE = 0x6F, // Divide key
		KEYCODE_F1 = 0x70, // F1 key
		KEYCODE_F2 = 0x71, // F2 key
		KEYCODE_F3 = 0x72, // F3 key
		KEYCODE_F4 = 0x73, // F4 key
		KEYCODE_F5 = 0x74, // F5 key
		KEYCODE_F6 = 0x75, // F6 key
		KEYCODE_F7 = 0x76, // F7 key
		KEYCODE_F8 = 0x77, // F8 key
		KEYCODE_F9 = 0x78, // F9 key
		KEYCODE_F10 = 0x79, // F10 key
		KEYCODE_F11 = 0x7A, // F11 key
		KEYCODE_F12 = 0x7B, // F12 key
		KEYCODE_F13 = 0x7C, // F13 key
		KEYCODE_F14 = 0x7D, // F14 key
		KEYCODE_F15 = 0x7E, // F15 key
		KEYCODE_F16 = 0x7F, // F16 key
		KEYCODE_F17 = 0x80, // F17 key
		KEYCODE_F18 = 0x81, // F18 key
		KEYCODE_F19 = 0x82, // F19 key
		KEYCODE_F20 = 0x83, // F20 key
		KEYCODE_F21 = 0x84, // F21 key
		KEYCODE_F22 = 0x85, // F22 key
		KEYCODE_F23 = 0x86, // F23 key
		KEYCODE_F24 = 0x87, // F24 key

							// 0x88-8F -> Unassigned

		KEYCODE_NUMLOCK = 0x90, // NUM LOCK key
		KEYCODE_SCROLL = 0x91, // SCROLL LOCK key

							   // 0x92-96 -> OEM specific

							   // 0x97-9F -> Unassigned

		KEYCODE_LSHIFT = 0xA0, // Left SHIFT key
		KEYCODE_RSHIFT = 0xA1, // Right SHIFT key
		KEYCODE_LCONTROL = 0xA2, // Left CONTROL key
		KEYCODE_RCONTROL = 0xA3, // Right CONTROL key
		KEYCODE_LMENU = 0xA4, // Left MENU key
		KEYCODE_RMENU = 0xA5, // Right MENU key
		KEYCODE_BROWSER_BACK = 0xA6, // Browser Back key
		KEYCODE_BROWSER_FORWARD = 0xA7, // Browser Forward key
		KEYCODE_BROWSER_REFRESH = 0xA8, // Browser Refresh key
		KEYCODE_BROWSER_STOP = 0xA9, // Browser Stop key
		KEYCODE_BROWSER_SEARCH = 0xAA, // Browser Search key
		KEYCODE_BROWSER_FAVORITES = 0xAB, // Browser Favorites key
		KEYCODE_BROWSER_HOME = 0xAC, // Browser Start and Home key
		KEYCODE_VOLUME_MUTE = 0xAD, // Volume Mute key
		KEYCODE_VOLUME_DOWN = 0xAE, // Volume Down key
		KEYCODE_VOLUME_UP = 0xAF, // Volume Up key
		KEYCODE_MEDIA_NEXT_TRACK = 0xB0, // Next Track key
		KEYCODE_MEDIA_PREV_TRACK = 0xB1, // Previous Track key
		KEYCODE_MEDIA_STOP = 0xB2, // Stop Media key
		KEYCODE_MEDIA_PLAY_PAUSE = 0xB3, // Play/Pause Media key
		KEYCODE_LAUNCH_MAIL = 0xB4, // Start Mail key
		KEYCODE_LAUNCH_MEDIA_SELECT = 0xB5, // Select Media key
		KEYCODE_LAUNCH_APP1 = 0xB6, // Start Application 1 key
		KEYCODE_LAUNCH_APP2 = 0xB7, // Start Application 2 key

									// 0xB8-B9 -> Reserved

		KEYCODE_OEM_1 = 0xBA, // Used for miscellaneous characters; it can vary by keyboard.
							  // For the US standard keyboard, the ';:' key
		KEYCODE_OEM_PLUS = 0xBB, // For any country/region, the '+' key
		KEYCODE_OEM_COMMA = 0xBC, // For any country/region, the ',' key
		KEYCODE_OEM_MINUS = 0xBD, // For any country/region, the '-' key
		KEYCODE_OEM_PERIOD = 0xBE, // For any country/region, the '.' key
		KEYCODE_OEM_2 = 0xBF, // Used for miscellaneous characters; it can vary by keyboard.
							  // For the US standard keyboard, the '/?' key
		KEYCODE_OEM_3 = 0xC0, // Used for miscellaneous characters; it can vary by keyboard.
							  // For the US standard keyboard, the '`~' key

							  // 0xC1-D7 -> Reserved

							  //0xD8-DA -> Unassigned

		KEYCODE_OEM_4 = 0xDB, // Used for miscellaneous characters; it can vary by keyboard.
							  // For the US standard keyboard, the '[{' key
		KEYCODE_OEM_5 = 0xDC, // Used for miscellaneous characters; it can vary by keyboard.
							  // For the US standard keyboard, the '\|' key
		KEYCODE_OEM_6 = 0xDD, // Used for miscellaneous characters; it can vary by keyboard.
							  // For the US standard keyboard, the ']}' key
		KEYCODE_OEM_7 = 0xDE, // Used for miscellaneous characters; it can vary by keyboard.
							  // For the US standard keyboard, the 'single-quote/double-quote' key
		KEYCODE_OEM_8 = 0xDF, // Used for miscellaneous characters; it can vary by keyboard.

							  // 0xE0 -> Reserved

							  // 0xE1 -> OEM specific

		KEYCODE_OEM_102 = 0xE2, // Either the angle bracket key or the backslash key on the RT 102-key keyboard

								//0xE3-E4 -> OEM specific

		KEYCODE_PROCESSKEY = 0xE5, // IME PROCESS key

								   // 0xE6 -> OEM specific

		KEYCODE_PACKET = 0xE7, // Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP

							   // 0xE8 -> Unassigned

							   // 0xE9-F5 -> OEM specific

		KEYCODE_ATTN = 0xF6, // Attn key
		KEYCODE_CRSEL = 0xF7, // CrSel key
		KEYCODE_EXSEL = 0xF8, // ExSel key
		KEYCODE_EREOF = 0xF9, // Erase EOF key
		KEYCODE_PLAY = 0xFA, // Play key
		KEYCODE_ZOOM = 0xFB, // Zoom key
		KEYCODE_NONAME = 0xFC, // Reserved
		KEYCODE_PA1 = 0xFD, // PA1 key
		KEYCODE_OEM_CLEAR = 0xFE
	};

#pragma endregion

}

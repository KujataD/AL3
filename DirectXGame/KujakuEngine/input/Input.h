#pragma once
#include "../math/Vector2.h"
#include <Windows.h>
#include <cassert>
// dinput
#define DIRECTINPUT_VERSION 0x0800 // DirectInputのバージョン指定
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

namespace KujakuEngine {

class Input {
public:

	static void Initialize();

	static void Update();

	static bool GetKey(char keycode) { return key_[keycode]; }
	static bool GetPreKey(char keycode) { return preKey_[keycode]; }

	static bool GetMouse(int num) { return mouseState_.rgbButtons[num]; }
	static bool GetPreMouse(int num) { return preMouseState_.rgbButtons[num]; }

	static Vector2 GetMousePos();

private:
 	static HWND hwnd_;
	
	static IDirectInput8* directInput_;
	static IDirectInputDevice8* keyboard_;
	static IDirectInputDevice8* mouse_;

	// キー入力の箱
	static BYTE key_[256];
	static BYTE preKey_[256];

	// マウス入力を受け止める箱
	static DIMOUSESTATE2 mouseState_;
	static DIMOUSESTATE2 preMouseState_;
};

}; // namespace Kujaku


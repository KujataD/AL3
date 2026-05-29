#pragma once
#include "../math/Vector2.h"
#include <Windows.h>
#include <cassert>

// DirectInputのバージョン指定
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Xinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "xinput.lib")

namespace KujakuEngine {

class Input {
public:
	static void Initialize();

	static void Update();

	static bool GetKey(unsigned char keycode) { return key_[keycode]; }
	static bool GetPreKey(unsigned char keycode) { return preKey_[keycode]; }
	static bool GetKeyTrigger(unsigned char keycode) { return key_[keycode] && !preKey_[keycode]; }
	static bool GetKeyRelease(unsigned char keycode) { return !key_[keycode] && preKey_[keycode]; }

	static bool GetClick(int num) { return mouseState_.rgbButtons[num]; }
	static bool GetPreClick(int num) { return preMouseState_.rgbButtons[num]; }
	static bool GetClickTrigger(int num) { return mouseState_.rgbButtons[num] && !preMouseState_.rgbButtons[num]; }
	static bool GetClickRelease(int num) { return !mouseState_.rgbButtons[num] && preMouseState_.rgbButtons[num]; }

	static Vector2 GetMousePos();

	// コントローラーが接続されているかを取得する。
	static bool IsControllerConnected(int padNo = 0);

	// コントローラーのボタン入力を取得する。
	static bool GetControllerButton(WORD button, int padNo = 0);
	static bool GetPreControllerButton(WORD button, int padNo = 0);
	static bool GetControllerButtonTrigger(WORD button, int padNo = 0);
	static bool GetControllerButtonRelease(WORD button, int padNo = 0);

	// 左右スティックの入力を取得する。
	static Vector2 GetLeftStick(int padNo = 0);
	static Vector2 GetRightStick(int padNo = 0);

	// 左右トリガーの入力を取得する。
	static float GetLeftTrigger(int padNo = 0);
	static float GetRightTrigger(int padNo = 0);

private:
	static HWND hwnd_;

	static IDirectInput8* directInput_;
	static IDirectInputDevice8* keyboard_;
	static IDirectInputDevice8* mouse_;

	// キーボード入力状態
	static BYTE key_[256];
	static BYTE preKey_[256];

	// マウス入力状態
	static DIMOUSESTATE2 mouseState_;
	static DIMOUSESTATE2 preMouseState_;

	// 最大4台分のコントローラー入力状態を保持する。
	static XINPUT_STATE controllerState_[XUSER_MAX_COUNT];
	static XINPUT_STATE preControllerState_[XUSER_MAX_COUNT];
	static bool isControllerConnected_[XUSER_MAX_COUNT];
};

}; // namespace KujakuEngine

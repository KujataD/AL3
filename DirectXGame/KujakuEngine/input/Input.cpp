#include "Input.h"
#include "../base/WinApp.h"

namespace KujakuEngine {

IDirectInput8* Input::directInput_ = nullptr;
IDirectInputDevice8* Input::keyboard_ = nullptr;
IDirectInputDevice8* Input::mouse_ = nullptr;

BYTE Input::key_[256] = {};
BYTE Input::preKey_[256] = {};

DIMOUSESTATE2 Input::mouseState_ = {};
DIMOUSESTATE2 Input::preMouseState_ = {};

XINPUT_STATE Input::controllerState_[XUSER_MAX_COUNT] = {};
XINPUT_STATE Input::preControllerState_[XUSER_MAX_COUNT] = {};
bool Input::isControllerConnected_[XUSER_MAX_COUNT] = {};

HWND Input::hwnd_;

Vector2 Input::mouseClientPos_ = {};
Vector2 Input::mousePreClientPos_ = {};

namespace {

// XInputで扱えるコントローラー番号かを確認する。
bool IsValidControllerNo(int padNo) {
	return 0 <= padNo && padNo < XUSER_MAX_COUNT;
}

// スティックの生値を-1.0f～1.0fの範囲に変換する。
float NormalizeStickValue(SHORT value, SHORT deadZone) {
	if (-deadZone < value && value < deadZone) {
		return 0.0f;
	}

	// XInputの負方向は-32768まで、正方向は32767までなので分母を分ける。
	return value < 0 ? static_cast<float>(value) / 32768.0f : static_cast<float>(value) / 32767.0f;
}

/// トリガーの生値を0.0f～1.0fの範囲に変換する。
float NormalizeTriggerValue(BYTE value) {
	if (value < XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
		return 0.0f;
	}

	return static_cast<float>(value) / 255.0f;
}

}

void Input::Initialize() {
	HRESULT hr;
	WinApp* winApp = WinApp::GetInstance();

	hwnd_ = winApp->GetHwnd();

	// DirectInputを初期化する。
	hr = DirectInput8Create(winApp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput_, nullptr);
	assert(SUCCEEDED(hr));

	// キーボードデバイスを生成する。
	hr = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(hr));

	// キーボード入力データの形式を設定する。
	hr = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));

	// キーボードの協調レベルを設定する。
	hr = keyboard_->SetCooperativeLevel(hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));

	// マウスデバイスを生成する。
	hr = directInput_->CreateDevice(GUID_SysMouse, &mouse_, NULL);
	assert(SUCCEEDED(hr));

	// マウス入力データの形式を設定する。
	hr = mouse_->SetDataFormat(&c_dfDIMouse2);

	// マウスの協調レベルを設定する。
	mouse_->SetCooperativeLevel(hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
}

void Input::Update() {
	// キーボード入力の取得を開始する。
	keyboard_->Acquire();

	// 前フレームのキーボード入力を保存してから、現在フレームの入力を取得する。
	memcpy(preKey_, key_, 256);
	keyboard_->GetDeviceState(sizeof(key_), key_);

	// マウス入力の取得を開始する。
	mouse_->Acquire();

	// 前フレームのマウス入力を保存してから、現在フレームの入力を取得する。
	preMouseState_ = mouseState_;
	mouse_->GetDeviceState(sizeof(mouseState_), &mouseState_);

	// マウス座標の更新
	mousePreClientPos_ = mouseClientPos_;
	mouseClientPos_ = CalcMouseClientPos();

	// コントローラー入力を更新する。
	// XInputは最大4台まで扱えるため、全てのスロットを毎フレーム確認する。
	for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i) {
		preControllerState_[i] = controllerState_[i];

		// XInputGetStateは接続中のコントローラーだけERROR_SUCCESSを返す。
		if (XInputGetState(i, &controllerState_[i]) == ERROR_SUCCESS) {
			isControllerConnected_[i] = true;
		} else {
			// 未接続の場合は状態を空にして、前回の入力が残らないようにする。
			isControllerConnected_[i] = false;
			controllerState_[i] = {};
		}
	}
}

Vector2 Input::GetMouseClientPos() {
	return mouseClientPos_;
}

Vector2 Input::GetMousePreClientPos() {
	return mousePreClientPos_;
}

bool Input::IsControllerConnected(int padNo) {
	if (!IsValidControllerNo(padNo)) {
		return false;
	}

	return isControllerConnected_[padNo];
}

bool Input::GetControllerButton(WORD button, int padNo) {
	if (!IsControllerConnected(padNo)) {
		return false;
	}

	return (controllerState_[padNo].Gamepad.wButtons & button) != 0;
}

bool Input::GetPreControllerButton(WORD button, int padNo) {
	if (!IsValidControllerNo(padNo)) {
		return false;
	}

	return (preControllerState_[padNo].Gamepad.wButtons & button) != 0;
}

bool Input::GetControllerButtonTrigger(WORD button, int padNo) {
	return GetControllerButton(button, padNo) && !GetPreControllerButton(button, padNo);
}

bool Input::GetControllerButtonRelease(WORD button, int padNo) {
	return !GetControllerButton(button, padNo) && GetPreControllerButton(button, padNo);
}

Vector2 Input::GetLeftStick(int padNo) {
	if (!IsControllerConnected(padNo)) {
		return Vector2{0.0f, 0.0f};
	}

	const XINPUT_GAMEPAD& gamepad = controllerState_[padNo].Gamepad;
	return Vector2{
		NormalizeStickValue(gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE),
		NormalizeStickValue(gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)};
}

Vector2 Input::GetRightStick(int padNo) {
	if (!IsControllerConnected(padNo)) {
		return Vector2{0.0f, 0.0f};
	}

	const XINPUT_GAMEPAD& gamepad = controllerState_[padNo].Gamepad;
	return Vector2{
		NormalizeStickValue(gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE),
		NormalizeStickValue(gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)};
}

float Input::GetLeftTrigger(int padNo) {
	if (!IsControllerConnected(padNo)) {
		return 0.0f;
	}

	return NormalizeTriggerValue(controllerState_[padNo].Gamepad.bLeftTrigger);
}

float Input::GetRightTrigger(int padNo) {
	if (!IsControllerConnected(padNo)) {
		return 0.0f;
	}

	return NormalizeTriggerValue(controllerState_[padNo].Gamepad.bRightTrigger);
}

Vector2 Input::CalcMouseClientPos() {
	POINT mousePoint;

	// マウスカーソルのスクリーン座標を取得する。
	GetCursorPos(&mousePoint);

	// スクリーン座標をウィンドウのクライアント座標へ変換する。
	ScreenToClient(hwnd_, &mousePoint);

	return Vector2{ static_cast<float>(mousePoint.x), static_cast<float>(mousePoint.y) };
}

}

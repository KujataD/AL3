#include "WinApp.h"
#include "../../externals/imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace KujakuEngine {

// 静的メンバ変数の実体
const wchar_t WinApp::kWindowClassName[] = L"KujakuEngineWindowClass";

WinApp* WinApp::GetInstance() {
	static WinApp instance;
	return &instance;
}

LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void WinApp::CreateGameWindow(const std::wstring& title, int32_t clientWidth, int32_t clientHeight) {
	// ウィンドウクラスの設定
	wndClass_.cbSize = sizeof(WNDCLASSEX);
	wndClass_.lpfnWndProc = WindowProc;
	wndClass_.lpszClassName = kWindowClassName;
	wndClass_.hInstance = GetModuleHandle(nullptr);
	wndClass_.hCursor = LoadCursor(nullptr, IDC_ARROW);
	RegisterClassEx(&wndClass_);

	// クライアント領域のサイズからウィンドウサイズを計算
	RECT wrc = {0, 0, clientWidth, clientHeight};
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウの生成 → hwnd_ に代入（修正済み）
	hwnd_ = CreateWindow(
	    wndClass_.lpszClassName, title.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, wrc.right - wrc.left, wrc.bottom - wrc.top, nullptr, nullptr, wndClass_.hInstance, nullptr);

	ShowWindow(hwnd_, SW_SHOW);
}

bool WinApp::ProcessMessage() {
	MSG msg{};

	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.message == WM_QUIT;
}

void WinApp::TerminateGameWindow() {
	CloseWindow(hwnd_);
	UnregisterClass(wndClass_.lpszClassName, wndClass_.hInstance);
}

} // namespace KujakuEngine

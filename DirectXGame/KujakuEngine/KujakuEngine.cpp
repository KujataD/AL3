#include "KujakuEngine.h"
#include <objbase.h>

namespace KujakuEngine {

void Initialize(const std::wstring& title, bool enableDebugLayer) {
	// COMの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

	// ウィンドウの生成
	WinApp* winApp = WinApp::GetInstance();
	winApp->CreateGameWindow(title);

	// DirectX の初期化
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(winApp, WinApp::kWindowWidth, WinApp::kWindowHeight, enableDebugLayer);

#ifdef _DEBUG

	// ImGui の初期化
	ImGuiManager::GetInstance()->Initialize();

#endif // _DEBUG

	// グラフィックスパイプラインの初期化
	GraphicsPipeline::GetInstance()->Initialize();
}

void Finalize() { WinApp::GetInstance()->TerminateGameWindow(); }

bool Update() {
	// ウィンドウメッセージを処理し、終了リクエストなら false を返す
	if (WinApp::GetInstance()->ProcessMessage()) {
		return false;
	}
	return true;
}

void PreDraw() {
	// バックバッファクリア＆バリア設定
	DirectXCommon::GetInstance()->PreDraw();

	// RootSignature・PSO・Viewport・ScissorRect をコマンドリストに積む
	GraphicsPipeline::GetInstance()->SetCommandList();
}

void PostDraw() {

#ifdef _DEBUG
	// ImGuiの描画コマンドを積む（描画処理の最後に呼ぶ）
	ImGuiManager::GetInstance()->End();
#endif // _DEBUG

	// プレゼント・フェンス待機・コマンドリストリセット
	DirectXCommon::GetInstance()->PostDraw();
}

} // namespace KujakuEngine

#include "GameScene.h"
#include "KujakuEngine/KujakuEngine.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include <memory>

using namespace KujakuEngine;

// Windowsアプリでのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// エンジン初期化
	Initialize(L"KujakuEngine", true);

	GameScene* gameScene = new GameScene;
	gameScene->Init();

	// ゲームループ
	while (Update()) {

		ImGuiManager::GetInstance()->Begin();

		///
		/// ↓↓↓ 更新処理ここから ↓↓↓
		///

		///
		/// ↑↑↑ 更新処理ここまで ↑↑↑
		///

		PreDraw();

		///
		/// ↓↓↓ 描画処理ここから ↓↓↓
		///

		gameScene->Draw();

		///
		/// ↑↑↑ 描画処理ここまで ↑↑↑
		///

		PostDraw();
	}

	delete gameScene;

	// エンジンの終了処理
	Finalize();

	return 0;
}
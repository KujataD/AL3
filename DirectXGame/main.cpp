#include <KujakuEngine.h>
#include "GameScene.h"

using namespace KujakuEngine;

// Windowsアプリでのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// エンジン初期化
	KujakuEngine::Initialize(L"LC2B_04_オオツカ_ダイチ_AL3", false);

	// ImGuiManagerインスタンスの取得
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	// ゲームシーン
	GameScene* gameScene = new GameScene;
	gameScene->Initialize();

	// ゲームループ
	while (Update()) {
		Input::Update();

		imguiManager->Begin();

		///
		/// ↓↓↓ 更新処理ここから ↓↓↓
		///
		
		gameScene->Update();

#ifdef USE_IMGUI
		GlobalVariables::GetInstance()->Update();
#endif // USE_IMGUI

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

	// エンジンの終了処理
	Finalize();

	return 0;
}
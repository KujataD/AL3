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
	Initialize(L"LC2B_04_オオツカ_ダイチ_AL3", true);

	GameScene* gameScene = new GameScene;
	gameScene->Init();

	// ゲームループ
	while (Update()) {

		///
		ImGuiManager::GetInstance()->Begin();
		/// ↓↓↓ 更新処理ここから ↓↓↓
		///

		gameScene->Update();

#ifdef USE_IMGUI
		ImGui::Begin("LightManager");
		auto& light = DirectionalLight::GetInstance()->GetData();
		ImGui::ColorEdit3("Light Color", &light.color.x);
		ImGui::SliderFloat3("Direction", &light.direction.x, -1.0f, 1.0f);
		ImGui::DragFloat("Intensity", &light.intensity, 0.01f);
		ImGui::End();
#endif // USE_IMGUI

		///
		/// ↑↑↑ 更新処理ここまで ↑↑↑
		///

		///
		PreDraw();
		/// ↓↓↓ 描画処理ここから ↓↓↓
		///

		gameScene->Draw();

		///
		/// ↑↑↑ 描画処理ここまで ↑↑↑
		PostDraw();
		///
	}

	delete gameScene;

	// エンジンの終了処理
	Finalize();

	return 0;
}
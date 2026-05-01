#include <KujakuEngine.h>
#include <array>
#include <cassert>
#include <chrono>
#include <fstream>
#include <memory>

#include "GameScene.h"
#include "TitleScene.h"

#include "GlobalVariables.h"

#include "StageManager.h"

using namespace KujakuEngine;

enum class Scene {
	kUnknown = 0,
	kTitle,
	kGame,
};

StageManager* stageManager = nullptr;

Scene scene = Scene::kUnknown;

// ゲームシーン
GameScene* gameScene = nullptr;

// タイトルシーン
TitleScene* titleScene;

void InitScene();

void ChangeScene();

void UpdateScene();

void DrawScene();

void LoadDebugSettings();

// Windowsアプリでのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// エンジン初期化
	KujakuEngine::Initialize(L"LC2B_04_オオツカ_ダイチ_AL3", false);
	// タイトルシーンの初期化
	scene = Scene::kTitle;

	// クラスインスタンスの生成
	stageManager = new StageManager;
	// ステージデータファイルを読み込む
	stageManager->LoadStageDataFile();

#ifdef _DEBUG
	// デバッグ設定ファイル読み込み
	LoadDebugSettings();

	scene = Scene::kGame;
#endif // _DEBUG

	// シーン初期化
	InitScene();

	// Randomのシード初期化
	Random::Initialize();

	// ImGuiManagerインスタンスの取得
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	// ゲームループ
	while (Update()) {
		Input::Update();

		imguiManager->Begin();
		auto frameWorkStart = std::chrono::steady_clock::now();

		///
		/// ↓↓↓ 更新処理ここから ↓↓↓
		///

		ChangeScene();

		UpdateScene();

#ifdef USE_IMGUI

		GlobalVariables::GetInstance()->Update();

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

		PreDraw();
		///
		/// ↓↓↓ 描画処理ここから ↓↓↓
		///

		DrawScene();

		///
		/// ↑↑↑ 描画処理ここまで ↑↑↑
		///
		PostDraw();
	}

	delete titleScene;
	titleScene = nullptr;
	delete gameScene;
	gameScene = nullptr;
	delete stageManager;

	// エンジンの終了処理
	Finalize();

	return 0;
}

void InitScene() {
	switch (scene) {
	case Scene::kUnknown:
		break;
	case Scene::kTitle:
		if (!titleScene) {
			titleScene = new TitleScene;
			titleScene->Init();
		}
		break;
	case Scene::kGame:
		if (!gameScene) {
			gameScene = new GameScene;
			gameScene->Init(stageManager);
		}
		break;
	default:
		break;
	}
}

void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene->IsFinished()) {
			// シーン変更
			scene = Scene::kGame;
			// 旧シーンの解放
			delete titleScene;
			titleScene = nullptr;
			// 新シーンの生成と初期化
			gameScene = new GameScene;
			gameScene->Init(stageManager);
		}
		break;
	case Scene::kGame:
		if (gameScene->IsFinished()) {
			// シーン変更
			scene = Scene::kTitle;
			// 旧シーンの解放
			delete gameScene;
			gameScene = nullptr;
			// 新シーンの生成と初期化
			titleScene = new TitleScene;
			titleScene->Init();
		} else if (gameScene->GetReloadRequested()) {
			// シーンリロード
			delete gameScene;
			gameScene = nullptr;
			gameScene = new GameScene;
			gameScene->Init(stageManager);
		}
		break;
	}
}

void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	default:
		break;
	}
}

void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	default:
		break;
	}
}

void LoadDebugSettings() {
	// ステージデータファイルのパス
	const std::string filePath = "DebugSettings.ini";

	// ifstreamでステージデータファイルを開く
	std::ifstream file;
	file.open(filePath);
	assert(file.is_open() && "DebugSettingsが存在しません");

	std::string key;
	std::string value;
	file >> key >> value;

	if (key == "InitialStage") {
		stageManager->SetCurrentStageIndexByName(value);
	}
}

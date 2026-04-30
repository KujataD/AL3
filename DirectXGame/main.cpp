#include <KujakuEngine.h>
#include <array>
#include <cassert>
#include <chrono>
#include <fstream>
#include <memory>

#include "GameScene.h"
#include "TitleScene.h"

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
	KujakuEngine::Initialize(L"LC2B_04_オオツカ_ダイチ_AL3", true);
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
	std::array<float, 180> frameMsHistory{};
	size_t frameMsHistoryIndex = 0;
	float frameMsMax = 0.0f;
	auto frameStartTime = std::chrono::steady_clock::now();
	float lastChangeSceneMs = 0.0f;
	float lastUpdateSceneMs = 0.0f;
	float lastDrawSceneMs = 0.0f;
	float lastPostDrawMs = 0.0f;
	float lastFrameWorkMs = 0.0f;


	// ゲームループ
	while (Update()) {
		auto frameNow = std::chrono::steady_clock::now();
		float frameMs = std::chrono::duration<float, std::milli>(frameNow - frameStartTime).count();
		frameStartTime = frameNow;
		frameMsHistory[frameMsHistoryIndex] = frameMs;
		frameMsHistoryIndex = (frameMsHistoryIndex + 1) % frameMsHistory.size();
		frameMsMax = 0.0f;
		for (float ms : frameMsHistory) {
			if (ms > frameMsMax) {
				frameMsMax = ms;
			}
		}

		Input::Update();

		imguiManager->Begin();
		auto frameWorkStart = std::chrono::steady_clock::now();
		
		///
		/// ↓↓↓ 更新処理ここから ↓↓↓
		///

		auto t0 = std::chrono::steady_clock::now();
		ChangeScene();
		auto t1 = std::chrono::steady_clock::now();

		UpdateScene();
		auto t2 = std::chrono::steady_clock::now();

#ifdef USE_IMGUI
			ImGui::Begin("LightManager");
			auto& light = DirectionalLight::GetInstance()->GetData();
			ImGui::ColorEdit3("Light Color", &light.color.x);
			ImGui::SliderFloat3("Direction", &light.direction.x, -1.0f, 1.0f);
			ImGui::DragFloat("Intensity", &light.intensity, 0.01f);
			ImGui::End();

			ImGui::Begin("Performance");
			ImGui::Text("Frame: %.2f ms (%.1f FPS)", frameMs, frameMs > 0.0f ? 1000.0f / frameMs : 0.0f);
			ImGui::Text("Worst(Last %d): %.2f ms", static_cast<int>(frameMsHistory.size()), frameMsMax);
			ImGui::PlotLines("Frame ms", frameMsHistory.data(), static_cast<int>(frameMsHistory.size()), static_cast<int>(frameMsHistoryIndex), nullptr, 0.0f, frameMsMax < 16.7f ? 16.7f : frameMsMax, ImVec2(0, 80));
			ImGui::SeparatorText("Texture Load Events");
			const auto& textureEvents = TextureManager::GetInstance()->GetRecentLoadEvents();
			for (const auto& event : textureEvents) {
				if (event.cacheHit) {
					ImGui::Text("[HIT ] %s", event.filePath.c_str());
				} else {
					ImGui::Text("[LOAD] %6.2f ms : %s", event.loadMs, event.filePath.c_str());
				}
			}
			ImGui::End();

			ImGui::Begin("Performance Breakdown");
			ImGui::Text("Frame Work: %.2f ms", lastFrameWorkMs);
			ImGui::Text("ChangeScene : %6.2f ms", lastChangeSceneMs);
			ImGui::Text("UpdateScene : %6.2f ms", lastUpdateSceneMs);
			ImGui::Text("DrawScene   : %6.2f ms", lastDrawSceneMs);
			ImGui::Text("PostDraw    : %6.2f ms", lastPostDrawMs);

			float phaseMs[4] = {lastChangeSceneMs, lastUpdateSceneMs, lastDrawSceneMs, lastPostDrawMs};
			ImGui::SeparatorText("Phase Bars");
			ImGui::PlotHistogram("##phase", phaseMs, 4, 0, nullptr, 0.0f, lastFrameWorkMs < 16.7f ? 16.7f : lastFrameWorkMs, ImVec2(0, 70));
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
		auto t3 = std::chrono::steady_clock::now();
			PostDraw();
			auto t4 = std::chrono::steady_clock::now();

			lastChangeSceneMs = std::chrono::duration<float, std::milli>(t1 - t0).count();
			lastUpdateSceneMs = std::chrono::duration<float, std::milli>(t2 - t1).count();
			lastDrawSceneMs = std::chrono::duration<float, std::milli>(t3 - t2).count();
			lastPostDrawMs = std::chrono::duration<float, std::milli>(t4 - t3).count();
			lastFrameWorkMs = std::chrono::duration<float, std::milli>(t4 - frameWorkStart).count();
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

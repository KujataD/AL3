#pragma once
#include "KujakuEngine/KujakuEngine.h"
#include "Player.h"
#include "MapchipField.h"
#include "Skydome.h"
#include "CameraController.h"

class GameScene {
public:
	GameScene();
	~GameScene();

	// 初期化
	void Init();

	// 更新
	void Update();

	// 描画
	void Draw();

	void GenerateBlocks();

private:
	// --- カメラ ---
	KujakuEngine::Camera camera_;
	KujakuEngine::DebugCamera debugCamera_;
	CameraController* cameraController_ = nullptr;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;


	// --- プレイヤー ---
	Player* player_ = nullptr;
	KujakuEngine::Model* playerModel_ = nullptr;

	// --- ブロック ---
	MapChipField* mapChipField_ = nullptr;
	KujakuEngine::Model* blockModel_ = nullptr;
	std::vector<std::vector<KujakuEngine::WorldTransform*>> worldTransformBlocks_;

	// --- スカイドーム ---
	Skydome* skydome_ = nullptr;
	KujakuEngine::Model* modelSkydome_ = nullptr;

};

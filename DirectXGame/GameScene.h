#pragma once
#include <KujakuEngine.h>
#include "Player.h"

class GameScene {
public:
	GameScene() = default;
	~GameScene();

	void Initialize();
	
	void Update();

	void Draw();

private:
	void UpdateCamera();

	void ApplyAllVariables();
	void RegisterAllVariables();
	

private:
	// カメラ
	KujakuEngine::Camera camera_;
	KujakuEngine::DebugCamera debugCamera_;
	bool isActiveDebugCamera_ = false;

	// --- プレイヤー ---
	Player* player_ = nullptr;

	// モデル
	KujakuEngine::Model* modelPlayer_ = nullptr;

	
};

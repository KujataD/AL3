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
	// カメラ
	KujakuEngine::Camera camera_;

	// --- プレイヤー ---
	Player* player_ = nullptr;

	// モデル
	KujakuEngine::Model* modelPlayer_ = nullptr;

	
};

#pragma once
#include "KujakuEngine/KujakuEngine.h"
#include "Player.h"

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

private:
	// モデル
	KujakuEngine::Model* model_ = nullptr;
	
	// --- カメラ ---
	KujakuEngine::Camera camera_ ;
	KujakuEngine::DebugCamera debugCamera_;

	// プレイヤー
	Player* player_ = nullptr;

};

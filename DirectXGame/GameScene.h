#pragma once
#include "Enemy.h"
#include "Player.h"
#include <KujakuEngine.h>

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
	std::unique_ptr<Player> player_;

	// モデル
	std::unique_ptr<KujakuEngine::Model> modelPlayer_;
	std::unique_ptr<KujakuEngine::Model> modelBullet_;

	// --- エネミー ---
	std::unique_ptr<Enemy> enemy_;

	std::unique_ptr<KujakuEngine::Model> modelEnemy_;


};

#pragma once
#include "Enemy.h"
#include "Player.h"
#include "Terrain.h"
#include "Skydome.h"
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

	void CheckAllCollisions();

	/// <summary>
	/// 敵弾を追加する
	/// </summary>
	void AddEnemyBullet();

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
	std::unique_ptr<KujakuEngine::Model> modelEnemyBullet_;

	// 当たり判定
	std::unique_ptr<KujakuEngine::CollisionManager> collisionManager_;

	// --- スカイドーム ---
	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<KujakuEngine::Model> modelSkydome_;

	// 地形
	std::unique_ptr<Terrain> terrain_;
	std::unique_ptr<KujakuEngine::Model> modelTerrain_;
	
	float timer_ = 10.0f;
};

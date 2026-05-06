#pragma once
#include "Enemy.h"
#include "Player.h"
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

private:
	// カメラ
	KujakuEngine::Camera camera_;
	KujakuEngine::DebugCamera debugCamera_;
	bool isActiveDebugCamera_ = false;
	KujakuEngine::RailCameraController railCamera_;

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

	// --- スプライン曲線 ---
	std::unique_ptr<KujakuEngine::ParticleModel> modelSpline_;
	std::unique_ptr<KujakuEngine::Model> modelSphere_;
	std::unique_ptr<KujakuEngine::Model> modelCube_;
	
	std::vector<KujakuEngine::Vector3> railControlPoints_;
	KujakuEngine::WorldTransform railFirstCube_;
	KujakuEngine::WorldTransform railSecondSphere_;
	float timer_ = 10.0f;
};

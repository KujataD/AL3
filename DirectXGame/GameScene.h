#pragma once
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
	/// <summary>
	/// カメラの更新
	/// </summary>
	void UpdateCamera();

	/// <summary>
	/// すべての調整項目の適応
	/// </summary>
	void ApplyAllVariables();

	/// <summary>
	/// すべての調整項目の登録
	/// </summary>
	void RegisterAllVariables();

	/// <summary>
	/// すべての当たり判定処理の更新
	/// </summary>
	void CheckAllCollisions();

private:
	// カメラ
	KujakuEngine::Camera camera_;
	KujakuEngine::DebugCamera debugCamera_;
	bool isActiveDebugCamera_ = false;
	std::unique_ptr<KujakuEngine::FollowCamera> followCamera_ = nullptr;

	// --- プレイヤー ---
	std::unique_ptr<Player> player_;

	// 3Dモデル
	std::unique_ptr<KujakuEngine::Model> modelFighterBody_ = nullptr;
	std::unique_ptr<KujakuEngine::Model> modelFighterHead_ = nullptr;
	std::unique_ptr<KujakuEngine::Model> modelFighterArm_L_ = nullptr;
	std::unique_ptr<KujakuEngine::Model> modelFighterArm_R_ = nullptr;

	// --- 当たり判定 ---
	std::unique_ptr<KujakuEngine::CollisionManager> collisionManager_;

	// --- スカイドーム ---
	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<KujakuEngine::Model> modelSkydome_;

	// 地形
	std::unique_ptr<Terrain> terrain_;
	std::unique_ptr<KujakuEngine::Model> modelTerrain_;
};

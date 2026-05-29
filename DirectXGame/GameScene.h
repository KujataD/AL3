#pragma once
#include "Enemy.h"
#include "Player.h"
#include "Terrain.h"
#include "Skydome.h"
#include <KujakuEngine.h>
#include <sstream>

class GameScene {
public:
	GameScene() = default;
	~GameScene();

	void Initialize();

	void Update();

	void Draw();

	// --- 外部API ---

	/// <summary>
	/// 敵弾を追加する
	/// </summary>
	void AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBullet);

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

	/// <summary>
	/// 敵弾の更新
	/// </summary>
	void UpdateEnemyBullets();

	/// <summary>
	/// 敵ベクタの更新
	/// </summary>
	void UpdateEnemies();

	/// <summary>
	/// 敵ベクタの更新
	/// </summary>
	void SpawnEnemy(KujakuEngine::Vector3 spawnPos);

	/// <summary>
	/// 敵弾を追加する
	/// </summary>
	void LoadEnemyPopData();

	/// <summary>
	/// 敵発生コマンドの更新
	/// </summary>
	void UpdateEnemyPopCommands();

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
	std::list<std::unique_ptr<Enemy>>enemies_;
	std::unique_ptr<KujakuEngine::Model> modelEnemy_;
	std::stringstream enemyPopCommands_;
	bool isEnemyWaitingForPop_ = false;
	int32_t enemyWaitTimer_ = 0;


	// --- 敵弾 ---
	std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;
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

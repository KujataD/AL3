#pragma once
#include <KamataEngine.h>
#include "AABB.h"

class MapChipField;
class Player;
class GameScene;

class BaseEnemy {
public:
	virtual ~BaseEnemy() = default;

	virtual void Init(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	virtual void OnCollision(Player* player) = 0;
	virtual void Destroy() = 0;
	virtual void CollisionDisable() = 0;
	
	// --- set ---

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	// --- get ---

	bool IsDead() const { return isDead_; }
	bool IsCollisionDisabled() const { return isCollisionDisabled_; }

	virtual KamataEngine::Vector3 GetWorldPosition() const = 0;
	virtual KamataEngine::Vector3 GetWorldRotation() const = 0;
	virtual AABB GetAABB() const = 0;

protected:
	// --- 外部依存 ---

	// ゲームシーン
	GameScene* gameScene_ = nullptr;
	// マップチップ
	MapChipField* mapChipField_ = nullptr;

	// デス状態
	bool isDead_ = false;
	// コリジョン無効にするかどうか
	bool isCollisionDisabled_ = false;
	
};

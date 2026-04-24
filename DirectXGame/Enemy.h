#pragma once
#include "BaseEnemy.h"
#include "IEnemyBehaviorState.h"
#include "KujakuEngine/KujakuEngine.h"
#include <cmath>
#include <numbers>

class MapChipField;
class Player;
class GameScene;

/// <summary>
/// 敵
/// </summary>
class Enemy final : public BaseEnemy {
public:
	Enemy() = default;
	~Enemy();

	// 初期化
	void Init(KujakuEngine::Model* model, KujakuEngine::Camera* camera, const KujakuEngine::Vector3& position) override;

	// 更新
	void Update() override;

	// 描画
	void Draw() override;

	// Collision
	// ------------------------------------------

	void OnCollision(Player* player) override;

	// Accessor
	// ------------------------------------------

	// --- set ---

	void Destroy() override { isDead_ = true; }
	void CollisionDisable() override { isCollisionDisabled_ = true; }

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	// --- get ---

	KujakuEngine::Vector3 GetWorldPosition() const override;
	KujakuEngine::Vector3 GetWorldRotation() const override;
	KujakuEngine::AABB GetAABB() const override;

	// API
	// ------------------------------------------

	/// <summary>
	/// 移動処理
	/// </summary>
	void Move();

	/// <summary>
	/// トランスフォーム更新
	/// </summary>
	void UpdateTransform();

	void ChangeBehavior(IEnemyBehaviorState* behavior);

	void SetRotationEaseOut(const KujakuEngine::Vector3& startRotation, const KujakuEngine::Vector3& endRotation, float t);

public:
	// 定数
	// ------------------------------------------

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	// 歩行の速さ
	static inline const float kWalkSpeed = 0.05f;

	// 歩行アニメーションの周期（秒）
	static inline const float kWalkAnimationDuration = 1.0f;

	// 歩行アニメーションの最初の角度（Degree）
	static inline const float kStartWalkAnimationRotationDegree = -45.0f;

	// 歩行アニメーションの最後の角度（Degree）
	static inline const float kEndWalkAnimationRotationDegree = 90.0f;

	// デスアニメーションの時間（秒）
	static inline const float kDeadAnimDuration = 0.5f;

	// デスアニメーションでX軸回転する角度（radian）
	static inline const float kDeadAnimRotationX = -std::numbers::pi_v<float> * 0.25f;

	// デスアニメーションでY軸回転加算する角度（radian）
	static inline const float kDeadAnimAdditionalRotationY = std::numbers::pi_v<float> * -1.0f;

private:
	// 変数
	// ------------------------------------------
	// --- 外部参照 ---

	// モデル
	KujakuEngine::Model* model_ = nullptr;

	// カメラ
	KujakuEngine::Camera* camera_ = nullptr;

	// --- 内部プロパティ ---

	// ワールドトランスフォーム
	KujakuEngine::WorldTransform worldTransform_;

	// 速度
	KujakuEngine::Vector3 velocity_ = {};

	// 経過時間
	float walkTimer_ = 0.0f;

	// ふるまい
	std::unique_ptr<IEnemyBehaviorState> behavior_;

};

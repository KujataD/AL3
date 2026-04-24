#include "Enemy.h"
#include "DeltaTime.h"
#include "Easing.h"
#include "EnemyBehaviorDead.h"
#include "EnemyBehaviorRoot.h"
#include "MapChipField.h"
#include "Player.h"
#include "Transform.h"
#include "GameScene.h"
#include <cassert>
#include <numbers>

using namespace KamataEngine;
using namespace MathUtility;

Enemy::~Enemy() {}

void Enemy::Init(Model* model, Camera* camera, const Vector3& position) {
	assert(model);

	// ポインタ代入
	model_ = model;
	camera_ = camera;

	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;

	// 速度の設定
	velocity_ = {-kWalkSpeed, 0.0f, 0.0f};

	// 経過時間の初期化
	walkTimer_ = 0.0f;

	ChangeBehavior(new EnemyBehaviorRoot);
}

void Enemy::Update() {
	if (isDead_) {
		return;
	}

	if (behavior_) {
		behavior_->Update(this);
	}
}

void Enemy::Draw() {
	if (isDead_) {
		return;
	}
	model_->Draw(worldTransform_, *camera_);
}

void Enemy::OnCollision(Player* player) {
	if (behavior_->GetType() == IEnemyBehaviorState::BehaviorType::kDead) {
		return;
	}

	if (player->IsAttack()) {
		// 敵の振る舞いをデス演出に変更
		ChangeBehavior(new EnemyBehaviorDead);

		// 敵と自キャラの中間位置にエフェクトを生成
		Vector3 effectPos = (worldTransform_.translation_ + player->GetWorldPosition()) / 2.0f;
		// ちょっと手前にずらす
		effectPos.z -= 0.5f;
		gameScene_->CreateHitEffect(effectPos);
	}
}

KamataEngine::Vector3 Enemy::GetWorldPosition() const {
	Vector3 worldPos;
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;
	return worldPos;
}

KamataEngine::Vector3 Enemy::GetWorldRotation() const {
	Vector3 worldRot;
	worldRot.x = worldTransform_.rotation_.x;
	worldRot.y = worldTransform_.rotation_.y;
	worldRot.z = worldTransform_.rotation_.z;
	return worldRot;
}

AABB Enemy::GetAABB() const {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Enemy::Move() {
	// タイマー加算
	walkTimer_ += DeltaTime::Get();

	// 回転アニメーション
	float param = std::sin(2.0f * std::numbers::pi_v<float> * walkTimer_ / 1.0f);
	float degree = kStartWalkAnimationRotationDegree + kEndWalkAnimationRotationDegree * (param + 1.0f) / 2.0f;
	worldTransform_.rotation_.z = degree * std::numbers::pi_v<float> / 180.0f;

	// 移動
	worldTransform_.translation_ += velocity_;
}

void Enemy::UpdateTransform() {
	// 行列の更新
	WorldTransformUpdate(worldTransform_);
}

void Enemy::ChangeBehavior(IEnemyBehaviorState* behavior) {
	if (behavior_) {
		behavior_->OnExit(this);
	}

	behavior_.reset(behavior);

	if (behavior_) {
		behavior_->OnEnter(this);
	}
}

void Enemy::SetRotationEaseOut(const Vector3& startRotation, const Vector3& endRotation, float t) {
	worldTransform_.rotation_.x = EaseLerp(startRotation.x, endRotation.x, t, EaseType::OutQuad);
	worldTransform_.rotation_.y = EaseLerp(startRotation.y, endRotation.y, t, EaseType::OutQuad);
	worldTransform_.rotation_.z = EaseLerp(startRotation.z, endRotation.z, t, EaseType::OutQuad);
}

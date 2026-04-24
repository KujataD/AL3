#include "ShieldEnemy.h"
#include "GameScene.h"
#include "MapChipField.h"
#include "PlayerBehaviorAttack.h"
#include "Player.h"
#include "ShieldEnemyBehaviorDead.h"
#include "ShieldEnemyBehaviorRoot.h"
#include "ShieldEnemyBehaviorGuard.h"
#include <cassert>
#include <numbers>

using namespace KujakuEngine;
using namespace EaseUtil;

void ShieldEnemy::Init(Model* model, Camera* camera, const Vector3& position) {
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

	ChangeBehavior(new ShieldEnemyBehaviorRoot);
}

void ShieldEnemy::Update() {
	if (isDead_) {
		return;
	}

	if (behavior_) {
		behavior_->Update(this);
	}
}

void ShieldEnemy::Draw() {
	if (isDead_) {
		return;
	}
	model_->Draw(worldTransform_, *camera_);
}

void ShieldEnemy::OnCollision(Player* player) {
	if (behavior_->GetType() == IShieldEnemyBehaviorState::BehaviorType::kDead) {
		return;
	}

	if (player->IsAttack()) {
		// --- 先にエフェクトの情報を定義する ---
		
		// 敵と自キャラの中間位置にエフェクトを生成
		Vector3 effectPos = (worldTransform_.translation_ + player->GetWorldPosition()) / 2.0f;
		// ちょっと手前にずらす
		effectPos.z -= 0.5f;

		// 前方であればガード成功
		if (player->GetLRDirection() != lrDirection_) {
			gameScene_->CreateGuardEffect(effectPos);
			player->RequestKnockbackBehavior();
			ChangeBehavior(new ShieldEnemyBehaviorGuard);
			return;
		}

		// 敵の振る舞いをデス演出に変更
		ChangeBehavior(new ShieldEnemyBehaviorDead);
		gameScene_->CreateHitEffect(effectPos, Vector4{0.86f, 0.85f, 0.41f, 1.0f});
	}
}

KujakuEngine::Vector3 ShieldEnemy::GetWorldPosition() const {
	Vector3 worldPos;
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;
	return worldPos;
}

KujakuEngine::Vector3 ShieldEnemy::GetWorldRotation() const {
	Vector3 worldRot;
	worldRot.x = worldTransform_.rotation_.x;
	worldRot.y = worldTransform_.rotation_.y;
	worldRot.z = worldTransform_.rotation_.z;
	return worldRot;
}

AABB ShieldEnemy::GetAABB() const {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void ShieldEnemy::Move() {

	// タイマー加算
	walkTimer_ += 1.0f / 60.0f;

	// 回転アニメーション
	float param = std::sin(2.0f * std::numbers::pi_v<float> * walkTimer_ / 1.0f);
	float degree = kStartWalkAnimationRotationDegree + kEndWalkAnimationRotationDegree * (param + 1.0f) / 2.0f;

	float lr = 0.0f;
	switch (lrDirection_) {
	case LRDirection::kRight:
		lr = 90.0f;
		velocity_.x = kWalkSpeed;
		break;
	case LRDirection::kLeft:
		lr = -90.0f;
		velocity_.x = -kWalkSpeed;
		break;
	default:
		break;
	}

	worldTransform_.rotation_.y = (degree + lr) * std::numbers::pi_v<float> / 180.0f;

	// 移動
	worldTransform_.translation_ += velocity_;
}

void ShieldEnemy::UpdateTransform() {
	// 行列の更新
	worldTransform_.UpdateMatrix(*camera_);
}

void ShieldEnemy::ChangeBehavior(IShieldEnemyBehaviorState* behavior) {
	if (behavior_) {
		behavior_->OnExit(this);
	}

	behavior_.reset(behavior);

	if (behavior_) {
		behavior_->OnEnter(this);
	}
}

void ShieldEnemy::SetRotationEaseOut(const Vector3& startRotation, const Vector3& endRotation, float t) {
	worldTransform_.rotation_.x = EaseLerp(startRotation.x, endRotation.x, t, EaseType::OutQuad);
	worldTransform_.rotation_.y = EaseLerp(startRotation.y, endRotation.y, t, EaseType::OutQuad);
	worldTransform_.rotation_.z = EaseLerp(startRotation.z, endRotation.z, t, EaseType::OutQuad);
}

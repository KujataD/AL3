#include "EnemyBullet.h"
#include "Player.h"

using namespace KujakuEngine;

void EnemyBullet::Initialize(KujakuEngine::Model* model, KujakuEngine::Camera* camera, const KujakuEngine::Vector3& position, const KujakuEngine::Vector3& direction, bool isHoming) {
	assert(model);
	assert(camera);

	camera_ = camera;
	model_ = model;
	velocity_ = direction * Param::speed_;
	isHoming_ = isHoming;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {0.5f, 0.5f, 3.0f};

	model_->SetColor({1.0f, 0.0f, 0.0f, 1.0f});

	// デスタイマー
	deathTimer_ = Param::lifeTime_;

	// 向きを合わせる
	ApplyRotationOfVelocity();
}

void EnemyBullet::Update() {
	deathTimer_ -= 1.0f / 60.0f;
	if (deathTimer_ <= 0.0f) {
		isDead_ = true;
	}

	if (isHoming_) {
		Vector3 toPlayer = player_->GetWorldPosition() - worldTransform_.GetWorldPosition();

		velocity_ = Slerp(velocity_, toPlayer, Param::handlingPower) * Param::speed_;

		// 向きを合わせる
		ApplyRotationOfVelocity();
	}

	worldTransform_.translation_ += velocity_;
	worldTransform_.UpdateMatrix(*camera_);
}

void EnemyBullet::Draw() { model_->Draw(worldTransform_, *camera_); }

void EnemyBullet::RegisterGlobalVariables() {}

void EnemyBullet::ApplyGlobalVariables() {}

void EnemyBullet::ApplyRotationOfVelocity() {
	// Y軸周り角度(θy) ...atan2(高さ, 底辺)
	worldTransform_.rotation_.y = std::atan2(velocity_.x, velocity_.z);
	// 横軸方向の長さを求める
	float velocityXZ = Length({velocity_.x, 0.0f, velocity_.z});
	// X軸周り角度(θx)
	worldTransform_.rotation_.x = std::atan2(-velocity_.y, velocityXZ);
}

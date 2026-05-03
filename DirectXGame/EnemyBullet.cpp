#include "EnemyBullet.h"

void EnemyBullet::Initialize(KujakuEngine::Model* model, KujakuEngine::Camera* camera, const KujakuEngine::Vector3& position, const KujakuEngine::Vector3& velocity) {
	assert(model);
	assert(camera);

	camera_ = camera;
	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	model_->SetColor({1.0f, 0.0f, 0.0f, 1.0f});
	velocity_ = velocity;
	deathTimer_ = Param::lifeTime_;
}

void EnemyBullet::Update() {
	deathTimer_ -= 1.0f / 60.0f;
	if (deathTimer_ <= 0.0f) {
		isDead_ = true;
	}

	worldTransform_.translation_ += velocity_;
	worldTransform_.UpdateMatrix(*camera_);
}

void EnemyBullet::Draw() { model_->Draw(worldTransform_, *camera_); }

void EnemyBullet::RegisterGlobalVariables() {}

void EnemyBullet::ApplyGlobalVariables() {}

#include "PlayerBullet.h"

void PlayerBullet::Initialize(KujakuEngine::Model* model, KujakuEngine::Camera* camera, const KujakuEngine::Vector3& position) {
	assert(model);
	assert(camera);

	camera_ = camera;
	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
}

void PlayerBullet::Update() { worldTransform_.UpdateMatrix(*camera_); }

void PlayerBullet::Draw() { model_->Draw(worldTransform_, *camera_); }

void PlayerBullet::RegisterGlobalVariant() {}

void PlayerBullet::ApplyGlobalVariant() {}

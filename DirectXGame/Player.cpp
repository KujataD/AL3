#include "Player.h"

void Player::Initialize(KujakuEngine::Model* model, KujakuEngine::Camera* camera) {
	assert(model);
	assert(camera);

	// 各要素のセット
	camera_ = camera;
	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.UpdateMatrix(*camera_);
}

void Player::Update() { worldTransform_.UpdateMatrix(*camera_); }

void Player::Draw() { model_->Draw(worldTransform_, *camera_); }

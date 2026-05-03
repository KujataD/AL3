#include "Player.h"

void Player::Initialize(KujakuEngine::Model* model, KujakuEngine::Camera* camera) {
	assert(model);
	assert(camera);

	// カメラの設定
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.UpdateMatrix(*camera_);
}

void Player::Update() { worldTransform_.UpdateMatrix(*camera_); }

void Player::Draw() { model_->Draw(worldTransform_, *camera_); }

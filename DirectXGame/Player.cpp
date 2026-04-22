#include "Player.h"

void Player::Init(KujakuEngine::Model* model, KujakuEngine::Camera* camera) { // nullptr check
	assert(model);

	// 引数の記録
	model_ = model;
	camera_ = camera;

	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = {1.0f, 1.0f, 0.0f};
}

void Player::Update() {

	worldTransform_.rotation_.y += 0.03f;
	worldTransform_.UpdateMatrix(*camera_);
}

void Player::Draw() { model_->Draw(worldTransform_, *camera_); }

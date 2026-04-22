#include "Player.h"

void Player::Init(KujakuEngine::Model* model, KujakuEngine::Camera* camera) {// nullptr check
	assert(model);

	// 引数の記録
	model_ = model;
	camera_ = camera;

	// ワールド変換の初期化
	worldTransform_.Initialize();
}

void Player::Update() { worldTransform_.UpdateMatrix(*camera_); }

void Player::Draw() {
	model_->Draw(worldTransform_, *camera_); }

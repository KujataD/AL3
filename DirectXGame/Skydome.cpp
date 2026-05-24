#include "Skydome.h"

void Skydome::Initialize(KujakuEngine::Model* model, KujakuEngine::Camera* camera) {
	// nullptr check
	assert(model);

	// 引数の記録
	model_ = model;
	camera_ = camera;

	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = {1.5f, 1.5f, 1.5f};
	worldTransform_.translation_ = {0.0f, 0.0f, 50.0f};
}

void Skydome::Update() { worldTransform_.UpdateMatrix(*camera_); }

void Skydome::Draw() { model_->Draw(worldTransform_, *camera_); }

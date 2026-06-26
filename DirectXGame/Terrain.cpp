#include "Terrain.h"

void Terrain::Initialize(KujakuEngine::Model* model, KujakuEngine::Camera* camera) {
	// nullptr check
	assert(model);

	// 引数の記録
	model_ = model;
	camera_ = camera;

	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = {1.5f, 1.0f, 1.5f};
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 0.5f;
}

void Terrain::Update() { worldTransform_.UpdateMatrix(*camera_); }

void Terrain::Draw() { model_->Draw(worldTransform_, *camera_); }

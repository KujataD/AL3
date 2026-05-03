#include "Enemy.h"

using namespace KujakuEngine;

void Enemy::Initialize(KujakuEngine::Model* model, KujakuEngine::Camera* camera, KujakuEngine::Vector3 position) {
	assert(model);
	assert(camera);

	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
}

void Enemy::Update() {
	worldTransform_.translation_.z -= Param::speed_;
	worldTransform_.UpdateMatrix(*camera_);
}

void Enemy::Draw() { model_->Draw(worldTransform_, *camera_); }

void Enemy::RegisterGlobalVariables() {
	GlobalVariables* gv = GlobalVariables::GetInstance();
	gv->AddItem(ParamKey::kGroup, ParamKey::kSpeed, Param::speed_);
}

void Enemy::ApplyGlobalVariables() {
	GlobalVariables* gv = GlobalVariables::GetInstance();
	Param::speed_ = gv->GetValue<float>(ParamKey::kGroup, ParamKey::kSpeed);
}

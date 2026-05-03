#include "Enemy.h"

using namespace KujakuEngine;

void (Enemy::*Enemy::phaseFuncTable[])() = {
	&Enemy::Approach,
	&Enemy::Leave
};

void Enemy::Initialize(KujakuEngine::Model* model, KujakuEngine::Camera* camera, KujakuEngine::Vector3 position) {
	assert(model);
	assert(camera);

	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
}

void Enemy::Update() {

	// 各フェーズの関数を呼び出す
	(this->*phaseFuncTable[static_cast<size_t>(phase_)])();

	worldTransform_.UpdateMatrix(*camera_);
}

void Enemy::Draw() { model_->Draw(worldTransform_, *camera_); }

void Enemy::RegisterGlobalVariables() {
	GlobalVariables* gv = GlobalVariables::GetInstance();
	gv->AddItem(ParamKey::kGroup, ParamKey::kApproachVelocity, Param::approachVelocity_);
	gv->AddItem(ParamKey::kGroup, ParamKey::kLeaveVelocity, Param::leaveVelocity_);
}

void Enemy::ApplyGlobalVariables() {
	GlobalVariables* gv = GlobalVariables::GetInstance();
	Param::approachVelocity_ = gv->GetValue<Vector3>(ParamKey::kGroup, ParamKey::kApproachVelocity);
	Param::leaveVelocity_ = gv->GetValue<Vector3>(ParamKey::kGroup, ParamKey::kLeaveVelocity);
}

void Enemy::Approach() {
	worldTransform_.translation_ += Param::approachVelocity_;
	if (worldTransform_.translation_.z <= 0.0f) {
		phase_ = Phase::Leave;
	}
}

void Enemy::Leave() { worldTransform_.translation_ += Param::leaveVelocity_; }

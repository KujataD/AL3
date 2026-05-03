#include "Enemy.h"
#include "EnemyStateApproach.h"

using namespace KujakuEngine;


void Enemy::Initialize(KujakuEngine::Model* model, KujakuEngine::Camera* camera, KujakuEngine::Vector3 position) {
	assert(model);
	assert(camera);

	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	ChangeState(std::make_unique<EnemyStateApproach>(this));
}

void Enemy::Update() {

	// 各フェーズの関数を呼び出す
	state_->Update();

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
}

void Enemy::Leave() { worldTransform_.translation_ += Param::leaveVelocity_; }

void Enemy::ChangeState(std::unique_ptr<BaseEnemyState> state) {
	state_ = std::move(state);
	state_->DebugLog();
}

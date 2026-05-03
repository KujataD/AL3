#include "PlayerBullet.h"

using namespace KujakuEngine;

void PlayerBullet::Initialize(KujakuEngine::Model* model, KujakuEngine::Camera* camera, const KujakuEngine::Vector3& position, const KujakuEngine::Vector3& velocity) {
	assert(model);
	assert(camera);

	camera_ = camera;
	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {0.5f, 2.0f, 1.0f};
	model_->SetColor({Param::colorRGB_.x, Param::colorRGB_.y, Param::colorRGB_.z, 1.0f});
	velocity_ = velocity;
	deathTimer_ = Param::lifeTime_;
}

void PlayerBullet::Update() {
	deathTimer_ -= 1.0f / 60.0f;
	if (deathTimer_ <= 0.0f) {
		isDead_ = true;
	}

	worldTransform_.translation_ += velocity_;
	worldTransform_.UpdateMatrix(*camera_);
}

void PlayerBullet::Draw() { model_->Draw(worldTransform_, *camera_); }

void PlayerBullet::RegisterGlobalVariables() {
	GlobalVariables* gv = GlobalVariables::GetInstance();
	gv->AddItem(ParamKey::kGroup, ParamKey::kColorRGB, Param::colorRGB_);
	gv->AddItem(ParamKey::kGroup, ParamKey::kLifeTime, Param::lifeTime_);
}

void PlayerBullet::ApplyGlobalVariables() {
	GlobalVariables* gv = GlobalVariables::GetInstance();
	Param::colorRGB_ = gv->GetValue<Vector3>(ParamKey::kGroup, ParamKey::kColorRGB);
	Param::lifeTime_ = gv->GetValue<float>(ParamKey::kGroup, ParamKey::kLifeTime);
}

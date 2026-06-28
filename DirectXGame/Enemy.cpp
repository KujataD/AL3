#include "Enemy.h"
#include <cmath>
#include <numbers>

using namespace KujakuEngine;

Enemy::~Enemy() {}

void Enemy::Initialize(const std::vector<Model*>& models, const Camera* camera) {
	assert(models.size() == kModelIndexCount);
	for (Model* model : models) {
		assert(model);
	}
	assert(camera);

	BaseCharacter::Initialize(models, camera);
	assert(models_.size() == kModelIndexCount);

	worldTransform_.translation_ = {0.0f, 0.0f, 12.0f};
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	BaseCharacter::Update();

	worldTransformBody_.Initialize();
	worldTransformBody_.parent_ = &worldTransform_;
	worldTransformBody_.translation_ = Param::offsetTranslateBody_;
	worldTransformBody_.rotation_ = Param::offsetRotateBody_;

	worldTransformArm_L_.Initialize();
	worldTransformArm_L_.parent_ = &worldTransformBody_;
	worldTransformArm_L_.translation_ = Param::offsetTranslateArm_L_;
	worldTransformArm_L_.rotation_ = Param::offsetRotateArm_L_;

	worldTransformArm_R_.Initialize();
	worldTransformArm_R_.parent_ = &worldTransformBody_;
	worldTransformArm_R_.translation_ = Param::offsetTranslateArm_R_;
	worldTransformArm_R_.rotation_ = Param::offsetRotateArm_R_;
}

void Enemy::Update() {
	Move();

	worldTransformBody_.translation_ = Param::offsetTranslateBody_;
	worldTransformBody_.rotation_ = Param::offsetRotateBody_;
	worldTransformArm_L_.translation_ = Param::offsetTranslateArm_L_;
	worldTransformArm_L_.rotation_ = Param::offsetRotateArm_L_;
	worldTransformArm_R_.translation_ = Param::offsetTranslateArm_R_;
	worldTransformArm_R_.rotation_ = Param::offsetRotateArm_R_;

	BaseCharacter::Update();
	worldTransformBody_.UpdateMatrix(*camera_);
	worldTransformArm_L_.UpdateMatrix(*camera_);
	worldTransformArm_R_.UpdateMatrix(*camera_);
}

void Enemy::Draw() {
	assert(models_.size() == kModelIndexCount);
	models_[kModelIndexBody]->Draw(worldTransformBody_, *camera_);
	models_[kModelIndexArm_L]->Draw(worldTransformArm_L_, *camera_);
	models_[kModelIndexArm_R]->Draw(worldTransformArm_R_, *camera_);
}

void Enemy::SetCamera(const Camera* camera) {
	camera_ = camera;
	BaseCharacter::Update();
	worldTransformBody_.UpdateMatrix(*camera_);
	worldTransformArm_L_.UpdateMatrix(*camera_);
	worldTransformArm_R_.UpdateMatrix(*camera_);
}

void Enemy::RegisterGlobalVariables() {
	GlobalVariables* gv = GlobalVariables::GetInstance();

	gv->AddItem(ParamKey::kGroupKey, ParamKey::kMoveSpeed, Param::moveSpeed_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kRotateSpeed, Param::rotateSpeed_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kRotationOffsetY, Param::rotationOffsetY_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kOffsetTranslateBody, Param::offsetTranslateBody_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kOffsetTranslateArm_L, Param::offsetTranslateArm_L_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kOffsetTranslateArm_R, Param::offsetTranslateArm_R_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kOffsetRotateBody, Param::offsetRotateBody_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kOffsetRotateArm_L, Param::offsetRotateArm_L_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kOffsetRotateArm_R, Param::offsetRotateArm_R_);
}

void Enemy::ApplyGlobalVariables() {
	GlobalVariables* gv = GlobalVariables::GetInstance();

	Param::moveSpeed_ = gv->GetValue<float>(ParamKey::kGroupKey, ParamKey::kMoveSpeed);
	Param::rotateSpeed_ = gv->GetValue<float>(ParamKey::kGroupKey, ParamKey::kRotateSpeed);
	Param::rotationOffsetY_ = gv->GetValue<float>(ParamKey::kGroupKey, ParamKey::kRotationOffsetY);
	Param::offsetTranslateBody_ = gv->GetValue<Vector3>(ParamKey::kGroupKey, ParamKey::kOffsetTranslateBody);
	Param::offsetTranslateArm_L_ = gv->GetValue<Vector3>(ParamKey::kGroupKey, ParamKey::kOffsetTranslateArm_L);
	Param::offsetTranslateArm_R_ = gv->GetValue<Vector3>(ParamKey::kGroupKey, ParamKey::kOffsetTranslateArm_R);
	Param::offsetRotateBody_ = gv->GetValue<Vector3>(ParamKey::kGroupKey, ParamKey::kOffsetRotateBody);
	Param::offsetRotateArm_L_ = gv->GetValue<Vector3>(ParamKey::kGroupKey, ParamKey::kOffsetRotateArm_L);
	Param::offsetRotateArm_R_ = gv->GetValue<Vector3>(ParamKey::kGroupKey, ParamKey::kOffsetRotateArm_R);
}

void Enemy::Move() {
	float moveDirectionY = worldTransform_.rotation_.y - Param::rotationOffsetY_;
	moveDirectionY += Param::rotateSpeed_;
	moveDirectionY = WrapAngle(moveDirectionY);

	Vector3 move = {std::sin(moveDirectionY), 0.0f, std::cos(moveDirectionY)};
	move *= Param::moveSpeed_;

	worldTransform_.translation_ += move;
	if (Length(move) > 0.001f) {
		worldTransform_.rotation_.y = LookAt(move).y + Param::rotationOffsetY_;
	}
}

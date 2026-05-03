#include "Enemy.h"
#include "EnemyStateApproach.h"

using namespace KujakuEngine;

void Enemy::Initialize(KujakuEngine::Model* model, KujakuEngine::Model* modelBullet, KujakuEngine::Camera* camera, KujakuEngine::Vector3 position) {
	assert(model);
	assert(modelBullet);
	assert(camera);

	model_ = model;
	camera_ = camera;
	modelBullet_ = modelBullet;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	ChangeState(std::make_unique<EnemyStateApproach>(this));
	InitApproach();
}

void Enemy::Update() {

	// 各フェーズの関数を呼び出す
	state_->Update();

	UpdateBullets();

	worldTransform_.UpdateMatrix(*camera_);
}

void Enemy::Draw() {
	model_->Draw(worldTransform_, *camera_);
	for (auto& bullet : bullets_) {
		bullet->Draw();
	}
}

void Enemy::RegisterGlobalVariables() {
	GlobalVariables* gv = GlobalVariables::GetInstance();
	gv->AddItem(ParamKey::kGroup, ParamKey::kApproachVelocity, Param::approachVelocity_);
	gv->AddItem(ParamKey::kGroup, ParamKey::kLeaveVelocity, Param::leaveVelocity_);
	gv->AddItem(ParamKey::kGroup, ParamKey::kBulletSpeed, Param::bulletSpeed_);
	gv->AddItem(ParamKey::kGroup, ParamKey::kBulletFireDuration, Param::bulletFireDuration_);
}

void Enemy::ApplyGlobalVariables() {
	GlobalVariables* gv = GlobalVariables::GetInstance();
	Param::approachVelocity_ = gv->GetValue<Vector3>(ParamKey::kGroup, ParamKey::kApproachVelocity);
	Param::leaveVelocity_ = gv->GetValue<Vector3>(ParamKey::kGroup, ParamKey::kLeaveVelocity);
	Param::bulletSpeed_ = gv->GetValue<float>(ParamKey::kGroup, ParamKey::kBulletSpeed);
	Param::bulletFireDuration_ = gv->GetValue<float>(ParamKey::kGroup, ParamKey::kBulletFireDuration);
}

void Enemy::Approach() {
	worldTransform_.translation_ += Param::approachVelocity_;
	fireTimer -= kDT;
	if (fireTimer <= 0.0f) {
		// 弾発射
		Fire();
		// タイマー初期化
		fireTimer = Param::bulletFireDuration_;
	}
}

void Enemy::InitApproach() { fireTimer = Param::bulletFireDuration_; }

void Enemy::Leave() { worldTransform_.translation_ += Param::leaveVelocity_; }

void Enemy::Fire() {

	// 弾の速度
	Vector3 velocity(0, 0, -Param::bulletSpeed_);

	// 速度ベクトルを自機の向きに合わせて回転させる
	velocity = TransformNormal(velocity, worldTransform_.matWorld_);

	// 弾を生成し、初期化
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(modelBullet_, camera_, worldTransform_.translation_, velocity);

	// 弾を登録する
	bullets_.push_back(std::move(newBullet));
}

void Enemy::ChangeState(std::unique_ptr<BaseEnemyState> state) {
	state_ = std::move(state);
	state_->DebugLog();
}

void Enemy::UpdateBullets() {

	// 更新
	for (auto& bullet : bullets_) {
		bullet->Update();
	}

	// デスフラグの立った弾を排除
	bullets_.remove_if([](std::unique_ptr<EnemyBullet>& bullet) {
		if (bullet->IsDead()) {
			return true;
		}
		return false;
	});
}

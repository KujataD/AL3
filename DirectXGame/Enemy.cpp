#include "Enemy.h"
#include "Player.h"
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
	worldTransform_.UpdateMatrix(*camera_);

	ChangeState(std::make_unique<EnemyStateApproach>(this));
}

void Enemy::Update() {

	// 終了イベント削除
	timedCalls_.remove_if([](const std::unique_ptr<TimedCall>& timedCall) { return timedCall->IsFinished(); });
	
	// 更新イテレータ
	for(auto& timedCall : timedCalls_){
		timedCall->Update();
	}

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
	gv->AddItem(ParamKey::kGroup, ParamKey::kBulletFireDuration, Param::bulletFireDuration_);
}

void Enemy::ApplyGlobalVariables() {
	GlobalVariables* gv = GlobalVariables::GetInstance();
	Param::approachVelocity_ = gv->GetValue<Vector3>(ParamKey::kGroup, ParamKey::kApproachVelocity);
	Param::leaveVelocity_ = gv->GetValue<Vector3>(ParamKey::kGroup, ParamKey::kLeaveVelocity);
	Param::bulletFireDuration_ = gv->GetValue<float>(ParamKey::kGroup, ParamKey::kBulletFireDuration);
}

void Enemy::Approach() { worldTransform_.translation_ += Param::approachVelocity_; }

void Enemy::InitApproach() { FireAndTimerReset(); }

void Enemy::Leave() { worldTransform_.translation_ += Param::leaveVelocity_; }

void Enemy::Fire() {

	assert(player_);

	// 弾の速度
	Vector3 direction = player_->GetWorldPosition() - GetWorldPosition();
	direction = Normalize(direction);

	// 弾を生成し、初期化
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
	newBullet->SetPlayer(player_);
	newBullet->Initialize(modelBullet_, camera_, worldTransform_.translation_, direction, true);

	// 弾を登録する
	bullets_.push_back(std::move(newBullet));
}

void Enemy::ChangeState(std::unique_ptr<BaseEnemyState> state) {
	state_ = std::move(state);
	state_->DebugLog();
}

void Enemy::FireAndTimerReset() {
	// 弾を発射する
	Fire();

	// メンバ関数と呼び出し元をbindしてstd::functionに代入
	std::function<void(void)> callback = std::bind(&Enemy::FireAndTimerReset, this);
	// 時限発動イベントを生成
	std::unique_ptr<TimedCall> timedCall = std::make_unique<TimedCall>(callback, Param::bulletFireDuration_);
	// 時限発動イベントを時限発動イベントリストに追加
	timedCalls_.push_back(std::move(timedCall));
}

void Enemy::ClearFireTimer() { timedCalls_.clear(); }

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

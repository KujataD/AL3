#include "Player.h"
#include <algorithm>
#include <numbers>

using namespace KujakuEngine;

void Player::Initialize(KujakuEngine::Model* model, KujakuEngine::Camera* camera) {
	assert(model);
	assert(camera);

	// 各要素のセット
	camera_ = camera;
	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.rotation_.y = std::numbers::pi_v<float>;
	worldTransform_.UpdateMatrix(*camera_);
}

void Player::Update() {
	// 移動ベクトル
	Vector3 move = {0, 0, 0};

	if (Input::GetKey(DIK_LEFT)) {
		move.x -= 1.0f;
	} else if (Input::GetKey(DIK_RIGHT)) {
		move.x += 1.0f;
	}
	if (Input::GetKey(DIK_DOWN)) {
		move.y -= 1.0f;
	} else if (Input::GetKey(DIK_UP)) {
		move.y += 1.0f;
	}

	worldTransform_.translation_ += Vector3::Normalize(move) * Param::speed_;

	// Imgui
#ifdef USE_IMGUI
	ImGui::Begin("Player");
	ImGui::DragFloat3("Translate", &worldTransform_.translation_.x, 0.01f);
	ImGui::End();
#endif // USE_IMGUI

	Rect cameraVisible = camera_->GetVisibleRect(worldTransform_.translation_.z);
	worldTransform_.translation_.x = std::clamp(worldTransform_.translation_.x, cameraVisible.left + Param::moveLimitBlank_, cameraVisible.right - Param::moveLimitBlank_);
	worldTransform_.translation_.y = std::clamp(worldTransform_.translation_.y, cameraVisible.bottom + Param::moveLimitBlank_, cameraVisible.top - Param::moveLimitBlank_);

	worldTransform_.UpdateMatrix(*camera_);
}

void Player::Draw() { model_->Draw(worldTransform_, *camera_); }

void Player::RegisterGlobalVariables() {
	GlobalVariables* gv = GlobalVariables::GetInstance();
	gv->AddItem(ParamKeys::kGroupKey, ParamKeys::kSpeedKey, Param::speed_);
	gv->AddItem(ParamKeys::kGroupKey, ParamKeys::kMoveLimitBlank, Param::moveLimitBlank_);
}

void Player::ApplyGlobalVariables() {
	GlobalVariables* gv = GlobalVariables::GetInstance();
	Param::speed_ = gv->GetValue<float>(ParamKeys::kGroupKey, ParamKeys::kSpeedKey);
	Param::moveLimitBlank_ = gv->GetValue<float>(ParamKeys::kGroupKey, ParamKeys::kMoveLimitBlank);
}

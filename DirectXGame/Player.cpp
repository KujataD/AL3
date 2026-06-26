#include "Player.h"
#include <algorithm>
#include <cmath>
#include <numbers>

using namespace KujakuEngine;

Player::~Player() {}

void Player::Initialize(KujakuEngine::Model* model, KujakuEngine::Camera* camera) {
	assert(model);
	assert(camera);

	// 各要素のセット
	camera_ = camera;
	model_ = model;

	// 各ワールドトランスフォームの初期化・設定
	// ---------------------------------------------
	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.rotation_.y = std::numbers::pi_v<float>;
	worldTransform_.UpdateMatrix(*camera_);

	// 衝突設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(~kCollisionAttributePlayer);
}

void Player::Update() {
	UpdateControlType();

	// IMGUI
	ManageImGui();

	// 移動処理
	Move();

	// トランスフォーム更新
	worldTransform_.UpdateMatrix(*camera_);
}

void Player::Draw() {
	model_->Draw(worldTransform_, *camera_);
}

void Player::RegisterGlobalVariables() {
	GlobalVariables* gv = GlobalVariables::GetInstance();

	gv->AddItem(ParamKey::kGroupKey, ParamKey::kSpeed, Param::speed_);
}

void Player::ApplyGlobalVariables() {
	GlobalVariables* gv = GlobalVariables::GetInstance();

	Param::speed_ = gv->GetValue<float>(ParamKey::kGroupKey, ParamKey::kSpeed);
}

void Player::OnCollision() {}

void Player::Move() { 
}

void Player::ManageImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("Player");
	ImGui::DragFloat3("Translate", &worldTransform_.translation_.x, 0.01f);
	ImGui::End();
#endif // USE_IMGUI
}

void Player::UpdateControlType() {
	// マウス座標が異なる場合はキーボード操作とみなす
	if (Input::GetMouseClientPos() != Input::GetMousePreClientPos()) {
		controlType_ = ControlType::kControlTypeKeyboard;
	}

	// 右スティックの入力がある場合はゲームパッド操作とみなす
	if (Vector2::Length(Input::GetRightStick()) > 0.0f) {
		controlType_ = ControlType::kControlTypeGamepad;
	}
}

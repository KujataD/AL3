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

void Player::Draw() { model_->Draw(worldTransform_, *camera_); }

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
	Vector3 move{};
	if (controlType_ == ControlType::kControlTypeGamepad) {
		// コントローラー操作
		Vector2 leftStick = Input::GetLeftStick();
		move = {leftStick.x, 0.0f, leftStick.y};
	} else {
		// キーボード操作
		
		if (Input::GetKey(DIK_W)) {
			move.z += 1.0f;
		}
		if (Input::GetKey(DIK_S)) {
			move.z -= 1.0f;
		}
		if (Input::GetKey(DIK_D)) {
			move.x += 1.0f;
		}
		if (Input::GetKey(DIK_A)) {
			move.x -= 1.0f;
		}

		if (Length(move) > 0.0f) {
			move = Normalize(move);
		}
	}

	if (Length(move) == 0.0f) {
		return;
	}

	// 移動ベクトルをカメラの角度だけ回転する
	Matrix4x4 matRotate = MakeIdentity();
	if (viewProjection_) {
		matRotate = MakeRotateYMatrix(viewProjection_->rotation_.y);
	}
	move = TransformNormal(move, matRotate);
	move *= Param::speed_;

	worldTransform_.translation_ += move;
	if (Length(move) > 0.001f) {
		worldTransform_.rotation_.y = LookAt(move).y;
	}
}

void Player::ManageImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("Player");
	ImGui::DragFloat3("Translate", &worldTransform_.translation_.x, 0.01f);
	ImGui::End();
#endif // USE_IMGUI
}

void Player::UpdateControlType() {
	if (Input::IsControllerInput()) {
		controlType_ = ControlType::kControlTypeGamepad;
		return;
	}

	controlType_ = ControlType::kControlTypeKeyboard;
}

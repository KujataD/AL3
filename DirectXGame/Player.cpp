#include "Player.h"
#include <algorithm>
#include <cmath>
#include <numbers>

using namespace KujakuEngine;

Player::~Player() {}

void Player::Initialize(const std::vector<Model*>& models, const Camera* camera) {
	assert(models.size() == kModelIndexCount);
	for (Model* model : models) {
		assert(model);
	}
	assert(camera);

	// 各要素のセット
	BaseCharacter::Initialize(models, camera);
	assert(models_.size() == kModelIndexCount);

	// 各ワールドトランスフォームの初期化・設定
	// ---------------------------------------------
	worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };
	worldTransform_.rotation_.y = std::numbers::pi_v<float>;
	BaseCharacter::Update();

	worldTransformBody_.Initialize();
	worldTransformBody_.parent_ = &worldTransform_;
	worldTransformBody_.translation_ = Param::offsetTranslateBody_;

	worldTransformHead_.Initialize();
	worldTransformHead_.parent_ = &worldTransformBody_;
	worldTransformHead_.translation_ = Param::offsetTranslateHead_;

	worldTransformArm_L_.Initialize();
	worldTransformArm_L_.parent_ = &worldTransformBody_;
	worldTransformArm_L_.translation_ = Param::offsetTranslateArm_L_;
	worldTransformArm_L_.rotation_ = Param::offsetRotateArm_L_;

	worldTransformArm_R_.Initialize();
	worldTransformArm_R_.parent_ = &worldTransformBody_;
	worldTransformArm_R_.translation_ = Param::offsetTranslateArm_R_;
	worldTransformArm_R_.rotation_ = Param::offsetRotateArm_R_;

	// 衝突設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(~kCollisionAttributePlayer);
	InitializeFloatingGimmick();
}

void Player::Update() {
	UpdateControlType();

	// IMGUI
	ManageImGui();

	// 移動処理
	Move();

	worldTransformBody_.translation_ = Param::offsetTranslateBody_;
	worldTransformHead_.translation_ = Param::offsetTranslateHead_;
	worldTransformArm_L_.translation_ = Param::offsetTranslateArm_L_;
	worldTransformArm_R_.translation_ = Param::offsetTranslateArm_R_;
	worldTransformArm_L_.rotation_ = Param::offsetRotateArm_L_;
	worldTransformArm_R_.rotation_ = Param::offsetRotateArm_R_;


	// 浮遊ギミック更新
	UpdateFloatingGimmick();

	// トランスフォーム更新
	BaseCharacter::Update();
	worldTransformBody_.UpdateMatrix(*camera_);
	worldTransformHead_.UpdateMatrix(*camera_);
	worldTransformArm_L_.UpdateMatrix(*camera_);
	worldTransformArm_R_.UpdateMatrix(*camera_);
}

void Player::Draw() {
	assert(models_.size() == kModelIndexCount);
	models_[kModelIndexBody]->Draw(worldTransformBody_, *camera_);
	models_[kModelIndexHead]->Draw(worldTransformHead_, *camera_);
	models_[kModelIndexArm_L]->Draw(worldTransformArm_L_, *camera_);
	models_[kModelIndexArm_R]->Draw(worldTransformArm_R_, *camera_);
}

void Player::RegisterGlobalVariables() {
	GlobalVariables* gv = GlobalVariables::GetInstance();

	gv->AddItem(ParamKey::kGroupKey, ParamKey::kSpeed, Param::speed_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kFloatingAmplitude, Param::floatingAmplitude_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kFloatingArmRotationAmplitude, Param::floatingArmRotationAmplitude_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kFloatingCycle, Param::floatingCycle_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kOffsetTranslateBody, Param::offsetTranslateBody_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kOffsetTranslateHead, Param::offsetTranslateHead_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kOffsetTranslateArm_L, Param::offsetTranslateArm_L_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kOffsetTranslateArm_R, Param::offsetTranslateArm_R_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kOffsetRotateArm_L, Param::offsetRotateArm_L_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kOffsetRotateArm_R, Param::offsetRotateArm_R_);
}

void Player::ApplyGlobalVariables() {
	GlobalVariables* gv = GlobalVariables::GetInstance();

	Param::speed_ = gv->GetValue<float>(ParamKey::kGroupKey, ParamKey::kSpeed);
	Param::floatingAmplitude_ = gv->GetValue<float>(ParamKey::kGroupKey, ParamKey::kFloatingAmplitude);
	Param::floatingArmRotationAmplitude_ = gv->GetValue<float>(ParamKey::kGroupKey, ParamKey::kFloatingArmRotationAmplitude);
	Param::floatingCycle_ = gv->GetValue<int32_t>(ParamKey::kGroupKey, ParamKey::kFloatingCycle);
	Param::offsetTranslateBody_ = gv->GetValue<Vector3>(ParamKey::kGroupKey, ParamKey::kOffsetTranslateBody);
	Param::offsetTranslateHead_ = gv->GetValue<Vector3>(ParamKey::kGroupKey, ParamKey::kOffsetTranslateHead);
	Param::offsetTranslateArm_L_ = gv->GetValue<Vector3>(ParamKey::kGroupKey, ParamKey::kOffsetTranslateArm_L);
	Param::offsetTranslateArm_R_ = gv->GetValue<Vector3>(ParamKey::kGroupKey, ParamKey::kOffsetTranslateArm_R);
	Param::offsetRotateArm_L_ = gv->GetValue<Vector3>(ParamKey::kGroupKey, ParamKey::kOffsetRotateArm_L);
	Param::offsetRotateArm_R_ = gv->GetValue<Vector3>(ParamKey::kGroupKey, ParamKey::kOffsetRotateArm_R);
}

void Player::OnCollision() {}

void Player::Move() {
	Vector3 move{};
	if (controlType_ == ControlType::kControlTypeGamepad) {
		// コントローラー操作
		Vector2 leftStick = Input::GetLeftStick();
		move = { leftStick.x, 0.0f, leftStick.y };
	}
	else {
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

void Player::InitializeFloatingGimmick() {
	floatingParameter_ = 0.0f;
}

void Player::UpdateFloatingGimmick() {
	if (Param::floatingCycle_ <= 0) {
		floatingParameter_ = 0.0f;
		return;
	}

	// 浮遊移動のサイクル<frame>
	const float cycle = static_cast<float>(Param::floatingCycle_);
	// 1フレームでのパラメータ加算値
	const float step = 2.0f * std::numbers::pi_v<float> / cycle;
	floatingParameter_ += step;

	// 浮遊の振幅<m>
	const float amplitude = Param::floatingAmplitude_;

	// 浮遊を座標に反映
	const float floatingTranslationY = std::sin(floatingParameter_) * amplitude;
	worldTransformBody_.translation_.y += floatingTranslationY;

	const float armRotationAmplitude = Param::floatingArmRotationAmplitude_;
	const float floatingArmRotationX = std::sin(floatingParameter_) * armRotationAmplitude;
	worldTransformArm_L_.rotation_.x = floatingArmRotationX;
	worldTransformArm_R_.rotation_.x = -floatingArmRotationX;
}

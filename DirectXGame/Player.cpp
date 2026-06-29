#include "Player.h"
#include <algorithm>
#include <cmath>
#include <numbers>

using namespace KujakuEngine;

namespace {
float CalculateFrameProgress(float currentFrame, int durationFrame) {
	if (durationFrame <= 0) {
		return 1.0f;
	}

	const float progress = currentFrame / static_cast<float>(durationFrame);
	return std::clamp(progress, 0.0f, 1.0f);
}
} // namespace

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

	worldTransformShoulder_.Initialize();
	worldTransformShoulder_.parent_ = &worldTransformBody_;
	worldTransformShoulder_.translation_.y = Param::offsetTranslateArm_L_.y;

	worldTransformHead_.Initialize();
	worldTransformHead_.parent_ = &worldTransformBody_;
	worldTransformHead_.translation_ = Param::offsetTranslateHead_;

	worldTransformArm_L_.Initialize();
	worldTransformArm_L_.parent_ = &worldTransformShoulder_;
	worldTransformArm_L_.translation_ = Param::offsetTranslateArm_L_;
	worldTransformArm_L_.translation_.y = 0.0f;
	worldTransformArm_L_.rotation_ = Param::offsetRotateArm_L_;

	worldTransformArm_R_.Initialize();
	worldTransformArm_R_.parent_ = &worldTransformShoulder_;
	worldTransformArm_R_.translation_ = Param::offsetTranslateArm_R_;
	worldTransformArm_R_.translation_.y = 0.0f;
	worldTransformArm_R_.rotation_ = Param::offsetRotateArm_R_;

	worldTransformWeapon_.Initialize();
	worldTransformWeapon_.parent_ = &worldTransformShoulder_;

	// 衝突設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(~kCollisionAttributePlayer);
	InitializeFloatingGimmick();
}

void Player::Update() {
	if (behaviorRequest_) {
		// 振舞を変更する
		behavior_ = behaviorRequest_.value();

		switch (behavior_) {
		case Player::Behavior::kRoot:
			BehaviorRootInitialize();
			break;
		case Player::Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		default:
			break;
		}

		// 振舞リクエストをリセット
		behaviorRequest_ = std::nullopt;
	}
	
	switch (behavior_) {
	case Player::Behavior::kRoot:
		BehaviorRootUpdate();
		break;
	case Player::Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	default:
		break;
	}

	// IMGUI
	ManageImGui();

	UpdateControlType();

	// トランスフォーム更新
	UpdateWorldTransforms();
}

void Player::Draw() {
	assert(models_.size() == kModelIndexCount);
	models_[kModelIndexBody]->Draw(worldTransformBody_, *camera_);
	models_[kModelIndexHead]->Draw(worldTransformHead_, *camera_);
	models_[kModelIndexArm_L]->Draw(worldTransformArm_L_, *camera_);
	models_[kModelIndexArm_R]->Draw(worldTransformArm_R_, *camera_);
	if (behavior_ == Behavior::kAttack) {
		models_[kModelIndexWeapon]->Draw(worldTransformWeapon_, *camera_);
	}
}

void Player::RegisterGlobalVariables() {
	GlobalVariables* gv = GlobalVariables::GetInstance();

	gv->AddItem(ParamKey::kGroupKey, ParamKey::kSpeed, Param::speed_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kFloatingAmplitude, Param::floatingAmplitude_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kFloatingArmRotationAmplitude, Param::floatingArmRotationAmplitude_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kFloatingCycle, Param::floatingCycle_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kAttackStartShoulderRotationX, Param::attackStartShoulderRotationX_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kAttackWindUpShoulderRotationX, Param::attackWindUpShoulderRotationX_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kAttackArmRotationX, Param::attackArmRotationX_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kAttackWindUpFrame, Param::attackWindUpFrame_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kAttackSwingDownFrame, Param::attackSwingDownFrame_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kAttackEndlagFrame, Param::attackEndlagFrame_);
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
	Param::attackStartShoulderRotationX_ = gv->GetValue<float>(ParamKey::kGroupKey, ParamKey::kAttackStartShoulderRotationX);
	Param::attackWindUpShoulderRotationX_ = gv->GetValue<float>(ParamKey::kGroupKey, ParamKey::kAttackWindUpShoulderRotationX);
	Param::attackArmRotationX_ = gv->GetValue<float>(ParamKey::kGroupKey, ParamKey::kAttackArmRotationX);
	Param::attackWindUpFrame_ = gv->GetValue<int32_t>(ParamKey::kGroupKey, ParamKey::kAttackWindUpFrame);
	Param::attackSwingDownFrame_ = gv->GetValue<int32_t>(ParamKey::kGroupKey, ParamKey::kAttackSwingDownFrame);
	Param::attackEndlagFrame_ = gv->GetValue<int32_t>(ParamKey::kGroupKey, ParamKey::kAttackEndlagFrame);
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
	//floatingParameter_ = 0.0f;
}

void Player::InitializeAttackFrame() {
	attackingParameter_ = 0.0f;
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
}

void Player::UpdateFloatingTranslationY() {
	// 浮遊の振幅<m>
	const float amplitude = Param::floatingAmplitude_;

	// 浮遊を座標に反映
	const float floatingTranslationY = std::sin(floatingParameter_) * amplitude;
	worldTransformBody_.translation_.y += floatingTranslationY;
}

void Player::UpdateFloatingShoulderRotation() {
	const float armRotationAmplitude = Param::floatingArmRotationAmplitude_;
	const float floatingArmRotationX = std::sin(floatingParameter_) * armRotationAmplitude;
	worldTransformShoulder_.rotation_.x = floatingArmRotationX;
}

void Player::BehaviorRootUpdate() {
	// 攻撃に移行
	if (Input::GetControllerButton(XINPUT_GAMEPAD_B)) {
		behaviorRequest_ = Behavior::kAttack;
	}

	// 移動処理
	Move();
	
	// 浮遊ギミック更新
	UpdateFloatingGimmick();
	UpdateFloatingTranslationY();
	UpdateFloatingShoulderRotation();
	
}

void Player::BehaviorAttackUpdate() {
	Attack();
}

void Player::UpdateWorldTransforms() {
	BaseCharacter::Update();
	worldTransformBody_.UpdateMatrix(*camera_);
	worldTransformHead_.UpdateMatrix(*camera_);
	worldTransformShoulder_.UpdateMatrix(*camera_);
	worldTransformArm_L_.UpdateMatrix(*camera_);
	worldTransformArm_R_.UpdateMatrix(*camera_);
	worldTransformWeapon_.UpdateMatrix(*camera_);
}

void Player::BehaviorRootInitialize() {
	InitializeFloatingGimmick();
	worldTransformArm_L_.rotation_ = Param::offsetRotateArm_L_;
	worldTransformArm_R_.rotation_ = Param::offsetRotateArm_R_;
}

void Player::BehaviorAttackInitialize() {
	InitializeAttackFrame();
	attackPhase_ = AttackPhase::kWindUp;

	worldTransformArm_R_.rotation_.x = Param::attackArmRotationX_;
	worldTransformArm_L_.rotation_.x = Param::attackArmRotationX_;
	worldTransformShoulder_.rotation_.x = Param::attackStartShoulderRotationX_;
	attackBaseShoulderRotationX_ = worldTransformShoulder_.rotation_.x;
}

void Player::Attack() {
	switch (attackPhase_) {
	case AttackPhase::kWindUp:
		AttackWindUp();
		break;
	case AttackPhase::kSwingDown:
		AttackSwingDown();
		break;
	case AttackPhase::kEndlag:
		AttackEndlag();
		break;
	default:
		break;
	}
}

void Player::AttackWindUp() {
	attackingParameter_ += 1.0f;

	const float progress = CalculateFrameProgress(attackingParameter_, Param::attackWindUpFrame_);
	worldTransformShoulder_.rotation_.x = Lerp(attackBaseShoulderRotationX_, Param::attackWindUpShoulderRotationX_, progress);

	if (progress >= 1.0f) {
		attackPhase_ = AttackPhase::kSwingDown;
		InitializeAttackFrame();
	}
}

void Player::AttackSwingDown() {
	attackingParameter_ += 1.0f;

	const float progress = CalculateFrameProgress(attackingParameter_, Param::attackSwingDownFrame_);
	worldTransformShoulder_.rotation_.x = Lerp(Param::attackWindUpShoulderRotationX_, attackBaseShoulderRotationX_, progress);

	if (progress >= 1.0f) {
		attackPhase_ = AttackPhase::kEndlag;
		InitializeAttackFrame();
	}
}

void Player::AttackEndlag() {
	attackingParameter_ += 1.0f;

	if (Param::attackEndlagFrame_ <= 0) {
		behaviorRequest_ = Behavior::kRoot;
		return;
	}

	if (attackingParameter_ >= static_cast<float>(Param::attackEndlagFrame_)) {
		behaviorRequest_ = Behavior::kRoot;
	}
}

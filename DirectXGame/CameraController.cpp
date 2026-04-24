#include "CameraController.h"

#include "externals/imgui/imgui.h"
#include "Math.h"
#include "Player.h"
#include <algorithm>

using namespace KujakuEngine;

void CameraController::Init(Camera* camera) {
	// カメラの設定
	camera_ = camera;

	// カメラの初期化
	camera_->Initialize();
}

void CameraController::Update() {
	switch (mode_) {
	case CameraController::Mode::kFollow:
		Follow();
		break;
	case CameraController::Mode::kForcedScroll:
		ForcedScroll();
		break;
	default:
		break;
	}

#ifdef USE_IMGUI
	ImGui::Begin("CameraController");
	ImGui::DragFloat("movableArea.right", &movableArea_.right, 0.1f);
	ImGui::End();
#endif // USE_IMGUI


	// 範囲内クランプ
	ClampMovableArea();

	// 行列を更新する
	camera_->UpdateMatrix();
}

void CameraController::Reset() {
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの座標を計算
	camera_->translation_ = targetWorldTransform.translation_ + targetOffset_;
}

void CameraController::Follow() {
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	goalPosition_ = targetWorldTransform.translation_ + targetOffset_ + target_->GetVelocity() * kVelocityBias;

	// 座標補間によりゆったり追従
	camera_->translation_ = Vector3::Lerp(camera_->translation_, goalPosition_, kInterpolationRate);

	// 追従対象が画面外に出ないように補正
	camera_->translation_.x = std::clamp(camera_->translation_.x, targetWorldTransform.translation_.x + kMarginArea.left, targetWorldTransform.translation_.x + kMarginArea.right);
	camera_->translation_.y = std::clamp(camera_->translation_.y, targetWorldTransform.translation_.y + kMarginArea.bottom, targetWorldTransform.translation_.y + kMarginArea.top);

}

void CameraController::ForcedScroll() { camera_->translation_.x += kScrollSpeed; }

void CameraController::ClampMovableArea() {
	// 移動範囲制限
	camera_->translation_.x = std::clamp(camera_->translation_.x, movableArea_.left, movableArea_.right);
	camera_->translation_.y = std::clamp(camera_->translation_.y, movableArea_.bottom, movableArea_.top);
}

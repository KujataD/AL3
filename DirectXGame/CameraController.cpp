#include "CameraController.h"

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
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	goalPosition_ = targetWorldTransform.translation_ + targetOffset_ + target_->GetVelocity() * kVelocityBias;

	// 座標補間によりゆったり追従
	camera_->translation_ = Vector3::Lerp(camera_->translation_, goalPosition_, kInterpolationRate);

	// 追従対象が画面外に出ないように補正
	camera_->translation_.x = std::clamp(camera_->translation_.x, targetWorldTransform.translation_.x + kMarginArea.left, targetWorldTransform.translation_.x + kMarginArea.right);
	camera_->translation_.y = std::clamp(camera_->translation_.y,  targetWorldTransform.translation_.y +kMarginArea.bottom, targetWorldTransform.translation_.y + kMarginArea.top);

	// 移動範囲制限
	camera_->translation_.x = std::clamp(camera_->translation_.x, movableArea_.left, movableArea_.right);
	camera_->translation_.y = std::clamp(camera_->translation_.y, movableArea_.bottom, movableArea_.top);

	// 行列を更新する
	camera_->UpdateMatrix();
}

void CameraController::Reset() {
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの座標を計算
	camera_->translation_ = targetWorldTransform.translation_ + targetOffset_;
}

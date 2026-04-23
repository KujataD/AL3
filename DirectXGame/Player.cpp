#include "Player.h"
#include "MapChipField.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <numbers>

using namespace KujakuEngine;

void Player::Init(KujakuEngine::Model* model, KujakuEngine::Camera* camera, const Vector3& position) { // nullptr check
	assert(model);

	// 引数の記録
	model_ = model;
	camera_ = camera;

	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = {1.0f, 1.0f, 0.0f};
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::Update() {
	// 移動入力
	Move();

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.moveAmount = velocity_;

	CheckMapCollision(collisionMapInfo);

	ApplyCollision(collisionMapInfo);

	OnCeiling(collisionMapInfo);

	// 着地フラグ
	bool landing = false;

	// 地面との当たり判定
	//  下降中?
	if (velocity_.y < 0) {
		// Y座標が地面以下になったら着地
		if (worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}
	}

	// 接地判定
	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態に移行
			onGround_ = false;
		}
	} else {
		// 着地
		if (landing) {
			// めり込み排斥
			worldTransform_.translation_.y = 1.0f;
			// 摩擦で横方向速度が減衰する
			velocity_.x *= (1.0f - kAttenuation);
			// 下方向速度をリセット
			velocity_.y = 0.0f;
			// 接地状態に移行
			onGround_ = true;
		}
	}

	TurnControl();

	worldTransform_.UpdateMatrix(*camera_);
}

void Player::Draw() { model_->Draw(worldTransform_, *camera_); }

void Player::Move() {
	// 移動
	worldTransform_.translation_ += velocity_;

	if (onGround_) {

		// 移動入力
		// 左右移動操作
		if (Input::GetKey(DIK_RIGHT) || Input::GetKey(DIK_LEFT)) {

			// 左右加速
			Vector3 acceleration = {};
			if (Input::GetKey(DIK_RIGHT)) {
				// 左移動中の右入力
				if (velocity_.x < 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}

				acceleration.x += kAcceleration;

				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;

					// 旋回開始時の角度を記録する
					turnFirstRotationY_ = worldTransform_.rotation_.y;

					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
				}
			} else if (Input::GetKey(DIK_LEFT)) {
				// 右移動中の左入力
				if (velocity_.x > 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}

				acceleration.x -= kAcceleration;
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;

					// 旋回開始時の角度を記録する
					turnFirstRotationY_ = worldTransform_.rotation_.y;

					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
				}
			}

			// 加速/減速
			velocity_ += acceleration;

			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			// 非入力時は移動減衰をかける
			velocity_.x *= (1.0f - kAttenuation);
		}

		if (Input::GetKey(DIK_UP)) {
			// ジャンプ初速
			velocity_ += Vector3(0, kJumpAcceleration, 0);
		}

	} else { // 空中

		// 落下速度
		velocity_ += Vector3(0, -kGravityAcceleration, 0);

		// 落下速度制限
		velocity_.y = (std::max)(velocity_.y, -kLimitFallSpeed);
	}
}

void Player::CheckMapCollision(CollisionMapInfo& info) {
	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionLeft(info);
	CheckMapCollisionRight(info);
}

void Player::CheckMapCollisionUp(CollisionMapInfo& info) {
	// 上昇していなかったらリターン
	if (info.moveAmount.y <= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.moveAmount, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真上の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		// めり込み先ブロックの範囲矩形
		Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.moveAmount.y = (std::max)(0.0f, rect.bottom - worldTransform_.translation_.y - kHeight / 2.0f - kBlank);
		// 天井に当たったことを記録する
		info.isCeiling = true;
	}
}

void Player::CheckMapCollisionDown(CollisionMapInfo& info) { info; }

void Player::CheckMapCollisionRight(CollisionMapInfo& info) { info; }

void Player::CheckMapCollisionLeft(CollisionMapInfo& info) { info; }

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, // kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, // kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, // kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}  // kLeftTop
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}

void Player::ApplyCollision(const CollisionMapInfo& info) { worldTransform_.translation_ += info.moveAmount; }

void Player::OnCeiling(const CollisionMapInfo& info) {
	// 天井に当たった?
	if (info.isCeiling) {
		velocity_.y = 0;
	}
}

void Player::OnWall() {}

void Player::UpdateGroundState() {}

void Player::TurnControl() {
	// 旋回制御
	if (turnTimer_ > 0.0f) {
		// 旋回タイマーを1 / 60秒だけカウントダウンする
		turnTimer_ -= 1.0f / 60.0f;

		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};

		// 状態に応じた目標角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		// 自キャラの角度を設定する
		worldTransform_.rotation_.y = std::lerp(turnFirstRotationY_, destinationRotationY, 1.0f - turnTimer_ / kTimeTurn);
	}
}

void Player::UpdateMatrix() {}

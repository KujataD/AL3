#define NOMINMAX
#include "Player.h"
#include "MapChipField.h"
#include "Math.h"
#include "PlayerBehaviorAttack.h"
#include "PlayerBehaviorKnockback.h"
#include "PlayerBehaviorRoot.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <numbers>

using namespace KujakuEngine;
using namespace EaseUtil;

Player::Player() {}

Player::~Player() {}

void Player::Init(Model* model, Model* modelAttack, Camera* camera, const Vector3& position) {
	// nullptr check
	assert(model);
	assert(modelAttack);
	assert(camera);

	// 引数の記録
	model_ = model;
	modelAttack_ = modelAttack;
	camera_ = camera;

	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	worldTransformAttack_.Initialize();
	worldTransformAttack_.translation_ = worldTransform_.translation_;
	worldTransformAttack_.rotation_ = worldTransform_.rotation_;

	isDead_ = false;

	// ルートビヘイビアに変更
	ChangeBehavior(new PlayerBehaviorRoot);
}

void Player::Update() {
	// ふるまい変更のrequest処理
	if (behaviorRequest_) {
		ChangeBehavior(behaviorRequest_);
		behaviorRequest_ = nullptr;
	}

	if (isDead_) {
		return;
	}

	// アタックのクールタイム更新
	if (attackCoolTimer_ > 0.0f) {
		attackCoolTimer_ -= 1.0f / 60.0f;
	}

	// ビヘイビアの更新
	if (behavior_) {
		behavior_->Update(this);
	}

	// カメラが見えている範囲
	Rect cameraVisibleRect = camera_->GetVisibleRect(worldTransform_.translation_.z);
	
	// ウィンドウ外に接しているかどうか
	bool isHitWindow = false;
	if (worldTransform_.translation_.x <= cameraVisibleRect.left + kWidth / 2.0f || worldTransform_.translation_.x >= cameraVisibleRect.right - kWidth / 2.0f) {
		isHitWindow = true;
	}

	// ウィンドウ内にクランプする
	worldTransform_.translation_.x = std::clamp(worldTransform_.translation_.x, cameraVisibleRect.left + kWidth / 2.0f, cameraVisibleRect.right - kWidth / 2.0f);
	worldTransform_.translation_.y = std::clamp(worldTransform_.translation_.y, cameraVisibleRect.bottom + kHeight / 2.0f, cameraVisibleRect.top - kHeight / 2.0f);

	// 壁に挟まれて死ぬ
	if (isHitWindow && IsTouchingWall()) {
		isDead_ = true;
	}
}

void Player::Draw() {
	if (isDead_) {
		return;
	}
	model_->Draw(worldTransform_, *camera_);

	if (isActiveAttackFX_) {
		// 表面の攻撃エフェクト
		worldTransformAttack_.translation_ = worldTransform_.translation_;
		worldTransformAttack_.rotation_ = worldTransform_.rotation_;
		worldTransformAttack_.rotation_.y += std::numbers::pi_v<float>;
		worldTransformAttack_.UpdateMatrix(*camera_);
		modelAttack_->Draw(worldTransformAttack_, *camera_);

		// 裏面の攻撃エフェクト
		worldTransformAttack_.translation_ = worldTransform_.translation_;
		worldTransformAttack_.rotation_ = worldTransform_.rotation_;
		worldTransformAttack_.rotation_.y += std::numbers::pi_v<float> * 2.0f;
		worldTransformAttack_.rotation_.x += std::numbers::pi_v<float> * 2.0f;
		worldTransformAttack_.UpdateMatrix(*camera_);
		modelAttack_->Draw(worldTransformAttack_, *camera_);
	}
}

void Player::OnCollision(const BaseEnemy* enemy) {
	(void)enemy;

	// 攻撃中はダメージ無効
	if (IsAttack()) {
		return;
	}

	isDead_ = true;
}

KujakuEngine::Vector3 Player::GetWorldPosition() const {
	Vector3 worldPos;
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;
	return worldPos;
}

AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Player::RequestBehavior(IPlayerBehavior* newBehavior, BehaviorChangeToken token) {
	behaviorRequest_ = newBehavior;
	(void)token;
}

void Player::RequestKnockbackBehavior() { RequestBehavior(new PlayerBehaviorKnockback, BehaviorChangeToken{}); }

void Player::Move() {
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
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}

void Player::Knockback() {}

void Player::UpdateCollision(const Vector3& move) {
	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.moveAmount = move;

	// マップ衝突判定
	CheckMapCollision(collisionMapInfo);

	// 判定結果を反映
	ApplyCollision(collisionMapInfo);

	// 天井に当たったとき
	OnCeiling(collisionMapInfo);

	// 壁に当たったとき
	OnWall(collisionMapInfo);

	// 接地状態の切り替え
	UpdateGroundState(collisionMapInfo);

	// 攻撃回数の回復
	if (onGround_ && attackCounter_ < kAttackMaxCount) {
		attackCounter_ = kAttackMaxCount;
	}
}

float Player::GetBlinkVelocityX() {
	switch (lrDirection_) {
	case LRDirection::kRight:
		return kAttackBlinkSpeed;
		break;
	case LRDirection::kLeft:
		return -kAttackBlinkSpeed;
		break;
	}
	return 0.0f;
}

float Player::GetKnockbackVelocityX() {
	switch (lrDirection_) {
	case LRDirection::kRight:
		return -kKnockbackSpeed;
		break;
	case LRDirection::kLeft:
		return kKnockbackSpeed;
		break;
	}
	return 0.0f;
}

void Player::SetSizeEaseOut(const KujakuEngine::Vector3& startSize, const KujakuEngine::Vector3& endSize, float t) {
	worldTransform_.scale_.x = EaseLerp(startSize.x, endSize.x, t, EaseType::OutQuad);
	worldTransform_.scale_.y = EaseLerp(startSize.y, endSize.y, t, EaseType::OutQuad);
	worldTransform_.scale_.z = EaseLerp(startSize.z, endSize.z, t, EaseType::OutQuad);
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
	MapChipType mapChipTypeNext;

	// 真上の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		// めり込み先ブロックの範囲矩形
		Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.moveAmount.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - kHeight / 2.0f - kBlank);
		// 天井に当たったことを記録する
		info.isCeiling = true;
	}
}

void Player::CheckMapCollisionDown(CollisionMapInfo& info) {
	if (info.moveAmount.y >= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.moveAmount, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	// 真下の当たり判定を行う
	bool hit = false;

	// 左下点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);

		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_);
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// めり込み先ブロックの範囲矩形
			Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.moveAmount.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + kHeight / 2.0f + kBlank);
			// 地面に当たったことを記録する
			info.isGround = true;
		}
	}
}

void Player::CheckMapCollisionRight(CollisionMapInfo& info) {
	// 右移動あり？
	if (info.moveAmount.x <= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.moveAmount, static_cast<Corner>(i));
	}

	MapChipType mapChipType;

	// 右の当たり判定を行う
	bool hit = false;
	// 右上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		// めり込み先ブロックの範囲矩形
		Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.moveAmount.x = std::min(0.0f, rect.left - worldTransform_.translation_.x - kWidth / 2.0f - kBlank);
		// 壁に当たったことを記録する
		info.isWall = true;
	}
}

void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {
	// 左移動あり？
	if (info.moveAmount.x >= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.moveAmount, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	// 左の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		// めり込み先ブロックの範囲矩形
		Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.moveAmount.x = std::max(0.0f, rect.right - worldTransform_.translation_.x + kWidth / 2.0f + kBlank);
		// 壁に当たったことを記録する
		info.isWall = true;
	}
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) const {
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

void Player::OnWall(const CollisionMapInfo& info) {
	if (info.isWall) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
}

void Player::OnGround(const CollisionMapInfo& info) { info; }

void Player::UpdateGroundState(const CollisionMapInfo& info) {
	if (onGround_) {
		// 接地状態の処理

		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態に移行
			onGround_ = false;
		} else {
			std::array<Vector3, kNumCorner> positionsNew;

			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPosition(worldTransform_.translation_ + Vector3(0, -kGroundCheckEpsilon, 0), static_cast<Corner>(i));
			}

			MapChipType mapChipType;

			// 真下の当たり判定を行う
			bool hit = false;

			// 左上点の判定
			MapChipField::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			// 右上点の判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}
			// 落下開始
			if (!hit) {
				// 空中状態に切り替える
				onGround_ = false;
			}
		}
	} else {
		// 空中状態の処理

		// 接地したら
		if (info.isGround) {
			// 接地状態に移行
			onGround_ = true;
			// 着地時にX速度を減衰
			velocity_.x *= (1.0f - kAttenuationLanding);
			// 下方向速度をリセット
			velocity_.y = 0.0f;
		}
	}
}

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
		worldTransform_.rotation_.y = std::lerp(turnFirstRotationY_, destinationRotationY, std::clamp(1.0f - turnTimer_ / kTimeTurn, 0.0f, 1.0f));
	}
}

void Player::BehaviorRootUpdate() {
	// 移動入力
	Move();

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.moveAmount = velocity_;

	// マップ衝突判定
	CheckMapCollision(collisionMapInfo);

	// 判定結果を反映
	ApplyCollision(collisionMapInfo);

	// 天井に当たったとき
	OnCeiling(collisionMapInfo);

	// 壁に当たったとき
	OnWall(collisionMapInfo);

	// 接地状態の切り替え
	UpdateGroundState(collisionMapInfo);

	// 旋回制御
	TurnControl();

	UpdateTransform();
}

bool Player::IsTouchingWall() const {
	constexpr float kWallCheckEpsilon = 0.01f;

	Vector3 rightCheckPos = worldTransform_.translation_ + Vector3(kWallCheckEpsilon, 0.0f, 0.0f);
	Vector3 leftCheckPos = worldTransform_.translation_ + Vector3(-kWallCheckEpsilon, 0.0f, 0.0f);

	// コーナーをチェックするラムダ
	auto IsBlockCorner = [this](const Vector3& center, Corner corner) -> bool {
		Vector3 pos = CornerPosition(center, corner);

		MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(pos);

		return mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock;
	};

	// 左右判定
	bool result = IsBlockCorner(rightCheckPos, kRightTop) || IsBlockCorner(rightCheckPos, kRightBottom) || IsBlockCorner(leftCheckPos, kLeftTop) || IsBlockCorner(leftCheckPos, kLeftBottom);

	return result;
}

void Player::ChangeBehavior(IPlayerBehavior* newBehavior) {

	// 状態から抜けたときの処理
	if (behavior_) {
		behavior_->Exit(this);
	}

	// 状態変更
	behavior_.reset(newBehavior);

	// 状態に入ったときの処理
	if (behavior_) {
		behavior_->Enter(this);
	}
}

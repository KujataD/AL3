#include "GuardEffect.h"
#include <algorithm>

// 生定期メンバの実体
KujakuEngine::Model* GuardEffect::model_ = nullptr;
KujakuEngine::Camera* GuardEffect::camera_ = nullptr;

using namespace KujakuEngine;
using namespace EaseUtil;

void GuardEffect::Init(const KujakuEngine::Vector3 spawnPos) {
	circleWorldTransform_.translation_ = spawnPos;
	circleWorldTransform_.Initialize();

	state_ = State::kAnim;
}

void GuardEffect::Update() {
	switch (state_) {
	case GuardEffect::State::kAnim: {
		// ライフタイム更新
		animCounter_ += 1.0f / 60.0f;

		// アニメーション用 T
		float animT = std::clamp(animCounter_ / kAnimDuration, 0.0f, 1.0f);

		// スケールのイージング
		circleWorldTransform_.scale_.x = EaseLerp(kSizeStart, kSizeEnd, animT, EaseType::OutQuad);
		circleWorldTransform_.scale_.y = EaseLerp(kSizeStart, kSizeEnd, animT, EaseType::OutQuad);

		if (animCounter_ > kAnimDuration) {
			state_ = State::kDead;
		}
	} break;
	case GuardEffect::State::kDead:
		break;
	default:
		break;
	}

	circleWorldTransform_.UpdateMatrix(*camera_);
}

void GuardEffect::Draw() {
	if (IsDead()) {
		return;
	}

	model_->Draw(circleWorldTransform_, *camera_);
}

GuardEffect* GuardEffect::Create(const KujakuEngine::Vector3 spawnPos) {
	// インスタンス作成
	GuardEffect* instance = new GuardEffect();
	// newの失敗を検出
	assert(instance);
	// インスタンスの初期化
	instance->Init(spawnPos);
	// 初期化したインスタンスを返す
	return instance;
}

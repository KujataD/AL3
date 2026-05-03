#include "HitEffect.h"
#include "GlobalVariables.h"
#include <algorithm>

// 生定期メンバの実体
KujakuEngine::Model* HitEffect::model_ = nullptr;
KujakuEngine::Camera* HitEffect::camera_ = nullptr;

using namespace KujakuEngine;
using namespace EaseUtil;

void HitEffect::Init(const KujakuEngine::Vector3 spawnPos, const KujakuEngine::Vector4 color) {
	// トランスフォーム初期化
	circleWorldTransform_.Initialize();
	circleWorldTransform_.translation_ = spawnPos;

	// 楕円エフェクト
	for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
		worldTransform.scale_ = {kEllipseWidth * kSizeStart, kEllipseHeight * kSizeStart, 1.0f};
		worldTransform.rotation_ = {0.0f, 0.0f, Random::GetRandom<float>(-1.0f, 1.0f) * std::numbers::pi_v<float>};
		worldTransform.translation_ = spawnPos;

		worldTransform.Initialize();
	}

	state_ = State::kAnim;
	color_ = color;
}

void HitEffect::Update() {
	switch (state_) {
	case HitEffect::State::kAnim: {
		// ライフタイム更新
		animCounter_ += 1.0f / 60.0f;

		// アニメーション用 T
		float animT = std::clamp(animCounter_ / kAnimDuration, 0.0f, 1.0f);

		// α値のイージング
		color_.w = EaseLerp(1.0f, 0.0f, animT, EaseType::Linear);
		model_->SetColor(color_);

		// スケールのイージング
		circleWorldTransform_.scale_.x = EaseLerp(kSizeStart, kSizeEnd, animT, EaseType::OutQuad);
		circleWorldTransform_.scale_.y = EaseLerp(kSizeStart, kSizeEnd, animT, EaseType::OutQuad);
		for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
			worldTransform.scale_.x = EaseLerp(kEllipseWidth * kSizeStart, kEllipseWidth * kSizeEnd, animT, EaseType::OutQuad);
			worldTransform.scale_.y = EaseLerp(kEllipseHeight * kSizeStart, kEllipseHeight * kSizeEnd, animT, EaseType::OutQuad);
		}

		if (animCounter_ > kAnimDuration) {
			state_ = State::kDead;
		}
	} break;
	case HitEffect::State::kDead:
		break;
	default:
		break;
	}

	circleWorldTransform_.UpdateMatrix(*camera_);
	for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
		worldTransform.UpdateMatrix(*camera_);
	}
}

void HitEffect::Draw() {
	if (IsDead()) {
		return;
	}

	model_->Draw(circleWorldTransform_, *camera_);
	for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
		model_->Draw(worldTransform, *camera_);
	}
}

HitEffect* HitEffect::Create(const KujakuEngine::Vector3 spawnPos, const KujakuEngine::Vector4 color) {
	// インスタンス作成
	HitEffect* instance = new HitEffect();
	// newの失敗を検出
	assert(instance);
	// インスタンスの初期化
	instance->Init(spawnPos, color);
	// 初期化したインスタンスを返す
	return instance;
}

void HitEffect::RegisterGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	globalVariables->AddItem(ParamKeys::kGroup, ParamKeys::kEllipseWidthKey, kEllipseWidth);
	globalVariables->AddItem(ParamKeys::kGroup, ParamKeys::kEllipseHeightKey, kEllipseHeight);

	globalVariables->AddItem(ParamKeys::kGroup, ParamKeys::kSizeStartKey, kSizeStart);
	globalVariables->AddItem(ParamKeys::kGroup, ParamKeys::kSizeEndKey, kSizeEnd);

	globalVariables->AddItem(ParamKeys::kGroup, ParamKeys::kAnimDurationKey, kAnimDuration);
}

void HitEffect::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	kEllipseWidth = globalVariables->GetValue<float>(ParamKeys::kGroup, ParamKeys::kEllipseWidthKey);
	kEllipseHeight = globalVariables->GetValue<float>(ParamKeys::kGroup, ParamKeys::kEllipseHeightKey);

	kSizeStart = globalVariables->GetValue<float>(ParamKeys::kGroup, ParamKeys::kSizeStartKey);
	kSizeEnd = globalVariables->GetValue<float>(ParamKeys::kGroup, ParamKeys::kSizeEndKey);

	kAnimDuration = globalVariables->GetValue<float>(ParamKeys::kGroup, ParamKeys::kAnimDurationKey);
}
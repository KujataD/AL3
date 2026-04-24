#include "TitleScene.h"
#include "DeltaTime.h"
#include "Transform.h"
#include <numbers>

using namespace KamataEngine;

TitleScene::~TitleScene() {
	delete playerModel_;
	delete titleFontModel_;
	delete fade_;
}

void TitleScene::Init() {

	// プレイヤーモデル初期化
	playerModel_ = Model::CreateFromOBJ("player", true);
	playerWorldTransform_.Initialize();
	playerWorldTransform_.translation_ = Vector3(0.0f, -4.5f, 0.0f);
	playerWorldTransform_.rotation_.y = std::numbers::pi_v<float>;
	playerWorldTransform_.scale_ = Vector3(4.0f, 4.0f, 4.0f);

	// タイトル文字初期化
	titleFontModel_ = Model::CreateFromOBJ("titleFont", true);
	titleFontWorldTransform_.Initialize();
	titleFontWorldTransform_.translation_ = Vector3(0.0f, 2.0f, 0.0f);
	titleFontAnimStartPosY_ = titleFontWorldTransform_.translation_.y;

	// カメラ初期化
	camera_.Initialize();
	camera_.translation_ = Vector3(0.0f, 0.0f, -20.0f);

	// フェード処理の初期化
	fade_ = new Fade();
	fade_->Init();
	fade_->Start(Fade::Status::FadeIn, kFadeDuration);
}

void TitleScene::Update() {
	{ // タイトル文字の上下アニメーション

		// デルタタイム加算
		titleFontAnimCounter_ += DeltaTime::Get();

		// sin波で上下動
		float animPosY = std::cos(2.0f * std::numbers::pi_v<float> * titleFontAnimCounter_ / kTitleFontAnimDuration);

		// 加算
		titleFontWorldTransform_.translation_.y = titleFontAnimStartPosY_ + animPosY;
	}

	fade_->Update();

	switch (phase_) {
	case Phase::kFadeIn:
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
			fade_->Stop();
		}
		break;
	case Phase::kMain:
		// シーン遷移
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, kFadeDuration);
		}
		break;
	case Phase::kFadeOut:
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	default:
		break;
	}

	// カメラ更新
	camera_.UpdateMatrix();

	// 各トランスフォームの更新
	// ------------------------------------------

	// プレイヤー
	WorldTransformUpdate(playerWorldTransform_);

	// タイトル文字
	WorldTransformUpdate(titleFontWorldTransform_);
}

void TitleScene::Draw() {

	// ------------------------------------------
	// 3Dモデル描画前処理
	Model::PreDraw();

	playerModel_->Draw(playerWorldTransform_, camera_);
	titleFontModel_->Draw(titleFontWorldTransform_, camera_);

	// 3Dモデル描画後処理
	Model::PostDraw();
	// ------------------------------------------
	fade_->Draw();
}

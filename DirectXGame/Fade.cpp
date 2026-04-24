#include "Fade.h"
#include "DeltaTime.h"
#include <algorithm>

using namespace KamataEngine;

void Fade::Init() {
	textureHandleWhite1x1 = TextureManager::Load("white1x1.png");
	sprite_ = Sprite::Create(textureHandleWhite1x1, {0.0f, 0.0f});
	sprite_->SetSize(Vector2(1280, 720));
	sprite_->SetColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
}

void Fade::Update() {
	switch (status_) {
	case Status::None:
		// 何もしない
		break;
	case Status::FadeIn:
		// 1フレーム分の秒数をカウントアップ
		counter_ += 1.0f / 60.0f;

		// フェード継続時間に達したら打ち止め
		if (counter_ >= duration_) {
			counter_ = duration_;
		}

		// 0.0fから1.0fの間で、経過時間がフェード継続時間に近づくほどアルファ値を大きくする
		sprite_->SetColor(Vector4(0.0f, 0.0f, 0.0f, std::clamp(1.0f - counter_ / duration_, 0.0f, 1.0f)));


		break;
	case Status::FadeOut:
		// 1フレーム分の秒数をカウントアップ
		counter_ +=  1.0f / 60.0f;

		// フェード継続時間に達したら打ち止め
		if (counter_ >= duration_) {
			counter_ = duration_;
		}

		// 0.0fから1.0fの間で、経過時間がフェード継続時間に近づくほどアルファ値を大きくする
		sprite_->SetColor(Vector4(0.0f, 0.0f, 0.0f, std::clamp(counter_ / duration_, 0.0f, 1.0f)));
		break;
	default:
		break;
	}
}

void Fade::Draw() {
	if (status_ == Status::None) {
		return;
	}

	Sprite::PreDraw();
	sprite_->Draw();
	Sprite::PostDraw();
}

void Fade::Start(Status status, float duration) {
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
}

void Fade::Stop() { status_ = Status::None; }

bool Fade::IsFinished() const { 
	switch (status_) {
	case Fade::Status::None:
		break;
	case Fade::Status::FadeIn:
		if (counter_ >= duration_) {
			return true;
		} else {
			return false;
		}
		break;
	case Fade::Status::FadeOut:
		if (counter_ >= duration_) {
			return true;
		} else {
			return false;
		}
		break;
	default:
		break;
	}

	return false; 
}

#include "DeathParticles.h"
#include <algorithm>

using namespace KujakuEngine;

void DeathParticles::Init(KujakuEngine::Model* model, KujakuEngine::Camera* camera, const KujakuEngine::Vector3& position) {
	// nullptr check
	assert(model);

	// 引数の記録
	model_ = model;
	camera_ = camera;

	for (WorldTransform& worldTransform : worldTransforms_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
		worldTransform.UpdateMatrix(*camera);
	}

	color_ = {1.0f, 1.0f, 1.0f, 1.0f};
}

void DeathParticles::Update() {

	// 終了なら何もしない
	if (isFinished_) {
		return;
	}

	// カウンターを1フレーム分の秒数進める
	counter_ += 1.0f / 60.0f;

	// 存続時間の上限に達したら
	if (counter_ >= kDuration) {
		counter_ = kDuration;
		// 終了扱いにする
		isFinished_ = true;
	}

	color_.w = std::clamp(1.0f - counter_ / kDuration, 0.0f, 1.0f);
	// 色変更オブジェクトに色の数値を設定する
	model_->SetColor(color_);

	for (uint32_t i = 0; i < kNumParticles; i++) {
		// 基本となる速度ベクトル
		Vector3 velocity = {kSpeed *  1.0f / 60.0f, 0, 0};
		// 回転角を計算する
		float angle = kAngleUnit * i;
		// Z軸まわり回転行列
		Matrix4x4 matrixRotation = Matrix4x4::MakeRotateZMatrix(angle);
		// 基本ベクトルを回転させて速度ベクトルを得る
		velocity = Vector3::Transform(velocity, matrixRotation);
		// 移動処理
		worldTransforms_[i].translation_ += velocity;
	}

	// ワールド座標更新
	for (WorldTransform& worldTransform : worldTransforms_) {
		worldTransform.UpdateMatrix(*camera_);
	}
}

void DeathParticles::Draw() {
	// 終了なら何もしない
	if (isFinished_) {
		return;
	}

	// モデル描画
	for (WorldTransform& worldTransform : worldTransforms_) {
		model_->Draw(worldTransform, *camera_);
	}
}

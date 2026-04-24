#pragma once
#include <KamataEngine.h>
#include <cmath>
#include <numbers>

class Player;

/// <summary>
/// デス演出用パーティクル
/// </summary>
class DeathParticles {
public:
	// Methods
	// ------------------------------------------

	void Init(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void Update();

	void Draw();

	// Accessors
	// ------------------------------------------

	// --- set ---

	// --- get ---

	bool IsFinished() const { return isFinished_; }

private:
	// 定数
	// ------------------------------------------

	// パーティクルの個数
	static inline const uint32_t kNumParticles = 8;

	// 存続時間(消滅までの時間)<秒>
	static inline const float kDuration = 1.5f;

	// 移動の速さ
	static inline const float kSpeed = 3.0f;

	// 分割した1個分の角度
	static inline const float kAngleUnit = 2.0f * std::numbers::pi_v<float> / kNumParticles;

	// 変数
	// ------------------------------------------

	// 3Dモデル
	KamataEngine::Model* model_ = nullptr;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// 各パーティクルのワールドトランスフォーム
	std::array<KamataEngine::WorldTransform, kNumParticles> worldTransforms_;

	// --- パーティクル処理 ---

	// 終了フラグ
	bool isFinished_ = false;

	// 経過時間カウント
	float counter_ = 0.0f;

	// 色変更オブジェクト
	KamataEngine::ObjectColor objectColor_;
	
	// 色の数値
	KamataEngine::Vector4 color_;
};

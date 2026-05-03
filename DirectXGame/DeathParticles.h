#pragma once
#include "KujakuEngine/KujakuEngine.h"
#include <array>
#include <cmath>
#include <numbers>

class Player;

/// <summary>
/// デス演出用パーティクル
/// </summary>
class DeathParticles {
public:
	struct ParamKeys {
		static inline const std::string kGroup = "DeathParticles";
		
		static inline const std::string kDurationKey = "Duration";
		static inline const std::string kSpeedKey = "Speed";
	};

public:
	// Methods
	// ------------------------------------------

	void Init(KujakuEngine::Model* model, KujakuEngine::Camera* camera, const KujakuEngine::Vector3& position);

	void Update();

	void Draw();

	static void RegisterGlobalVariables();

	static void ApplyGlobalVariables();

	// Accessors
	// ------------------------------------------

	// --- set ---

	// --- get ---

	bool IsFinished() const { return isFinished_; }

private:
	// 調整項目
	// ------------------------------------------
	
	// 存続時間(消滅までの時間)<秒>
	static inline float kDuration = 1.5f;

	// 移動の速さ
	static inline float kSpeed = 3.0f;

	// 定数
	// ------------------------------------------

	// パーティクルの個数
	static inline const uint32_t kNumParticles = 8;

	// 分割した1個分の角度
	static inline const float kAngleUnit = 2.0f * std::numbers::pi_v<float> / kNumParticles;

	// 変数
	// ------------------------------------------

	// 3Dモデル
	KujakuEngine::Model* model_ = nullptr;

	// カメラ
	KujakuEngine::Camera* camera_ = nullptr;

	// 各パーティクルのワールドトランスフォーム
	std::array<KujakuEngine::WorldTransform, kNumParticles> worldTransforms_;

	// --- パーティクル処理 ---

	// 終了フラグ
	bool isFinished_ = false;

	// 経過時間カウント
	float counter_ = 0.0f;

	// 色の数値
	KujakuEngine::Vector4 color_;
};

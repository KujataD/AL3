#pragma once
#include "KujakuEngine/KujakuEngine.h"

class Player;

/// <summary>
/// カメラコントローラ
/// </summary>
class CameraController {
public:
	enum class Mode { kFollow, kForcedScroll };

	struct ParamKeys {
		static inline const std::string kGroup = "CameraController";

		static inline const std::string kInterpolationRateKey = "InterpolationRate";
		static inline const std::string kVelocityBiasKey = "VelocityBias";

		// --- MarginArea ---
		static inline const std::string kMarginAreaLeftKey = "MarginAreaLeft";
		static inline const std::string kMarginAreaRightKey = "MarginAreaRight";
		static inline const std::string kMarginAreaBottomKey = "MarginAreaBottom";
		static inline const std::string kMarginAreaTopKey = "MarginAreaTop";

		static inline const std::string kScrollSpeedKey = "ScrollSpeed";
	};

public:
	void Init(KujakuEngine::Camera* camera);

	void Update();

	void Reset();

	// accessor

	// --- setter ---

	void SetTarget(Player* target) { target_ = target; }

	void SetMovableArea(KujakuEngine::Rect& area) { movableArea_ = area; }

	void SetMode(Mode mode) { mode_ = mode; }

	// --- getter ---

	Mode GetMode() const { return mode_; }

	static void RegisterGlobalVariables();

	static void ApplyGlobalVariables();

private:
	void Follow();

	void ForcedScroll();

	void ClampMovableArea();
private:
	// 定数
	// ------------------------------------------

	// 座標補間割合
	static inline float kInterpolationRate = 0.02f;

	// 速度掛け算
	static inline float kVelocityBias = 20.0f;

	// 追従対象の各方向へのカメラ移動範囲
	static inline KujakuEngine::Rect kMarginArea = {-10.0f, 10.0f, -5.0f, 5.0f};

	static inline float kScrollSpeed = 0.05f;

	// 変数
	// ------------------------------------------

	Mode mode_ = Mode::kFollow;

	// カメラ
	KujakuEngine::Camera* camera_ = nullptr;

	// ターゲット
	Player* target_ = nullptr;

	// ターゲットのオフセット
	KujakuEngine::Vector3 targetOffset_ = {0.0f, 0.0f, -40.0f};

	// カメラ移動範囲
	KujakuEngine::Rect movableArea_ = {15.5f, 83.3f, 9.0f, 100.0f};

	// カメラの目標座標
	KujakuEngine::Vector3 goalPosition_;
};

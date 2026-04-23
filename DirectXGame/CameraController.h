#pragma once
#include "KujakuEngine/KujakuEngine.h"

class Player;

/// <summary>
/// カメラコントローラ
/// </summary>
class CameraController {
public:
	void Init(KujakuEngine::Camera* camera);

	void Update();

	void Reset();

	void SetTarget(Player* target) { target_ = target; }

	void SetMovableArea(KujakuEngine::Rect& area) { movableArea_ = area; }

private:
	// 定数
	// ------------------------------------------

	// 座標補間割合
	static inline const float kInterpolationRate = 0.1f;

	// 速度掛け算
	static inline const float kVelocityBias = 20.0f;

	// 追従対象の各方向へのカメラ移動範囲
	static inline const KujakuEngine::Rect kMarginArea = {-10.0f, 10.0f, -5.0f, 5.0f};

	// 変数
	// ------------------------------------------
	
	// カメラ
	KujakuEngine::Camera* camera_ = nullptr;

	// ターゲット
	Player* target_ = nullptr;

	// ターゲットのオフセット
	KujakuEngine::Vector3 targetOffset_ = {0.0f, 0.0f, -40.0f};

	// カメラ移動範囲
	KujakuEngine::Rect movableArea_ = {15.5f, 130.0f, 9.0f, 100.0f};

	// カメラの目標座標
	KujakuEngine::Vector3 goalPosition_;
};

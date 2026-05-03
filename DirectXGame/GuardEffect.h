#pragma once
#include "KujakuEngine/KujakuEngine.h"
#include <vector>

/// <summary>
/// ヒット演出用エフェクト
/// </summary>
class GuardEffect {
public:
	enum class State {
		kAnim,
		kDead,
	};

	struct ParamKeys {
		static inline const std::string kGroup = "GuardEffect";

		static inline const std::string kSizeStartKey = "SizeStart";
		static inline const std::string kSizeEndKey = "SizeEnd";
		static inline const std::string kAnimDurationKey = "AnimDuration";
	};

public:
	void Init(const KujakuEngine::Vector3 spawnPos);

	void Update();

	void Draw();

	static GuardEffect* Create(const KujakuEngine::Vector3 spawnPos);

	static void RegisterGlobalVariables();

	static void ApplyGlobalVariables();

	static void SetModel(KujakuEngine::Model* model) { model_ = model; }
	static void SetCamera(KujakuEngine::Camera* camera) { camera_ = camera; }

	bool IsDead() const { return state_ == State::kDead; }

public:
	// --- 定数 ---

	// 楕円のサイズ
	static inline float kSizeStart = 0.0f;
	static inline float kSizeEnd = 1.5f;

	// ライフタイム
	static inline float kAnimDuration = 0.25f;

private:
	// --- 外部依存 ---

	// モデル
	static KujakuEngine::Model* model_;

	// カメラ
	static KujakuEngine::Camera* camera_;

	// --- インスタンスプロパティ ---

	// 状態
	State state_;

	// 全サイズ
	float allShapeSize_ = 1.0f;

	// ライフカウンター
	float animCounter_ = 0.0f;

	// 円のワールドトランスフォーム
	KujakuEngine::WorldTransform circleWorldTransform_;
};

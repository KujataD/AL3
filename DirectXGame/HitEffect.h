#pragma once
#include  "KujakuEngine/KujakuEngine.h"
#include <vector>
#include <array>

/// <summary>
/// ヒット演出用エフェクト
/// </summary>
class HitEffect {
public:
	enum class State{
		kAnim,
		kDead,
	};
public:
	void Init(const KujakuEngine::Vector3 spawnPos, const KujakuEngine::Vector4 color);

	void Update();

	void Draw();

	static HitEffect* Create(const KujakuEngine::Vector3 spawnPos, const KujakuEngine::Vector4 color);

	static void SetModel(KujakuEngine::Model* model) { model_ = model; }
	static void SetCamera(KujakuEngine::Camera* camera) { camera_ = camera; }

	bool IsDead() const { return state_ == State::kDead; }

public:
	// --- 定数 ---
	
	// 楕円の数
	static inline const int32_t kEllipseValue = 2;

	// 楕円の横幅
	static inline const float kEllipseWidth = 0.1f;
	
	// 楕円の長さ
	static inline const float kEllipseHeight = 2.0f;

	// 楕円のサイズ
	static inline const float kSizeStart = 0.5f;
	static inline const float kSizeEnd = 1.2f;

	// ライフタイム 
	static inline const float kAnimDuration = 0.5f;

private:

	// --- 外部依存 ---

	// モデル
	static KujakuEngine::Model* model_;

	// カメラ
	static KujakuEngine::Camera* camera_;

	// --- インスタンスプロパティ ---

	// 状態
	State state_;

	// Alpha値
	float alphaColor_ = 0.0f;

	KujakuEngine::Vector4 color_ = {1.0f, 1.0f, 1.0f, 1.0f};

	// 全サイズ
	float allShapeSize_ = 1.0f;

	// ライフカウンター
	float animCounter_ = 0.0f;

	// 円のワールドトランスフォーム
	KujakuEngine::WorldTransform circleWorldTransform_;

	// 楕円のワールドトランスフォーム
	std::array<KujakuEngine::WorldTransform, kEllipseValue> ellipseWorldTransforms_;
};

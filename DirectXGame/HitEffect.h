#pragma once
#include <KamataEngine.h> 
#include <vector>

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
	void Init(const KamataEngine::Vector3 spawnPos, const KamataEngine::Vector4 color);

	void Update();

	void Draw();

	static HitEffect* Create(const KamataEngine::Vector3 spawnPos, const KamataEngine::Vector4 color);

	static void SetModel(KamataEngine::Model* model) { model_ = model; }
	static void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; }

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
	static KamataEngine::Model* model_;

	// カメラ
	static KamataEngine::Camera* camera_;

	// --- インスタンスプロパティ ---

	// 状態
	State state_;

	// Alpha値
	float alphaColor_ = 0.0f;

	KamataEngine::ObjectColor objColor_;

	KamataEngine::Vector4 color_ = {1.0f, 1.0f, 1.0f, 1.0f};

	// 全サイズ
	float allShapeSize_ = 1.0f;

	// ライフカウンター
	float animCounter_ = 0.0f;

	// 円のワールドトランスフォーム
	KamataEngine::WorldTransform circleWorldTransform_;

	// 楕円のワールドトランスフォーム
	std::array<KamataEngine::WorldTransform, kEllipseValue> ellipseWorldTransforms_;
};

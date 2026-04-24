#pragma once
#include <KamataEngine.h> 
#include <vector>

/// <summary>
/// ヒット演出用エフェクト
/// </summary>
class GuardEffect {
public:
	enum class State{
		kAnim,
		kDead,
	};
public:
	void Init(const KamataEngine::Vector3 spawnPos);

	void Update();

	void Draw();

	static GuardEffect* Create(const KamataEngine::Vector3 spawnPos);

	static void SetModel(KamataEngine::Model* model) { model_ = model; }
	static void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; }

	bool IsDead() const { return state_ == State::kDead; }

public:
	// --- 定数 ---

	// 楕円のサイズ
	static inline const float kSizeStart = 0.0f;
	static inline const float kSizeEnd = 1.5f;

	// ライフタイム 
	static inline const float kAnimDuration = 0.25f;

private:

	// --- 外部依存 ---

	// モデル
	static KamataEngine::Model* model_;

	// カメラ
	static KamataEngine::Camera* camera_;

	// --- インスタンスプロパティ ---

	// 状態
	State state_;

	// 全サイズ
	float allShapeSize_ = 1.0f;

	// ライフカウンター
	float animCounter_ = 0.0f;

	// 円のワールドトランスフォーム
	KamataEngine::WorldTransform circleWorldTransform_;

};

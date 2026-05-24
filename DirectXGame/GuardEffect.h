#pragma once
#include  "KujakuEngine/KujakuEngine.h"
#include "BaseEffect.h"
#include <vector>

/// <summary>
/// ヒット演出用エフェクト
/// </summary>
class GuardEffect final : public BaseEffect {
public:
	void Init(const KujakuEngine::Vector3& spawnPos, KujakuEngine::Model* model, KujakuEngine::Camera* camera) override;

	void Update() override;

	void Draw() override;

	static GuardEffect* Create(const KujakuEngine::Vector3& spawnPos, KujakuEngine::Model* model, KujakuEngine::Camera* camera);

public:
	// --- 定数 ---

	// 楕円のサイズ
	static inline const float kSizeStart = 0.0f;
	static inline const float kSizeEnd = 1.5f;

	// ライフタイム 
	static inline const float kAnimDuration = 0.25f;

private:

	// --- インスタンスプロパティ ---
	
	// 全サイズ
	float allShapeSize_ = 1.0f;

	// ライフカウンター
	float animCounter_ = 0.0f;

	// 円のワールドトランスフォーム
	KujakuEngine::WorldTransform circleWorldTransform_;

};

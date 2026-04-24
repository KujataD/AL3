#pragma once
#include "IEnemyBehaviorState.h"
#include "KujakuEngine/KujakuEngine.h"

class EnemyBehaviorDead : public IEnemyBehaviorState {
public:

	void OnEnter(Enemy* enemy) override;
	void Update(Enemy* enemy) override;

	BehaviorType GetType() const override { return BehaviorType::kDead; }
private:
	// アニメーションタイマー
	float animTimer_;

	// デスアニメーションの初期回転
	KujakuEngine::Vector3 startRotation_;
	KujakuEngine::Vector3 endRotation_;
};

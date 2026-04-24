#pragma once
#include "IShieldEnemyBehaviorState.h"
#include  "KujakuEngine/KujakuEngine.h"

class ShieldEnemyBehaviorDead : public IShieldEnemyBehaviorState {
public:

	void OnEnter(ShieldEnemy* enemy) override;
	void Update(ShieldEnemy* enemy) override;

	BehaviorType GetType() const override { return BehaviorType::kDead; }
private:
	// アニメーションタイマー
	float animTimer_;

	// デスアニメーションの初期回転
	KujakuEngine::Vector3 startRotation_;
	KujakuEngine::Vector3 endRotation_;
};

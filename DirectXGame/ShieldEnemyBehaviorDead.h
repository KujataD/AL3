#pragma once
#include "IShieldEnemyBehaviorState.h"
#include "KamataEngine.h"

class ShieldEnemyBehaviorDead : public IShieldEnemyBehaviorState {
public:

	void OnEnter(ShieldEnemy* enemy) override;
	void Update(ShieldEnemy* enemy) override;

	BehaviorType GetType() const override { return BehaviorType::kDead; }
private:
	// アニメーションタイマー
	float animTimer_;

	// デスアニメーションの初期回転
	KamataEngine::Vector3 startRotation_;
	KamataEngine::Vector3 endRotation_;
};

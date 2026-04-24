#pragma once
#include "IEnemyBehaviorState.h"
#include "KamataEngine.h"

class EnemyBehaviorDead : public IEnemyBehaviorState {
public:

	void OnEnter(Enemy* enemy) override;
	void Update(Enemy* enemy) override;

	BehaviorType GetType() const override { return BehaviorType::kDead; }
private:
	// アニメーションタイマー
	float animTimer_;

	// デスアニメーションの初期回転
	KamataEngine::Vector3 startRotation_;
	KamataEngine::Vector3 endRotation_;
};

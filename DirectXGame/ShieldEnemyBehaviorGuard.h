#pragma once
#include "IShieldEnemyBehaviorState.h"
#include  "KujakuEngine/KujakuEngine.h"

class ShieldEnemyBehaviorGuard : public IShieldEnemyBehaviorState {
public:

	void OnEnter(ShieldEnemy* enemy) override;
	void Update(ShieldEnemy* enemy) override;

	BehaviorType GetType() const override { return BehaviorType::kGuard; }
private:
	// アニメーションタイマー
	float animTimer_;

	// ガードアニメーションの初期角度
	KujakuEngine::Vector3 startRotation_;

	// 上にのけぞったときの角度
	KujakuEngine::Vector3 upperRotation_;

	// 
	KujakuEngine::Vector3 endRotation_;
};

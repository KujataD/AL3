#pragma once
#include "IShieldEnemyBehaviorState.h"


class ShieldEnemyBehaviorRoot : public IShieldEnemyBehaviorState {
public:

	void Update(ShieldEnemy* enemy) override;
	void OnExit(ShieldEnemy* enemy) override;

	BehaviorType GetType() const override { return BehaviorType::kRoot; }
};

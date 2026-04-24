#pragma once
#include "IEnemyBehaviorState.h"


class EnemyBehaviorRoot : public IEnemyBehaviorState {
public:

	void Update(Enemy* enemy) override;
	void OnExit(Enemy* enemy) override;

	BehaviorType GetType() const override { return BehaviorType::kRoot; }
};

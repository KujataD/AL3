#pragma once
#include "BaseEnemyState.h"

class EnemyStateApproach : public BaseEnemyState {
public:
	EnemyStateApproach(Enemy* enemy);

	void Update() override;
};

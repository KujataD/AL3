#include "EnemyStateApproach.h"
#include "Enemy.h"
#include "EnemyStateLeave.h"

EnemyStateApproach::EnemyStateApproach(Enemy* enemy) : BaseEnemyState("State Approach", enemy) { enemy->InitApproach(); }

void EnemyStateApproach::Update() {
	enemy_->Approach();
	if (enemy_->GetPosition().z <= 0.0f) {
		enemy_->ChangeState(std::make_unique<EnemyStateLeave>(enemy_));
	}
}

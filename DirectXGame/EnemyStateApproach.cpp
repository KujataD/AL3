#include "EnemyStateApproach.h"
#include "EnemyStateLeave.h"
#include "Enemy.h"

EnemyStateApproach::EnemyStateApproach(Enemy* enemy) : BaseEnemyState("State Approach", enemy) {}

void EnemyStateApproach::Update() {
	enemy_->Approach();
	if (enemy_->GetPosition().z <= 0.0f) {
		enemy_->ChangeState(std::make_unique<EnemyStateLeave>(enemy_));
	}
}

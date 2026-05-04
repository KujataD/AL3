#include "EnemyStateLeave.h"
#include "Enemy.h"

EnemyStateLeave::EnemyStateLeave(Enemy* enemy) : BaseEnemyState("State Leave", enemy) { enemy_->ClearFireTimer(); }

void EnemyStateLeave::Update() { enemy_->Leave(); }


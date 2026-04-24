#include "EnemyBehaviorRoot.h"
#include "Enemy.h"

void EnemyBehaviorRoot::Update(Enemy* enemy) {

	// 移動
	enemy->Move();

	// トランスフォーム更新
	enemy->UpdateTransform();
}

void EnemyBehaviorRoot::OnExit(Enemy* enemy) { (void)enemy; }

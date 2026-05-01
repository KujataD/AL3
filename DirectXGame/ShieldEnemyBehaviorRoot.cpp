#include "ShieldEnemyBehaviorRoot.h"
#include "ShieldEnemy.h"


void ShieldEnemyBehaviorRoot::Update(ShieldEnemy* enemy) {

	// 移動
	enemy->Move();

	// トランスフォーム更新
	enemy->UpdateTransform();
}

void ShieldEnemyBehaviorRoot::OnExit(ShieldEnemy* enemy) { (void)enemy; }

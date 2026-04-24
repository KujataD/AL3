#include "ShieldEnemyBehaviorDead.h"
#include "ShieldEnemy.h"
#include <algorithm>

using namespace KujakuEngine;

void ShieldEnemyBehaviorDead::OnEnter(ShieldEnemy* enemy) {
	
	// アニメーションタイマーセット
	animTimer_ = ShieldEnemy::kDeadAnimDuration;
	
	// 始回転角
	startRotation_ = enemy->GetWorldRotation();
	
	// 終回転角
	endRotation_ = {ShieldEnemy::kDeadAnimRotationX, ShieldEnemy::kDeadAnimAdditionalRotationY, 0.0f};
	
	// コリジョン無効化
	enemy->CollisionDisable();
}

void ShieldEnemyBehaviorDead::Update(ShieldEnemy* enemy) {
	// タイマー加算
	animTimer_ -= 1.0f / 60.0f;

	// T
	float animT = std::clamp(1.0f - animTimer_ / ShieldEnemy::kDeadAnimDuration, 0.0f, 1.0f);

	// 回転角をイージングで変化
	enemy->SetRotationEaseOut(startRotation_, endRotation_, animT);

	// ワールドトランスフォームの行列更新
	enemy->UpdateTransform();

	// アニメーションタイマーがゼロになったらデスフラグを立てる
	if (animTimer_ <= 0.0f) {
		enemy->Destroy();
	}
}

#include "ShieldEnemyBehaviorGuard.h"
#include "ShieldEnemyBehaviorRoot.h"
#include "ShieldEnemy.h"
#include <algorithm>
#include <numbers>

using namespace KujakuEngine;

void ShieldEnemyBehaviorGuard::OnEnter(ShieldEnemy* enemy) {
	
	// アニメーションタイマーセット
	animTimer_ = 0.0f;
	
	// 始回転角
	startRotation_ = {0.0f, enemy->GetInitRotationY(), 0.0f};

	upperRotation_ = {0.0f, enemy->GetInitRotationY(), ShieldEnemy::kGuardAnimUpperRoatationZ};
	
	// 終回転角
	endRotation_ = startRotation_;
}

void ShieldEnemyBehaviorGuard::Update(ShieldEnemy* enemy) {
	// タイマー加算
	animTimer_ += 1.0f / 60.0f;

	if (animTimer_ < ShieldEnemy::kGuardAnimDuration / 2.0f) {
		// T
		float animT = std::clamp(animTimer_ / (ShieldEnemy::kGuardAnimDuration / 2.0f), 0.0f, 1.0f);
		// 回転角をイージングで変化
		enemy->SetRotationEaseOut(startRotation_, upperRotation_, animT);
	} else {
		// T
		float animT = std::clamp((animTimer_ - ShieldEnemy::kGuardAnimDuration / 2.0f) / (ShieldEnemy::kGuardAnimDuration / 2.0f), 0.0f, 1.0f);
		// 回転角をイージングで変化
		enemy->SetRotationEaseOut(upperRotation_, endRotation_, animT);
	}

	// ワールドトランスフォームの行列更新
	enemy->UpdateTransform();

	// アニメーションタイマーがゼロになったら移動処理に戻す
	if (animTimer_ >= ShieldEnemy::kGuardAnimDuration ) {
		enemy->ChangeBehavior(new ShieldEnemyBehaviorRoot);
	}
}

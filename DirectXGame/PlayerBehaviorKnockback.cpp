#include "PlayerBehaviorKnockback.h"
#include "Player.h"
#include "PlayerBehaviorRoot.h"
#include <algorithm>

using namespace KujakuEngine;

void PlayerBehaviorKnockback::Update(Player* player) {
	knockbackTimer_ += 1.0f / 60.0f;

	// 突進時の速度を取得
	knockbackMove_.x = player->GetKnockbackVelocityX();

	// マップ衝突判定
	player->UpdateCollision(knockbackMove_);

	// 旋回制御
	player->TurnControl();

	// トランスフォーム更新
	player->UpdateTransform();

	if (knockbackTimer_ >= Player::kAttackBlinkDashDuration) {
		isFinished_ = true;

		player->RequestBehavior(new PlayerBehaviorRoot, BehaviorChangeToken{});
	}
}

#include "PlayerBehaviorRoot.h"
#include "PlayerBehaviorAttack.h"
#include "Player.h"

void PlayerBehaviorRoot::Update(Player* player) {

	// スペースキーで攻撃
	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_SPACE) && player->CanAttack()) {
		// 攻撃ビヘイビアをリクエスト
		player->RequestBehavior(new PlayerBehaviorAttack, BehaviorChangeToken{});
		return;
	}
	
	// 移動入力
	player->Move();

	// マップ衝突判定
	player->UpdateCollision(player->GetVelocity());

	// 旋回制御
	player->TurnControl();

	// トランスフォーム更新
	player->UpdateTransform();
}

#include "PlayerBehaviorAttack.h"
#include "Player.h"
#include "PlayerBehaviorRoot.h"
#include "Transform.h"
#include <algorithm>

using namespace KamataEngine;

void PlayerBehaviorAttack::Enter(Player* player) {
	// 速度停止
	player->StopVelocity();
	player->DecreaseAttackCount(1);

	// フェーズを設定
	blinkPhase_ = BlinkPhase::kPrep;
	attackTimer_ = 0.0f;
}

void PlayerBehaviorAttack::Update(Player* player) {

	// ブリンク攻撃
	Blink(player);

	// 衝突判定
	player->UpdateCollision(attackMove_);

	// 行列更新
	player->UpdateTransform();

	// 終了フラグ
	if (isFinished_) {
		// ふるまいを変更
		player->RequestBehavior(new PlayerBehaviorRoot, BehaviorChangeToken{});
		return;
	}
}

void PlayerBehaviorAttack::Exit(Player* player) {
	// このアクションのクールタイムを開始する。
	player->StartAttackCoolTime();
	
	// サイズ初期化
	player->InitSize();

	// 攻撃エフェクトをfalseにする
	player->SetActiveAttackFX(false);
}

void PlayerBehaviorAttack::Blink(Player* player) {
	// タイマー更新
	attackTimer_ += 1.0f / 60.0f;
	attackMove_ = Vector3(0.0f, 0.0f, 0.0f);

	// 各フェーズ更新
	switch (blinkPhase_) {
	case PlayerBehaviorAttack::BlinkPhase::kPrep: {
		// 溜め処理
		// --------------------------------------

		// 溜め時間T
		float prepT = std::clamp(attackTimer_ / Player::kAttackBlinkPrepDuration, 0.0f, 1.0f);

		// サイズ変更(init -> dash)
		player->SetSizeEaseOut(Player::kInitSize, Player::kAttackBlinkDashSize, prepT);

		if (attackTimer_ >= Player::kAttackBlinkPrepDuration) {
			// 突進状態に移動
			blinkPhase_ = BlinkPhase::kDash;

			// 初期化
			attackTimer_ = 0.0f;

			// 攻撃エフェクトをアクティブ化
			player->SetActiveAttackFX(true);
		}
	} break;
	case PlayerBehaviorAttack::BlinkPhase::kDash: {
		// 突進処理
		// --------------------------------------

		// 突進時間T
		float dashT = std::clamp(attackTimer_ / Player::kAttackBlinkDashDuration, 0.0f, 1.0f);

		// サイズ変更(dash -> after)
		player->SetSizeEaseOut(Player::kAttackBlinkDashSize, Player::kAttackBlinkAfterSize, dashT);

		// 突進時の速度を取得
		attackMove_.x = player->GetBlinkVelocityX();

		if (attackTimer_ >= Player::kAttackBlinkDashDuration) {
			// 余韻状態に移動
			blinkPhase_ = BlinkPhase::kAfter;

			// 余韻時間を代入
			attackTimer_ = 0.0f;
		}
	} break;
	case PlayerBehaviorAttack::BlinkPhase::kAfter: {
		// 余韻処理
		// --------------------------------------

		// 余韻時間T
		float afterT = std::clamp(attackTimer_ / Player::kAttackBlinkAfterDuration, 0.0f, 1.0f);

		// サイズ変更(after -> init)
		player->SetSizeEaseOut(Player::kAttackBlinkAfterSize, Player::kInitSize, afterT);

		// 終了
		if (attackTimer_ >= Player::kAttackBlinkAfterDuration) {
			// 終了フラグON
			isFinished_ = true;
		}
	} break;
	default:
		break;
	}
}

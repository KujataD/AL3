#pragma once
#include "IPlayerBehavior.h"
#include <KamataEngine.h>

class PlayerBehaviorKnockback : public IPlayerBehavior {
public:

	void Update(Player* player) override;

	BehaviorType GetType() const override { return BehaviorType::kKnockback; }

private:
	
	// ノックバック時間のタイマー(秒)
	float knockbackTimer_ = 0.0f;

	// ノックバック時の移動
	KamataEngine::Vector3 knockbackMove_{};

	// ノックバック処理が終わったかどうか
	bool isFinished_ = false;
};

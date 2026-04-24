#pragma once
#include "IPlayerBehavior.h"
#include  "KujakuEngine/KujakuEngine.h"

class PlayerBehaviorAttack : public IPlayerBehavior {
public:
	void Enter(Player* player) override;

	void Update(Player* player) override;

	void Exit(Player* player) override;

	BehaviorType GetType() const override { return BehaviorType::kAttack; }

private:
	void Blink(Player* player);

private:
	enum class BlinkPhase {
		kPrep,
		kDash, 
		kAfter
	};

private:
	// --- 変数 ---

	BlinkPhase blinkPhase_;

	// アタック時間のタイマー(秒)
	float attackTimer_;

	// アタック時の移動
	KujakuEngine::Vector3 attackMove_{};

	// アタック処理が終わったかどうか
	bool isFinished_ = false;
};

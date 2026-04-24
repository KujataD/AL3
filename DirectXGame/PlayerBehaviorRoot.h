#pragma once
#include "IPlayerBehavior.h"

class PlayerBehaviorRoot : public IPlayerBehavior {
public:

	void Update(Player* player) override;

	BehaviorType GetType() const override { return BehaviorType::kRoot; }
};

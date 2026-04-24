#pragma once

class ShieldEnemy;

class IShieldEnemyBehaviorState {
public:
	IShieldEnemyBehaviorState() = default;
	virtual ~IShieldEnemyBehaviorState() = default;

	virtual void OnEnter(ShieldEnemy* enemy) { (void)enemy; }
	virtual void Update(ShieldEnemy* enemy) = 0;
	virtual void OnExit(ShieldEnemy* enemy) { (void)enemy; }

	enum BehaviorType { kRoot, kDead, kGuard };
	virtual BehaviorType GetType() const = 0;
};

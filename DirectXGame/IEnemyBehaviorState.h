#pragma once

class Enemy;

class IEnemyBehaviorState {
public:
	IEnemyBehaviorState() = default;
	virtual ~IEnemyBehaviorState() = default;

	virtual void OnEnter(Enemy* enemy) { (void)enemy; }
	virtual void Update(Enemy* enemy) = 0;
	virtual void OnExit(Enemy* enemy) { (void)enemy; }

	enum BehaviorType { kRoot, kDead };
	virtual BehaviorType GetType() const = 0;
};

#pragma once

// --- 前方宣言 ---
class Player;

class IPlayerBehavior {
public:
	virtual ~IPlayerBehavior() = default;

	/// <summary>
	/// 状態に入ったとき
	/// </summary>
	/// <param name="player"></param>
	virtual void Enter(Player* player) { (void)player; }

	// 状態中
	virtual void Update(Player* player) = 0;

	/// <summary>
	/// 状態を抜けるとき
	/// </summary>
	/// <param name="player"></param>
	virtual void Exit(Player* player) { (void)player; }

	enum class BehaviorType {
		kRoot,
		kAttack,
		kKnockback,
	};

	virtual BehaviorType GetType() const = 0;
};

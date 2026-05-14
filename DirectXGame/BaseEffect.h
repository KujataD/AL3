#pragma once
#include <KujakuEngine.h>

class BaseEffect {
public:
	enum class State{
		kAnim,
		kDead,
	};

public:

	virtual ~BaseEffect() = default;

	virtual void Init(const KujakuEngine::Vector3 spawnPos) = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	bool IsDead() const { return state_ == State::kDead; }
	bool IsAnim() const { return state_ == State::kDead; }

protected:


	// 状態
	State state_;

};

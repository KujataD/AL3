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

	virtual void Init(const KujakuEngine::Vector3& spawnPos, KujakuEngine::Model* model, KujakuEngine::Camera* camera) = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	bool IsDead() const { return state_ == State::kDead; }
	bool IsAnim() const { return state_ == State::kAnim; }

protected:

	// モデル
	KujakuEngine::Model* model_;

	// カメラ
	KujakuEngine::Camera* camera_;

	// 状態
	State state_;

};

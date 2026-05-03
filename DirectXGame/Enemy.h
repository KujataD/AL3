#pragma once
#include <KujakuEngine.h>
#include "BaseEnemyState.h"

class Enemy {
public:
	enum class Phase {
		Approach, // 接近
		Leave,    // 離脱
	};

	struct ParamKey {
		static inline const std::string kGroup = "Enemy";

		static inline const std::string kApproachVelocity = "ApproachVelocity";
		static inline const std::string kLeaveVelocity = "LeaveVelocity";
	};

	struct Param {
		static inline KujakuEngine::Vector3 approachVelocity_ = {0.0f, 0.0f, -0.4f};
		static inline KujakuEngine::Vector3 leaveVelocity_ = {-0.35f, 0.35f, -0.1f};
	};

public:
	void Initialize(KujakuEngine::Model* model, KujakuEngine::Camera* camera, KujakuEngine::Vector3 position);

	void Update();

	void Draw();

	/// <summary>
	/// 調整項目登録
	/// </summary>
	static void RegisterGlobalVariables();

	/// <summary>
	/// 調整項目適応
	/// </summary>
	static void ApplyGlobalVariables();

	// - get -
	bool IsDead() const { return isDead_; } 
 	const KujakuEngine::Vector3& GetPosition() const { return worldTransform_.translation_; }

	//　--- 外部API ---
	void Approach();

	void Leave();

	void ChangeState(std::unique_ptr<BaseEnemyState> state);

private:
private:
	KujakuEngine::Model* model_;
	KujakuEngine::Camera* camera_;

	// 内部プロパティ
	KujakuEngine::WorldTransform worldTransform_;

	bool isDead_ = false;

	std::unique_ptr<BaseEnemyState> state_;
};

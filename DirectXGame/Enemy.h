#pragma once
#include "BaseEnemyState.h"
#include "EnemyBullet.h"
#include "TimedCall.h"
#include <KujakuEngine.h>

class Player;

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
		static inline const std::string kBulletFireDuration = "BulletFireDuration";
	};

	struct Param {
		static inline KujakuEngine::Vector3 approachVelocity_ = {0.0f, 0.0f, -0.4f};
		static inline KujakuEngine::Vector3 leaveVelocity_ = {-0.35f, 0.35f, -0.1f};

		static inline float bulletFireDuration_ = 1.0f;
	};

public:
	void Initialize(KujakuEngine::Model* model, KujakuEngine::Model* modelBullet, KujakuEngine::Camera* camera, KujakuEngine::Vector3 position);

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

	// - set -
	void SetPlayer(Player* player) { player_ = player; }

	// - get -
	bool IsDead() const { return isDead_; }
	const KujakuEngine::Vector3& GetPosition() const { return worldTransform_.translation_; }
	KujakuEngine::Vector3 GetWorldPosition() const { return worldTransform_.GetWorldPosition(); }
	
	// --- 外部API ---
	void Approach();

	void InitApproach();

	void Leave();

	void Fire();

	void ChangeState(std::unique_ptr<BaseEnemyState> state);

	/// <summary>
	/// 弾発射のち、タイマーリセット
	/// </summary>
	void FireAndTimerReset();

	void ClearFireTimer();

private:
	void UpdateBullets();

private:
	KujakuEngine::Model* model_;
	KujakuEngine::Model* modelBullet_;
	KujakuEngine::Camera* camera_;
	Player* player_ = nullptr;

	// 内部プロパティ
	KujakuEngine::WorldTransform worldTransform_;
	bool isDead_ = false; // 死亡フラグ

	// 現在のフェーズ
	std::unique_ptr<BaseEnemyState> state_;

	// 弾
	std::list<std::unique_ptr<EnemyBullet>> bullets_;
	float fireTimer = 0.0f;
	std::list<std::unique_ptr<TimedCall>> timedCalls_;
};

#pragma once
#include <KujakuEngine.h>

class Player;

/// <summary>
/// 敵の弾
/// </summary>
class EnemyBullet {
public:
	struct ParamKey {
		static inline const std::string kGroup = "Enemy Bullet";
		static inline const std::string kSpeed = "Speed";
		static inline const std::string kHandlingPower = "HandlingPower";
	};

	struct Param {
		static inline float lifeTime_ = 5.0f;
		static inline float speed_ = 1.0f;
		static inline float handlingPower = 0.05f;
	};

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model"></param>
	/// <param name="camera"></param>
	/// <param name="position"></param>
	void Initialize(KujakuEngine::Model* model, KujakuEngine::Camera* camera, const KujakuEngine::Vector3& position, const KujakuEngine::Vector3& direction, bool isHoming = false);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	static void RegisterGlobalVariables();
	static void ApplyGlobalVariables();

	// -- get --
	bool IsDead() const { return isDead_; }

	// -- set --
	void SetPlayer(Player* player) { player_ = player; }

private:
	/// <summary>
	/// 速度の向きに合わせる
	/// </summary>
	void ApplyRotationOfVelocity();

private:
	KujakuEngine::Model* model_;
	KujakuEngine::Camera* camera_;
	Player* player_;

	// 内部プロパティ
	KujakuEngine::WorldTransform worldTransform_;
	KujakuEngine::Vector3 velocity_;

	// デスタイマー
	float deathTimer_;

	// デスフラグ
	bool isDead_ = false;
	bool isHoming_ = false;
};

#pragma once
#include <KujakuEngine.h>

/// <summary>
/// 敵の弾
/// </summary>
class EnemyBullet {
public:
	struct ParamKey {
		static inline const std::string kGroup = "Enemy Bullet";
	};

	struct Param {
		static inline float lifeTime_ = 5.0f;
	};

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model"></param>
	/// <param name="camera"></param>
	/// <param name="position"></param>
	void Initialize(KujakuEngine::Model* model, KujakuEngine::Camera* camera, const KujakuEngine::Vector3& position, const KujakuEngine::Vector3& velocity);

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

	// -- set --
	bool IsDead() const { return isDead_; }

private:
	KujakuEngine::Model* model_;
	KujakuEngine::Camera* camera_;

	// 内部プロパティ
	KujakuEngine::WorldTransform worldTransform_;
	KujakuEngine::Vector3 velocity_;

	// デスタイマー
	float deathTimer_;

	// デスフラグ
	bool isDead_ = false;
};

#pragma once
#include "PlayerBullet.h"
#include <KujakuEngine.h>
#include <list>

class Player : public KujakuEngine::Collider {
public:
	struct ParamKey {
		static inline const std::string kGroupKey = "Player";

		static inline const std::string kSpeed = "Speed";
		static inline const std::string kMoveLimitBlank = "MoveLimitBlank";
		static inline const std::string kRotateSpeed = "RotateSpeed";
		static inline const std::string kBulletSpeed = "BulletSpeed";
	};

	struct Param {
		static inline float speed_ = 0.1f;
		static inline float moveLimitBlank_ = 1.0f;
		static inline float rotateSpeed_ = 0.01f;
		static inline float bulletSpeed_ = 0.03f;
	};

public:
	~Player();

	void Initialize(KujakuEngine::Model* model, KujakuEngine::Model* modelBullet, KujakuEngine::Camera* camera);
	void Update();
	void Draw();

	// --- set ---

	void SetCamera(KujakuEngine::Camera* camera) {
		camera_ = camera;
		worldTransform_.UpdateMatrix(*camera_);
	}

	void SetParent(const KujakuEngine::WorldTransform* parent) { worldTransform_.parent_ = parent; }

	// --- get ---
	const KujakuEngine::Vector3& GetWorldPosition() const override { return worldTransform_.GetWorldPosition(); }
	const std::list<std::unique_ptr<PlayerBullet>>& GetBullets() { return bullets_; }

	// --- 外部API ---
	static void RegisterGlobalVariables();
	static void ApplyGlobalVariables();
	void OnCollision() override;

private:
	/// <summary>
	/// 移動
	/// </summary>
	void Move();

	/// <summary>
	/// 回転
	/// </summary>
	void Rotate();

	/// <summary>
	/// IMGUIの管理
	/// </summary>
	void ManageImGui();

	/// <summary>
	/// 画面内に納めます
	/// </summary>
	void ClampInWindow();

	/// <summary>
	/// 攻撃処理
	/// </summary>
	void Fire();

	/// <summary>
	/// 弾の更新
	/// </summary>
	void UpdateBullets();

	/// <summary>
	/// 弾の更新
	/// </summary>
	void Update3DReticle();

private:
	// 外部受け取り
	// ------------------------------------------
	KujakuEngine::Camera* camera_ = nullptr;
	KujakuEngine::Model* model_ = nullptr;
	KujakuEngine::Model* modelBullet_ = nullptr;

	// 内部プロパティ
	// ------------------------------------------
	KujakuEngine::WorldTransform worldTransform_;

	// 3Dレティクル
	// ------------------------------------------
	// ワールドトランスフォーム
	KujakuEngine::WorldTransform worldTransform3DReticle_;

	// モデル
	std::unique_ptr<KujakuEngine::Model> model3DReticle_ = nullptr;

	// 弾
	// ------------------------------------------
	std::list<std::unique_ptr<PlayerBullet>> bullets_;
};

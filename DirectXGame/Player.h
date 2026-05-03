#pragma once
#include <KujakuEngine.h>
#include "PlayerBullet.h"

class Player {
public:
	struct ParamKey {
		static inline const std::string kGroupKey = "Player";

		static inline const std::string kSpeed = "Speed";
		static inline const std::string kMoveLimitBlank = "MoveLimitBlank";
		static inline const std::string kRotateSpeed = "RotateSpeed";
	};

	struct Param {
		static inline float speed_ = 0.1f;
		static inline float moveLimitBlank_ = 1.0f;
		static inline float rotateSpeed_ = 0.01f;
	};
public:

	void Initialize(KujakuEngine::Model* model, KujakuEngine::Model* modelBullet, KujakuEngine::Camera* camera);
	void Update();
	void Draw();

	void SetCamera(KujakuEngine::Camera* camera) {
		camera_ = camera;
		worldTransform_.UpdateMatrix(*camera_);
	}

	static void RegisterGlobalVariables();

	static void ApplyGlobalVariables();
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

	void Attack();

private:

	// 外部受け取り
	// ------------------------------------------
	KujakuEngine::Camera* camera_ = nullptr;
	KujakuEngine::Model* model_ = nullptr;

	// 内部プロパティ
	// ------------------------------------------
	KujakuEngine::WorldTransform worldTransform_;

	// 弾
	// ------------------------------------------
	PlayerBullet* bullet_ = nullptr;
	KujakuEngine::Model* modelBullet_ = nullptr;

};

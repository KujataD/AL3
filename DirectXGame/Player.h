#pragma once
#include "BaseCharacter.h"
#include <KujakuEngine.h>
#include <optional>

class Player : public BaseCharacter, public KujakuEngine::Collider {
public:
	enum ModelIndex {
		kModelIndexBody,
		kModelIndexHead,
		kModelIndexArm_L,
		kModelIndexArm_R,
		kModelIndexWeapon,
		kModelIndexCount,
	};

	struct ParamKey {
		static inline const std::string kGroupKey = "Player";

		static inline const std::string kSpeed = "Speed";
		static inline const std::string kFloatingAmplitude = "FloatingAmplitude";
		static inline const std::string kFloatingArmRotationAmplitude = "FloatingArmRotationAmplitude";
		static inline const std::string kFloatingCycle = "FloatingCycle";
		static inline const std::string kOffsetTranslateBody = "OffsetTransformBody";
		static inline const std::string kOffsetTranslateHead = "OffsetTransformHead";
		static inline const std::string kOffsetTranslateArm_L = "OffsetTransformArm_L";
		static inline const std::string kOffsetTranslateArm_R = "OffsetTransformArm_R";
		static inline const std::string kOffsetRotateArm_L = "OffsetRotateArm_L";
		static inline const std::string kOffsetRotateArm_R = "OffsetRotateArm_R";
	};

	struct Param {
		static inline float speed_ = 0.1f;
		static inline float floatingAmplitude_ = 1.0f;
		static inline float floatingArmRotationAmplitude_ = std::numbers::pi_v<float> *0.25f;
		static inline int floatingCycle_ = 60;
		static inline KujakuEngine::Vector3 offsetTranslateBody_{};
		static inline KujakuEngine::Vector3 offsetTranslateHead_{};
		static inline KujakuEngine::Vector3 offsetTranslateArm_L_{};
		static inline KujakuEngine::Vector3 offsetTranslateArm_R_{};
		static inline KujakuEngine::Vector3 offsetRotateArm_L_{};
		static inline KujakuEngine::Vector3 offsetRotateArm_R_{};
	};

	enum class ControlType {
		kControlTypeKeyboard,
		kControlTypeGamepad,
	};

	enum class Behavior {
		kRoot,		// <! 通常
		kAttack,	// <! 攻撃中
	};

public:
	~Player() override;

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(const std::vector<KujakuEngine::Model*>& models, const KujakuEngine::Camera* camera) override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画関数
	/// </summary>
	void Draw() override;

	// --- set ---

	void SetCamera(const KujakuEngine::Camera* camera) {
		camera_ = camera;
		BaseCharacter::Update();
		worldTransformBody_.UpdateMatrix(*camera_);
		worldTransformHead_.UpdateMatrix(*camera_);
		worldTransformArm_L_.UpdateMatrix(*camera_);
		worldTransformArm_R_.UpdateMatrix(*camera_);
	}
	void SetViewProjection(const KujakuEngine::Camera* viewProjection) { viewProjection_ = viewProjection; }
	void SetParent(const KujakuEngine::WorldTransform* parent) { worldTransform_.parent_ = parent; }

	// --- get ---
	KujakuEngine::Vector3 GetWorldPosition() const override { return worldTransform_.GetWorldPosition(); }
	KujakuEngine::WorldTransform* GetWorldTransform() { return &worldTransform_; }

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
	/// IMGUIの管理
	/// </summary>
	void ManageImGui();

	/// <summary>
	/// コントローラー操作かキーボード操作かを判定して、レティクルの表示方法を切り替える
	/// </summary>
	void UpdateControlType();

	// 浮遊ギミック更新
	void InitializeFloatingGimmick();

	// 富裕ギミック更新
	void UpdateFloatingGimmick();

	// 通常行動更新
	void BehaviorRootUpdate();

	void BehaviorAttackUpdate();

	/// <summary>
	/// すべての行列の更新
	/// </summary>
	void UpdateWorldTransforms();


	// 通常行動初期化
	void BehaviorRootInitialize();
	// 攻擊行動初期化
	void BehaviorAttackInitialize();

private:
	// 外部受け取り
	// ------------------------------------------
	const KujakuEngine::Camera* viewProjection_ = nullptr;

	// 内部プロパティ
	// ------------------------------------------
	KujakuEngine::WorldTransform worldTransformBody_;
	KujakuEngine::WorldTransform worldTransformHead_;
	KujakuEngine::WorldTransform worldTransformArm_L_;
	KujakuEngine::WorldTransform worldTransformArm_R_;
	KujakuEngine::WorldTransform worldTransformWeapon_;
	KujakuEngine::WorldTransform worldTransformShoulder_;

	// 浮遊
	float floatingParameter_ = 0.0f;
	// 操作
	// ------------------------------------------
	ControlType controlType_ = ControlType::kControlTypeKeyboard;

	// B
	// ------------------------------------------
	Behavior behavior_ = Behavior::kRoot;
	std::optional<Behavior> behaviorRequest_ = std::nullopt;
};

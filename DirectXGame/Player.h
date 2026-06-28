#pragma once
#include <KujakuEngine.h>

class Player : public KujakuEngine::Collider {
public:
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
		static inline float floatingArmRotationAmplitude_ = std::numbers::pi_v<float> * 0.25f;
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

public:
	~Player();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(KujakuEngine::Model* modelHead, KujakuEngine::Model* modelBody, KujakuEngine::Model* modelArm_L, KujakuEngine::Model* modelArm_R, KujakuEngine::Camera* camera);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画関数
	/// </summary>
	void Draw();

	// --- set ---

	void SetCamera(KujakuEngine::Camera* camera) {
		camera_ = camera;
		worldTransformBase_.UpdateMatrix(*camera_);
	}
	void SetViewProjection(const KujakuEngine::Camera* viewProjection) { viewProjection_ = viewProjection; }
	void SetParent(const KujakuEngine::WorldTransform* parent) { worldTransformBase_.parent_ = parent; }

	// --- get ---
	KujakuEngine::Vector3 GetWorldPosition() const override { return worldTransformBase_.GetWorldPosition(); }
	KujakuEngine::WorldTransform* GetWorldTransform() { return &worldTransformBase_; }

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
private:
	// 外部受け取り
	// ------------------------------------------
	KujakuEngine::Camera* camera_ = nullptr;
	const KujakuEngine::Camera* viewProjection_ = nullptr;

	// モデル
	KujakuEngine::Model* modelBody_ = nullptr;
	KujakuEngine::Model* modelHead_ = nullptr;
	KujakuEngine::Model* modelArm_L_ = nullptr;
	KujakuEngine::Model* modelArm_R_ = nullptr;

	// 内部プロパティ
	// ------------------------------------------
	KujakuEngine::WorldTransform worldTransformBase_;
	KujakuEngine::WorldTransform worldTransformBody_;
	KujakuEngine::WorldTransform worldTransformHead_;
	KujakuEngine::WorldTransform worldTransformArm_L_;
	KujakuEngine::WorldTransform worldTransformArm_R_;

	// 浮遊
	float floatingParameter_ = 0.0f;
	// 操作
	// ------------------------------------------
	ControlType controlType_ = ControlType::kControlTypeKeyboard;

};

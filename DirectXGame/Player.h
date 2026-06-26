#pragma once
#include <KujakuEngine.h>

class Player : public KujakuEngine::Collider {
public:
	struct ParamKey {
		static inline const std::string kGroupKey = "Player";

		static inline const std::string kSpeed = "Speed";
	};

	struct Param {
		static inline float speed_ = 0.1f;
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
	void Initialize(KujakuEngine::Model* model, KujakuEngine::Camera* camera);

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
		worldTransform_.UpdateMatrix(*camera_);
	}

	void SetParent(const KujakuEngine::WorldTransform* parent) { worldTransform_.parent_ = parent; }

	// --- get ---
	KujakuEngine::Vector3 GetWorldPosition() const override { return worldTransform_.GetWorldPosition(); }

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

private:
	// 外部受け取り
	// ------------------------------------------
	KujakuEngine::Camera* camera_ = nullptr;
	KujakuEngine::Model* model_ = nullptr;

	// 内部プロパティ
	// ------------------------------------------
	KujakuEngine::WorldTransform worldTransform_;

	// 操作
	// ------------------------------------------
	ControlType controlType_ = ControlType::kControlTypeKeyboard;
};

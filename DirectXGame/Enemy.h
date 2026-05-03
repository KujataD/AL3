#pragma once
#include <KujakuEngine.h>

class Enemy {
public:
	struct ParamKey {
		static inline const std::string kGroup = "Enemy";
		static inline const std::string kSpeed = "Speed";
	};

	struct Param {
		static inline float speed_ = 1.0f;
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

private:

	KujakuEngine::Model* model_;
	KujakuEngine::Camera* camera_;
	
	// 内部プロパティ
	KujakuEngine::WorldTransform worldTransform_;

	bool isDead_ = false;
};

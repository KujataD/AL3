#pragma once
#include <KujakuEngine.h>

class Player {
public:
	struct ParamKeys {
		static inline const std::string kGroupKey = "Player";

		static inline const std::string kSpeedKey = "Speed";
		static inline const std::string kMoveLimitBlank = "MoveLimitBlank";
	};

	struct Param {
		static inline float speed_ = 0.1f;
		static inline float moveLimitBlank_ = 1.0f;
	};
public:

	void Initialize(KujakuEngine::Model* model, KujakuEngine::Camera* camera);
	void Update();
	void Draw();

	void SetCamera(KujakuEngine::Camera* camera) {
		camera_ = camera;
		worldTransform_.UpdateMatrix(*camera_);
	}

	static void RegisterGlobalVariables();
	static void ApplyGlobalVariables();

private:

	// 外部受け取り
	// ------------------------------------------
	KujakuEngine::Camera* camera_ = nullptr;
	KujakuEngine::Model* model_ = nullptr;

	// 内部プロパティ
	// ------------------------------------------
	KujakuEngine::WorldTransform worldTransform_;
};

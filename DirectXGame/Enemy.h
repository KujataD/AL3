#pragma once
#include "BaseCharacter.h"
#include <KujakuEngine.h>
#include <numbers>

class Enemy : public BaseCharacter {
public:
	enum ModelIndex {
		kModelIndexBody,
		kModelIndexArm_L,
		kModelIndexArm_R,
		kModelIndexCount,
	};

	struct ParamKey {
		static inline const std::string kGroupKey = "Enemy";

		static inline const std::string kMoveSpeed = "MoveSpeed";
		static inline const std::string kRotateSpeed = "RotateSpeed";
		static inline const std::string kRotationOffsetY = "RotationOffsetY";
		static inline const std::string kOffsetTranslateBody = "OffsetTransformBody";
		static inline const std::string kOffsetTranslateArm_L = "OffsetTransformArm_L";
		static inline const std::string kOffsetTranslateArm_R = "OffsetTransformArm_R";
		static inline const std::string kOffsetRotateBody = "OffsetRotateBody";
		static inline const std::string kOffsetRotateArm_L = "OffsetRotateArm_L";
		static inline const std::string kOffsetRotateArm_R = "OffsetRotateArm_R";
	};

	struct Param {
		static inline float moveSpeed_ = 0.08f;
		static inline float rotateSpeed_ = 0.03f;
		static inline float rotationOffsetY_ = std::numbers::pi_v<float>;
		static inline KujakuEngine::Vector3 offsetTranslateBody_ = {0.0f, 0.5f, 0.0f};
		static inline KujakuEngine::Vector3 offsetTranslateArm_L_ = {0.8f, 0.0f, 0.0f};
		static inline KujakuEngine::Vector3 offsetTranslateArm_R_ = {-0.8f, 0.0f, 0.0f};
		static inline KujakuEngine::Vector3 offsetRotateBody_{};
		static inline KujakuEngine::Vector3 offsetRotateArm_L_{};
		static inline KujakuEngine::Vector3 offsetRotateArm_R_{};
	};

public:
	~Enemy() override;

	void Initialize(const std::vector<KujakuEngine::Model*>& models, const KujakuEngine::Camera* camera) override;
	void Update() override;
	void Draw() override;

	void SetCamera(const KujakuEngine::Camera* camera);

	static void RegisterGlobalVariables();
	static void ApplyGlobalVariables();

private:
	void Move();

private:
	KujakuEngine::WorldTransform worldTransformBody_;
	KujakuEngine::WorldTransform worldTransformArm_L_;
	KujakuEngine::WorldTransform worldTransformArm_R_;
};

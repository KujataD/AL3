#pragma once
#include <KujakuEngine.h>

class Player {
public:
	void Initialize(KujakuEngine::Model* model, KujakuEngine::Camera* camera);
	void Update();
	void Draw();

	void SetCamera(KujakuEngine::Camera* camera) {
		camera_ = camera;
		worldTransform_.UpdateMatrix(*camera_);
	}

private:
	// 外部受け取り
	// ------------------------------------------
	KujakuEngine::Camera* camera_ = nullptr;
	KujakuEngine::Model* model_ = nullptr;

	// 内部プロパティ
	// ------------------------------------------
	KujakuEngine::WorldTransform worldTransform_;
};

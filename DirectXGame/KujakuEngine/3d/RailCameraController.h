#pragma once
#include "WorldTransform.h"
#include "Camera.h"

namespace KujakuEngine {

class RailCameraController {
public:
	
	void Initialize(Vector3 rotation, Vector3 position);

	void Update();

	const Matrix4x4& GetViewMatrix() const { return camera_.matView; }
	const WorldTransform* GetWorldTransform() const { return &worldTransform_; }

private:
	Camera camera_;

	WorldTransform worldTransform_;
};

} // namespace KujakuEngine
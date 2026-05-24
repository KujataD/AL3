#include "AvoidanceBehavior.h"
#include <math/MathUtil.h>

namespace KujakuEngine {
namespace SteeringBehaviors {

Vector3 AvoidanceBehavior::Calculate(const SteeringContext& context) const {
	if (param_.obstacles.empty()) {
		return {};
	}

	// 回避速度
	Vector3 avoid{};

	// 進行方向
	Vector3 forward = Normalize(context.velocity);
	//
	//forward.x = cosf(param_.rotation.x) * sinf(param_.rotation.y);
	//forward.y = sinf(param_.rotation.x);
	//forward.z = cosf(param_.rotation.x) * cosf(param_.rotation.y);

	//forward = Normalize(forward);

	Segment rays[3];

	// 正面のRay
	rays[0].origin = context.position;
	rays[0].diff = forward * param_.rayLength;

	rays[1].origin = context.position;
	rays[1].diff.x = forward.x * std::cos(param_.fov) - forward.z * std::sin(param_.fov);
	rays[1].diff.y = forward.y;
	rays[1].diff.z = forward.x * std::sin(param_.fov) + forward.z * std::cos(param_.fov);
	rays[1].diff = Normalize(rays[1].diff) * param_.rayLength;

	rays[2].origin = context.position;
	rays[2].diff.x = forward.x * std::cos(-param_.fov) - forward.z * std::sin(-param_.fov);
	rays[2].diff.y = forward.y;
	rays[2].diff.z = forward.x * std::sin(-param_.fov) + forward.z * std::cos(-param_.fov);
	rays[2].diff = Normalize(rays[2].diff) * param_.rayLength;

	for (const AABB& obstacle : param_.obstacles) {
		for (int i = 0; i < 3; i++) {
			if (ShapeUtil::IsCollision(obstacle, rays[i])) {
				Vector3 wallCenter = (obstacle.min + obstacle.max) * 0.5f;

				Vector3 awayDir = Normalize(context.position - wallCenter);

				// 進行方向の逆向き成分を消して、横に避ける
				awayDir = awayDir - forward * Dot(awayDir, forward);

				avoid += Normalize(awayDir);
			}
		}
	}
	return Normalize(avoid) * context.maxForce;
}

} // namespace SteeringBehaviors
} // namespace KujakuEngine
#pragma once
#include <assert.h>
#include <cmath>
#include <numbers>

namespace KujakuEngine {

class Matrix4x4;
class Segment;

class Vector3 {
public:
	float x;
	float y;
	float z;

	Vector3 operator+(const Vector3& v) const { return {x + v.x, y + v.y, z + v.z}; }
	Vector3 operator-(const Vector3& v) const { return {x - v.x, y - v.y, z - v.z}; }
	Vector3 operator-() const { return {-x, -y, -z}; }

	void operator+=(const Vector3& v) {
		x += v.x;
		y += v.y;
		z += v.z;
	}
	void operator-=(const Vector3& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}
	void operator*=(float scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
	}
	void operator/=(float scalar) {
		x /= scalar;
		y /= scalar;
		z /= scalar;
	}
};

inline Vector3 operator+(Vector3 v, float f) { return {v.x + f, v.y + f, v.z + f}; }
inline Vector3 operator-(Vector3 v, float f) { return {v.x - f, v.y - f, v.z - f}; }
inline Vector3 operator*(Vector3 v, float f) { return {v.x * f, v.y * f, v.z * f}; }
inline Vector3 operator/(Vector3 v, float f) { return {v.x / f, v.y / f, v.z / f}; }
inline Vector3 operator+(float f, Vector3 v) { return {v.x + f, v.y + f, v.z + f}; }
inline Vector3 operator-(float f, Vector3 v) { return {v.x - f, v.y - f, v.z - f}; }
inline Vector3 operator*(float f, Vector3 v) { return {v.x * f, v.y * f, v.z * f}; }

float Dot(const Vector3& v1, const Vector3& v2);
Vector3 Cross(const Vector3& a, const Vector3& b);
float Length(const Vector3& v);
Vector3 Normalize(const Vector3& v);
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);
Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);
Vector3 Bezier(const Vector3& p0, const Vector3& p1, const Vector3& p2, float t);
Vector3 Reflect(const Vector3& input, const Vector3& normal);
Vector3 Transform(const Vector3& v, const Matrix4x4& m);
Vector3 Project(const Vector3& a, const Vector3& b);
Vector3 ClosestPoint(const Vector3& point, const Segment& segment);
Vector3 Perpendicular(const Vector3& vector);
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);
inline float NormalizeAngle(float angle) {
	while (angle > std::numbers::pi_v<float>) {
		angle -= 2.0f * std::numbers::pi_v<float>;
	}
	while (angle < -std::numbers::pi_v<float>) {
		angle += 2.0f * std::numbers::pi_v<float>;
	}
	return angle;
}

// NearAngle を修正: 差分が ±π に達したら現在値を優先して符号を固定
inline float NearAngle(float current, float target) {
	float diff = NormalizeAngle(target - current);
	// ±π付近(真逆方向)では符号が不定になるため、現在の回転方向を維持する
	constexpr float kFlipThreshold = std::numbers::pi_v<float> - 0.01f;
	if (diff > kFlipThreshold) {
		diff = kFlipThreshold;
	} else if (diff < -kFlipThreshold) {
		diff = -kFlipThreshold;
	}
	return current + diff;
}

} // namespace KujakuEngine
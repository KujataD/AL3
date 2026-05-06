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

} // namespace KujakuEngine
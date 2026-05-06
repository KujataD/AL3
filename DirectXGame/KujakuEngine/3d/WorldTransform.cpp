#include "WorldTransform.h"
#include "../base/DirectXCommon.h"
#include "Camera.h"
#include <cassert>
#include <numbers>

namespace KujakuEngine {

void WorldTransform::Initialize() {
	// 定数バッファの生成
	transformationMatrixResource_ = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(TransformationMatrix));

	// マッピング
	HRESULT hr = transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&constMap_));
	assert(SUCCEEDED(hr));

	// 単位行列で初期化
	constMap_->WVP = MakeIdentity();
	constMap_->World = MakeIdentity();
}

void WorldTransform::UpdateMatrix(const Camera& camera) {
	// ワールド行列の生成
	matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);

	// 親がいれば親のワールド行列を掛ける（階層構造）
	if (parent_) {
		matWorld_ = matWorld_ * parent_->matWorld_;
	}

	TransferMatrix(camera);
}

void WorldTransform::UpdateBillboardMatrix(const Camera& camera) {
	// ワールド行列の生成
	Matrix4x4 billboardMatrix = kBackToFrontMatrix * Inverse(camera.matView);
	billboardMatrix.m[3][0] = 0.0f; // 平行移動成分は要らない
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	// 回転対応
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotation_.z);

	Matrix4x4 rotateMatrix = rotateZMatrix * billboardMatrix;
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale_);
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translation_);

	matWorld_ = scaleMatrix * rotateMatrix * translateMatrix;

	TransferMatrix(camera);
}

void WorldTransform::TransferMatrix(const Camera& camera) { // WVP行列の生成
	Matrix4x4 matWVP = matWorld_ * camera.matView * camera.matProjection;

	// 定数バッファへ転送
	constMap_->WVP = matWVP;
	constMap_->World = matWorld_;
	constMap_->WorldInverseTranspose = Transpose(Inverse(matWorld_));
}

TransformationMatrix WorldTransform::GetMatrixData(const Camera& camera) const {
	TransformationMatrix data;

	Matrix4x4 matWVP = matWorld_ * camera.matView * camera.matProjection;

	data.WVP = matWVP;
	data.World = matWorld_;

	return data;
}

TransformationMatrix WorldTransform::GetBillboardMatrixData(const Camera& camera) const { return TransformationMatrix(); }

void WorldTransform::ApplyRotationOfVelocity(const Vector3& velocity, const Vector3& deltaAngle) {
	// Y軸周り角度(θy) ...atan2(高さ, 底辺)
	float targetY = std::atan2(velocity.x, velocity.z);
	// 横軸方向の長さを求める
	float velocityXZ = Length({velocity.x, 0.0f, velocity.z});
	// X軸周り角度(θx)
	float targetX = std::atan2(-velocity.y, velocityXZ);

	rotation_.x = targetX;
	rotation_.y = targetY;

	rotation_ += deltaAngle;
}

} // namespace KujakuEngine
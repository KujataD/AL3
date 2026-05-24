#pragma once
#include "Particle.h"
#include "ParticleField.h"
#include "ParticleModel.h"
#include <math/MathUtil.h>
#include <3d/Model.h>

#include <list>
#include <vector>

namespace KujakuEngine {

class ParticleEmitter {
public:
	// 生成図形タイプ
	enum EmitShape {
		kEmitShapeBox,       // !< 従来の直方体からランダム生成する。
		kEmitShapeModelEdge, // !< モデルの辺からランダム生成する。（SetSourceModel必要）
	};

public:
	void Initialize(ParticleModel* model);

	void Update(float deltaTime, const Camera& camera);
	void Draw();

	void Emit();

	// --- set ---
	void AddField(AccelerationField& field) { accelerationFields_.push_back(field); }
	void SetIsActiveField(bool isActive) { isActiveField_ = isActive; }
	void SetSourceModel(Model* sourceModel, WorldTransform* sourceWorldTransform) {
		sourceModel_ = sourceModel;
		sourceWorldTransform_ = sourceWorldTransform;
	}

	// --- get ---

	/// <summary>
	/// ソースモデルの辺のランダムな座標を取得します。
	/// </summary>
	Vector3 GetRandomPosModelEdge();

private:
	Particle MakeParticle();

public:
	Vector3 translation_;
	Vector3 rotation_;
	Vector3 scale_ = {1.0f, 1.0f, 1.0f};

	uint32_t count_ = 3;         // !< 発生数
	float frequency_ = 0.5f;     // !< 発生頻度
	float frequencyTime_ = 0.0f; // !< 頻度用時刻

	EmitShape emitShape_ = kEmitShapeBox;

	Vector3 particleScale_ = {1.0f, 1.0f, 1.0f};
	Vector2 lifeTimeMinMax_ = {1.0f, 3.0f};
private:
	// モデルを使って生成する場合に必要
	Model* sourceModel_ = nullptr;
	WorldTransform* sourceWorldTransform_ = nullptr;

	// パーティクルモデル
	ParticleModel* model_ = nullptr;

	std::list<Particle> particles_;

	// フィールド
	std::list<AccelerationField> accelerationFields_;
	bool isActiveField_ = true;

};

} // namespace KujakuEngine

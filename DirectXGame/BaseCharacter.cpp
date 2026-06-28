#include "BaseCharacter.h"
using namespace KujakuEngine;
void BaseCharacter::Initialize(const std::vector<KujakuEngine::Model*>& models, const KujakuEngine::Camera* camera) {
	// 引数項目の代入
	models_ = models;
	camera_ = camera;

	// ワールドトランスフォーム初期化
	worldTransform_.Initialize();
}

void BaseCharacter::Update() {

	worldTransform_.UpdateMatrix(*camera_);
}

void BaseCharacter::Draw() {

	for (Model* model : models_) {
		model->Draw(worldTransform_, *camera_);
	}
}

#include "GameScene.h"

using namespace KujakuEngine;

GameScene::~GameScene() {
	delete player_;
	delete modelPlayer_;
	delete modelBullet_;
}

void GameScene::Initialize() {

	// カメラ
	// ------------------------------------------
	camera_.Initialize();
	camera_.translation_ = {0.0f, 0.0f, -100.0f};
	debugCamera_.Initialize(camera_.rotation_, camera_.translation_);

	// プレイヤー
	// ------------------------------------------
	modelPlayer_ = Model::CreateFromOBJ("airship", ShaderModel::kHalfLambert);
	player_ = new Player;
	modelBullet_ = Model::CreateCube("resources/white1x1.png");
	player_->Initialize(modelPlayer_, modelBullet_, &camera_);

	RegisterAllVariables();
}

void GameScene::Update() {
	ApplyAllVariables();

	UpdateCamera();
	player_->Update();
}

void GameScene::Draw() {
	Model::PreDraw();
	player_->Draw();
}

void GameScene::UpdateCamera() {
#ifdef _DEBUG
	if (Input::GetKeyTrigger(DIK_P)) {
		isActiveDebugCamera_ = !isActiveDebugCamera_;
	}

#endif // _DEBUG
	// カメラの処理
	if (isActiveDebugCamera_) {
		debugCamera_.Update();
		debugCamera_.UpdateViewMatrix();
		camera_.matView = debugCamera_.GetViewMatrix();
		camera_.UpdateProjectionMatrix();
		camera_.TransferConstBuffer();

	} else {
		// ビュープロジェクション行列の更新と転送
		camera_.UpdateMatrix();
	}
}

void GameScene::ApplyAllVariables() {
	Player::ApplyGlobalVariables();
	PlayerBullet::ApplyGlobalVariables();
}

void GameScene::RegisterAllVariables() {
	Player::RegisterGlobalVariables();
	PlayerBullet::RegisterGlobalVariables();
}

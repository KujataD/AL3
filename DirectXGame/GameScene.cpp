#include "GameScene.h"

using namespace KujakuEngine;

GameScene::~GameScene() {
}

void GameScene::Initialize() {

	// カメラ
	// ------------------------------------------
	camera_.Initialize();
	camera_.translation_ = {0.0f, 0.0f, -100.0f};
	debugCamera_.Initialize(camera_.rotation_, camera_.translation_);

	// プレイヤー
	// ------------------------------------------
	modelPlayer_ = std::unique_ptr<Model>(Model::CreateFromOBJ("airship", ShaderModel::kHalfLambert));
	modelBullet_ = std::unique_ptr<Model>(Model::CreateCube("resources/white1x1.png"));
	player_ = std::make_unique<Player>();
	player_->Initialize(modelPlayer_.get(), modelBullet_.get(), &camera_);

	// エネミー
	// ------------------------------------------
	modelEnemy_ = std::unique_ptr<Model>(Model::CreateFromOBJ("enemy_airship", ShaderModel::kHalfLambert));
	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize(modelEnemy_.get(), &camera_, {0.0f, 3.0f, 200.0f});

	RegisterAllVariables();
}

void GameScene::Update() {
	ApplyAllVariables();

	UpdateCamera();
	player_->Update();
	enemy_->Update();
}

void GameScene::Draw() {
	Model::PreDraw();
	player_->Draw();
	enemy_->Draw();
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
	Enemy::ApplyGlobalVariables();
}

void GameScene::RegisterAllVariables() {
	Player::RegisterGlobalVariables();
	PlayerBullet::RegisterGlobalVariables();
	Enemy::RegisterGlobalVariables();
}

#include "GameScene.h"

using namespace KujakuEngine;
using namespace ShapeUtil;

GameScene::~GameScene() {}

void GameScene::Initialize() {

	// テクスチャ
	// ------------------------------------------
	TextureManager::GetInstance()->LoadTexture("Resources/reticle.png");

	// カメラ
	// ------------------------------------------
	camera_.Initialize();
	camera_.translation_ = { 0.0f, 5.0f, -20.0f };
	debugCamera_.Initialize(camera_.rotation_, camera_.translation_);

	// プレイヤー
	// ------------------------------------------
	modelPlayer_ = std::unique_ptr<Model>(Model::CreateFromOBJ("player_body", ShaderModel::kHalfLambert));
	player_ = std::make_unique<Player>();
	player_->Initialize(modelPlayer_.get(), &camera_);

	// 当たり判定
	collisionManager_ = std::make_unique<CollisionManager>();

	// 環境
	// ------------------------------------------
	modelSkydome_ = std::unique_ptr<Model>(Model::CreateFromOBJ("sky_sphere"));
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(modelSkydome_.get(), &camera_);
	modelTerrain_ = std::unique_ptr<Model>(Model::CreateFromOBJ("terrain"));
	modelTerrain_->SetColor({ 0.4f, 0.4f, 0.4f, 1.0f });
	terrain_ = std::make_unique<Terrain>();
	terrain_->Initialize(modelTerrain_.get(), &camera_);


	// 調整項目を登録
	RegisterAllVariables();
}

void GameScene::Update() {
	// カメラ更新
	UpdateCamera();
	
	// 調整項目を適応
	ApplyAllVariables();

	// 当たり判定更新
	CheckAllCollisions();

	// --- プレイヤー ---
	player_->Update();

	// --- 環境 ---
	skydome_->Update();
	terrain_->Update();
}

void GameScene::Draw() {
	Model::PreDraw();
	player_->Draw();

	skydome_->Draw();
	terrain_->Draw();

	AxisIndicator::GetInstance()->Draw();
	Model::PostDraw();

	Sprite::PreDraw();
	Sprite::PostDraw();
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
		camera_.UpdateMatrix();
	}
}

void GameScene::ApplyAllVariables() {
	Player::ApplyGlobalVariables();
}

void GameScene::RegisterAllVariables() {
	Player::RegisterGlobalVariables();
}

void GameScene::CheckAllCollisions() {
	collisionManager_->Clear();

	// コライダーをリストに登録
	collisionManager_->AddCollider(player_.get());

	collisionManager_->Update();
}

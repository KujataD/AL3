#include "GameScene.h"

using namespace KujakuEngine;
using namespace ShapeUtil;

GameScene::~GameScene() {}

void GameScene::Initialize() {

	// 調整項目を登録
	RegisterAllVariables();
	ApplyAllVariables();

	// テクスチャ
	// ------------------------------------------
	TextureManager::GetInstance()->LoadTexture("Resources/reticle.png");

	// カメラ
	// ------------------------------------------
	camera_.Initialize();
	camera_.translation_ = { 0.0f, 5.0f, -20.0f };
	debugCamera_.Initialize(camera_.rotation_, camera_.translation_);
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();

	// プレイヤー
	// ------------------------------------------
	modelFighterHead_ = std::unique_ptr<Model>(Model::CreateFromOBJ("player_head", ShaderModel::kHalfLambert));
	modelFighterBody_ = std::unique_ptr<Model>(Model::CreateFromOBJ("player_body", ShaderModel::kHalfLambert));
	modelFighterArm_L_ = std::unique_ptr<Model>(Model::CreateFromOBJ("player_arm", ShaderModel::kHalfLambert));
	modelFighterArm_R_ = std::unique_ptr<Model>(Model::CreateFromOBJ("player_arm", ShaderModel::kHalfLambert));
	player_ = std::make_unique<Player>();
	player_->Initialize(modelFighterHead_.get(), modelFighterBody_.get(), modelFighterArm_L_.get(), modelFighterArm_R_.get(), &camera_);
	followCamera_->SetTarget(player_->GetWorldTransform());
	player_->SetViewProjection(&followCamera_->GetCamera());

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


}

void GameScene::Update() {
	// 調整項目を適応
	ApplyAllVariables();

	// --- プレイヤー ---
	player_->Update();

	// カメラ更新
	UpdateCamera();

	// --- 環境 ---
	skydome_->Update();
	terrain_->Update();

	// カメラ確定後の行列でプレイヤーのWVPを更新
	player_->SetCamera(&camera_);

	// 当たり判定更新
	CheckAllCollisions();
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
		camera_.rotation_ = debugCamera_.rotation_;
		camera_.translation_ = debugCamera_.translation_;
		camera_.matView = debugCamera_.GetViewMatrix();
		camera_.UpdateProjectionMatrix();
		camera_.TransferConstBuffer();

	} else {
		followCamera_->Update();
		const Camera& followCamera = followCamera_->GetCamera();
		camera_.rotation_ = followCamera.rotation_;
		camera_.translation_ = followCamera.translation_;
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

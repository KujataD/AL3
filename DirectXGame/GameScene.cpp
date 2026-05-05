#include "GameScene.h"

using namespace KujakuEngine;
using namespace ShapeUtil;

GameScene::~GameScene() {}

void GameScene::Initialize() {

	// カメラ
	// ------------------------------------------
	camera_.Initialize();
	camera_.translation_ = {0.0f, 0.0f, -100.0f};
	debugCamera_.Initialize(camera_.rotation_, camera_.translation_);

	// プレイヤー
	// ------------------------------------------
	modelPlayer_ = std::unique_ptr<Model>(Model::CreateFromOBJ("airship", ShaderModel::kHalfLambert));
	modelBullet_ = std::unique_ptr<Model>(Model::CreateFromOBJ("player_bullet"));
	player_ = std::make_unique<Player>();
	player_->Initialize(modelPlayer_.get(), modelBullet_.get(), &camera_);

	// エネミー
	// ------------------------------------------
	modelEnemy_ = std::unique_ptr<Model>(Model::CreateFromOBJ("enemy_ship", ShaderModel::kHalfLambert));
	modelEnemyBullet_ = std::unique_ptr<Model>(Model::CreateFromOBJ("enemy_bullet"));
	enemy_ = std::make_unique<Enemy>();
	enemy_->SetPlayer(player_.get());
	enemy_->Initialize(modelEnemy_.get(), modelEnemyBullet_.get(), &camera_, {20.0f, 0.0f, 200.0f});

	// 当たり判定
	collisionManager_ = std::make_unique<CollisionManager>();

	// スカイドーム
	// ------------------------------------------
	modelSkydome_ = std::unique_ptr<Model>(Model::CreateFromOBJ("sky_sphere"));
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(modelSkydome_.get(), &camera_);

	// 調整項目を登録
	RegisterAllVariables();
}

void GameScene::Update() {
	// 調整項目を適応
	ApplyAllVariables();

	// 当たり判定更新
	CheckAllCollisions();

	UpdateCamera();
	player_->Update();
	enemy_->Update();
	skydome_->Update();
}

void GameScene::Draw() {
	Model::PreDraw();
	player_->Draw();
	enemy_->Draw();
	skydome_->Draw();
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

void GameScene::CheckAllCollisions() {
	collisionManager_->Clear();

	// 自弾リスト取得
	const std::list<std::unique_ptr<PlayerBullet>>& playerBullets = player_->GetBullets();
	// 敵弾リスト取得
	const std::list<std::unique_ptr<EnemyBullet>>& enemyBullets = enemy_->GetBullets();

	// コライダーをリストに登録
	collisionManager_->AddCollider(player_.get());
	collisionManager_->AddCollider(enemy_.get());

	for (auto& enemyBullet : enemyBullets) {
		collisionManager_->AddCollider(enemyBullet.get());
	}

	for (auto& playerBullet : playerBullets) {
		collisionManager_->AddCollider(playerBullet.get());
	}

	collisionManager_->Update();
}

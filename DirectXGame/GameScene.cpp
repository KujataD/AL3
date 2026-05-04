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
	modelBullet_ = std::unique_ptr<Model>(Model::CreateCube("resources/white1x1.png"));
	player_ = std::make_unique<Player>();
	player_->Initialize(modelPlayer_.get(), modelBullet_.get(), &camera_);

	// エネミー
	// ------------------------------------------
	modelEnemy_ = std::unique_ptr<Model>(Model::CreateFromOBJ("enemy_airship", ShaderModel::kHalfLambert));
	modelEnemyBullet_ = std::unique_ptr<Model>(Model::CreateCube("resources/white1x1.png"));
	enemy_ = std::make_unique<Enemy>();
	enemy_->SetPlayer(player_.get());
	enemy_->Initialize(modelEnemy_.get(), modelEnemyBullet_.get(), &camera_, {20.0f, 0.0f, 200.0f});

	RegisterAllVariables();
}

void GameScene::Update() {
	ApplyAllVariables();
	CheckAllCollisions();

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

void GameScene::CheckAllCollisions() {
	Sphere sphereA, sphereB;
	sphereA.radius = 1.0f;
	sphereB.radius = 1.0f;

	// 自弾リスト取得
	const std::list<std::unique_ptr<PlayerBullet>>& playerBullets = player_->GetBullets();
	// 敵弾リスト取得
	const std::list<std::unique_ptr<EnemyBullet>>& enemyBullets = enemy_->GetBullets();

#pragma region 自キャラと敵弾の当たり判定
	sphereA.center = player_->GetWorldPosition();
	for (auto& enemyBullet : enemyBullets) {
		CheckCollisionPair(player_.get(), enemyBullet.get());
	}
#pragma endregion

#pragma region 自弾と敵キャラの当たり判定
	sphereA.center = enemy_->GetWorldPosition();
	for (auto& playerBullet : playerBullets) {
		CheckCollisionPair(playerBullet.get(), enemy_.get());
	}
#pragma endregion

#pragma region 自弾と敵弾の当たり判定
	for (auto& playerBullet : playerBullets) {
		for (auto& enemyBullet : enemyBullets) {
			CheckCollisionPair(playerBullet.get(), enemyBullet.get());
		}
	}
#pragma endregion
}

void GameScene::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {
	if (IsCollision(colliderA->GetSphere(), colliderB->GetSphere())) {
		colliderA->OnCollision();
		colliderB->OnCollision();
	}
}

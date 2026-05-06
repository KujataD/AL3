#include "GameScene.h"

using namespace KujakuEngine;
using namespace ShapeUtil;

GameScene::~GameScene() {}

void GameScene::Initialize() {

	// カメラ
	// ------------------------------------------
	camera_.Initialize();
	camera_.translation_ = {0.0f, 0.0f, -50.0f};
	debugCamera_.Initialize(camera_.rotation_, camera_.translation_);
	railCamera_.Initialize(camera_.rotation_, camera_.translation_);

	// プレイヤー
	// ------------------------------------------
	modelPlayer_ = std::unique_ptr<Model>(Model::CreateFromOBJ("airship", ShaderModel::kHalfLambert));
	modelBullet_ = std::unique_ptr<Model>(Model::CreateFromOBJ("player_bullet"));
	player_ = std::make_unique<Player>();
	player_->Initialize(modelPlayer_.get(), modelBullet_.get(), &camera_);
	player_->SetParent(railCamera_.GetWorldTransform());

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

	// スプライン曲線
	modelSpline_ = std::unique_ptr<ParticleModel>(ParticleModel::CreatePlane("resources/white1x1.png", false));

	modelSphere_ = std::unique_ptr<Model>(Model::CreateSphere("resources/white1x1.png"));
	modelSphere_->SetColor({0.0f, 0.0f, 1.0f, 1.0f});

	modelCube_ = std::unique_ptr<Model>(Model::CreateCube("resources/white1x1.png"));
	modelCube_->SetColor({1.0f, 0.0f, 0.0f, 1.0f});

	railFirstCube_.Initialize();
	railFirstCube_.scale_ = {0.5f, 0.5f, 3.0f};

	railSecondSphere_.Initialize();
	railSecondSphere_.scale_ = {0.1f, 0.1f, 0.1f};

	railControlPoints_ = {
	    {0,  0,  0},
        {10, 10, 0},
        {10, 15, 0},
        {20, 15, 0},
        {20, 10,  0},
        {30, 5,  0},
	};

	// 調整項目を登録
	RegisterAllVariables();
}

void GameScene::Update() {
	// 調整項目を適応
	ApplyAllVariables();

	// 当たり判定更新
	CheckAllCollisions();

	player_->Update();
	enemy_->Update();
	skydome_->Update();

	timer_ -= kDT;
	float t = std::clamp((1.0f - timer_ / 10.0f), 0.0f, 0.99f);
	float t1 = std::clamp((1.0f - timer_ / 10.0f) + 0.05f, 0.0f, 1.0f);
	railFirstCube_.translation_ = CatmullRomPosition(railControlPoints_, t);
	railSecondSphere_.translation_ = CatmullRomPosition(railControlPoints_, t1);

	//railFirstCube_.CalcRotationOfVelocity(railSecondSphere_.GetWorldPosition() - railFirstCube_.GetWorldPosition());
	
	//railFirstCube_.rotation_ = CalcRotationOfVelocity(railSecondSphere_.GetWorldPosition() - railFirstCube_.GetWorldPosition());
	Vector3 nextRotation = CalcRotationOfVelocity(railSecondSphere_.GetWorldPosition() - railFirstCube_.GetWorldPosition());
	railFirstCube_.rotation_ = nextRotation;
	//railFirstCube_.rotation_.y = nextRotation.y;
	//if (abs(railFirstCube_.rotation_.y - nextRotation.y) < std::numbers::pi_v<float>)  {
		//railFirstCube_.rotation_.y = nextRotation.y;
	//}

	railFirstCube_.UpdateMatrix(camera_);
	railSecondSphere_.UpdateMatrix(camera_);
	UpdateCamera();
}

void GameScene::Draw() {
	Model::PreDraw();
	player_->Draw();
	enemy_->Draw();
	skydome_->Draw();

	modelCube_->Draw(railFirstCube_, camera_, kFillModeWireframe);
	modelSphere_->Draw(railSecondSphere_, camera_, kFillModeWireframe);
	ParticleModel::PreDraw();
	DrawSplineParticles(modelSpline_.get(), railControlPoints_, camera_);
	ParticleModel::PostDraw();
}

void GameScene::UpdateCamera() {
#ifdef _DEBUG
	if (Input::GetKeyTrigger(DIK_P)) {
		isActiveDebugCamera_ = !isActiveDebugCamera_;
	}
	if (Input::GetKeyTrigger(DIK_L)) {
		Initialize();
	}
	if (Input::GetKeyTrigger(DIK_RETURN)) {
		timer_ = 10.0f;
	}

#endif // _DEBUG

	// カメラコントローラー操作
	railCamera_.SetPosition(railFirstCube_.translation_);
	railCamera_.SetRotation(railFirstCube_.rotation_);
	railCamera_.Update();

	// カメラの処理
	if (isActiveDebugCamera_) {
		debugCamera_.Update();
		debugCamera_.UpdateViewMatrix();
		camera_.matView = debugCamera_.GetViewMatrix();
		camera_.UpdateProjectionMatrix();
		camera_.TransferConstBuffer();

	} else {
		camera_.matView = railCamera_.GetViewMatrix();
		camera_.UpdateProjectionMatrix();
		camera_.TransferConstBuffer();
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

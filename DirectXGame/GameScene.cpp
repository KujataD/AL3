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
	camera_.translation_ = { 0.0f, 0.0f, -50.0f };
	debugCamera_.Initialize(camera_.rotation_, camera_.translation_);

	// プレイヤー
	// ------------------------------------------
	modelPlayer_ = std::unique_ptr<Model>(Model::CreateFromOBJ("airship", ShaderModel::kHalfLambert));
	modelBullet_ = std::unique_ptr<Model>(Model::CreateFromOBJ("player_bullet"));
	player_ = std::make_unique<Player>();
	player_->Initialize(modelPlayer_.get(), modelBullet_.get(), &camera_);

	// 敵弾
	// ------------------------------------------
	modelEnemyBullet_ = std::unique_ptr<Model>(Model::CreateFromOBJ("enemy_bullet"));

	// エネミー
	// ------------------------------------------
	modelEnemy_ = std::unique_ptr<Model>(Model::CreateFromOBJ("enemy_ship", ShaderModel::kHalfLambert));
	LoadEnemyPopData();

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

	// --- 敵 ---
	UpdateEnemyPopCommands();
	UpdateEnemies();
	UpdateEnemyBullets();

	// --- 環境 ---
	skydome_->Update();
	terrain_->Update();
}

void GameScene::Draw() {
	Model::PreDraw();
	player_->Draw();

	for (auto& enemy : enemies_) {
		enemy->Draw();
	}

	// 敵弾の描画
	for (auto& bullet : enemyBullets_) {
		bullet->Draw();
	}
	skydome_->Draw();
	terrain_->Draw();

	AxisIndicator::GetInstance()->Draw();
	Model::PostDraw();

	Sprite::PreDraw();
	player_->DrawUI();
	Sprite::PostDraw();
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
	const std::list<std::unique_ptr<EnemyBullet>>& enemyBullets = enemyBullets_;

	// コライダーをリストに登録
	collisionManager_->AddCollider(player_.get());

	for (auto& enemy : enemies_) {
		collisionManager_->AddCollider(enemy.get());
	}

	for (auto& enemyBullet : enemyBullets) {
		collisionManager_->AddCollider(enemyBullet.get());
	}

	for (auto& playerBullet : playerBullets) {
		collisionManager_->AddCollider(playerBullet.get());
	}

	collisionManager_->Update();
}

void GameScene::UpdateEnemyBullets() {

	// 更新
	for (auto& bullet : enemyBullets_) {
		bullet->Update();
	}

	// デスフラグの立った弾を排除
	enemyBullets_.remove_if([](std::unique_ptr<EnemyBullet>& bullet) {
		if (bullet->IsDead()) {
			return true;
		}
		return false;
		});
}

void GameScene::UpdateEnemies() {
	// 更新
	for (auto& bullet : enemies_) {
		bullet->Update();
	}

	// デスフラグの立った弾を排除
	enemies_.remove_if([](std::unique_ptr<Enemy>& bullet) {
		if (bullet->IsDead()) {
			return true;
		}
		return false;
		});

}

void GameScene::SpawnEnemy(KujakuEngine::Vector3 spawnPos, Enemy::LeaveState leaveState) {
	std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
	enemy->SetPlayer(player_.get());
	enemy->SetGameScene(this);
	enemy->Initialize(modelEnemy_.get(), modelEnemyBullet_.get(), &camera_, spawnPos);
	enemy->SetLeaveState(leaveState);
	enemies_.push_back(std::move(enemy));
}

void GameScene::AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBullet) {
	enemyBullets_.push_back(std::move(enemyBullet));
}

void GameScene::LoadEnemyPopData() {
	// ファイルを開く
	std::ifstream file;
	file.open("Resources/spawnData/enemyPop.csv");
	assert(file.is_open());

	// ファイルの内容を文字列ストリームにコピー
	enemyPopCommands_ << file.rdbuf();

	// ファイルを閉じる
	file.close();
}

void GameScene::UpdateEnemyPopCommands() {

	//待機処理
	if (isEnemyWaitingForPop_) {

		enemyWaitTimer_--;
		if (enemyWaitTimer_ <= 0) {
			//待機完了
			isEnemyWaitingForPop_ = false;
		}
		return;
	}

	//1行分の文字列を入れる変数
	std::string line;

	// コマンド実行ループ
	while (getline(enemyPopCommands_, line)) {
		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		//,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			// コメント行を飛ばす
			continue;
		}

		// POPコマンド
		if (word.find("POP") == 0) {
			// x座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			// y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			// z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			// 離脱コマンド
			Enemy::LeaveState leaveState = Enemy::LeaveState::Left; // デフォルト左
			getline(line_stream, word, ',');
			for (int32_t i = 0; i < (int32_t)Enemy::LeaveState::Count; i++) {
				if (word.find(Enemy::CommandList::kSetLeave[i]) == 0) {	
					// コマンドに一致する処理を追加
					leaveState = static_cast<Enemy::LeaveState>(i);
					break;
				}
			}

			// 敵を発生させる
			SpawnEnemy(Vector3(x, y, z), leaveState);
		} else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			//待ち時間
			int32_t waitTime = atoi(word.c_str());

			// 待機開始
			isEnemyWaitingForPop_ = true;
			enemyWaitTimer_ = waitTime;

			// 次のコマンドは待機終了後に実行するため、ループを抜ける
			break;
		}
	}
}
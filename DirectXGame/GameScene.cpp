#include "GameScene.h"
#include "GuardEffect.h"
#include "HitEffect.h"
#include "StageManager.h"

using namespace KujakuEngine;
using namespace ShapeUtil;

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete modelPlayer_;
	delete modelPlayerAttack_;
	delete modelEnemy_;
	delete modelSkydome_;
	delete modelHitEffect_;
	delete modelGuardEffect_;
	delete blockModel_;
	delete player_;
	delete mapChipField_;
	delete deathParticles_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	for (BaseEnemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();

	for (HitEffect* hitEffect : hitEffects_) {
		delete hitEffect;
	}
	hitEffects_.clear();

	for (GuardEffect* guardEffect : guardEffects_) {
		delete guardEffect;
	}
	guardEffects_.clear();
}

void GameScene::Init(StageManager* stageDataManager) {
	// 引数をメンバに記録
	stageManager_ = stageDataManager;

	// プレイヤー
	// ------------------------------------------

	// 3Dモデルの生成
	modelPlayer_ = Model::CreateFromOBJ("player");
	modelPlayerAttack_ = Model::CreateFromOBJ("hit_effect");

	// ------------------------------------------
	// エネミー

	// 3Dモデルの生成
	modelEnemy_ = Model::CreateFromOBJ("enemy");

	// 3Dモデルの生成
	modelShieldEnemy_ = Model::CreateFromOBJ("shieldEnemy");

	// マップチップ
	// ------------------------------------------
	blockModel_ = Model::CreateFromOBJ("block");
	mapChipField_ = new MapChipField;

	// 現在のステージデータを取得する
	const StageData& stageData = stageManager_->GetCurrentStageData();
	// ステージファイルパスの生成
	std::string stageFileName = "Resources/fields/" + stageData.name + ".csv";
	// ステージファイルの読み込み
	mapChipField_->LoadMapChipCsv(stageFileName);

	GenerateFieldObjects();

	// スカイドーム
	// ------------------------------------------

	// フェーズ
	// ------------------------------------------
	phase_ = Phase::kFadeIn;

	// フェード
	// ------------------------------------------
	fade_ = new Fade;
	fade_->Init();
	fade_->Start(Fade::Status::FadeIn, kFadeDuration);

	// スカイドーム
	// ------------------------------------------
	modelSkydome_ = Model::CreateFromOBJ("sky_sphere");
	skydome_ = new Skydome();
	skydome_->Init(modelSkydome_, &camera_);

	// カメラ初期化
	// ------------------------------------------
	camera_.Initialize();
	camera_.translation_ = {15.0f, 9.0f, -40.0f};

	debugCamera_.Initialize(camera_.rotation_, camera_.translation_);

	// カメラコントローラーの初期化
	cameraController_ = new CameraController;
	cameraController_->Init(&camera_);
	cameraController_->SetTarget(player_);
	cameraController_->Reset();

	// VFX
	// ------------------------------------------

	// デスパーティクル
	modelDeathParticle_ = Model::CreateFromOBJ("deathParticle");

	// ヒットエフェクト
	modelHitEffect_ = Model::CreateFromOBJ("particle");
	HitEffect::SetModel(modelHitEffect_);
	HitEffect::SetCamera(&camera_);

	// ガードエフェクト
	modelGuardEffect_ = Model::CreateFromOBJ("ring");
	GuardEffect::SetModel(modelGuardEffect_);
	GuardEffect::SetCamera(&camera_);
}

void GameScene::Update() {

	// フェーズ切り替え処理
	ChangePhase();

	// フェードの更新
	fade_->Update();

	switch (phase_) {
	case GameScene::Phase::kFadeIn: {

		// カメラコントローラーの処理
		cameraController_->Update();

		// skydomeの処理
		skydome_->Update();

		// 自キャラの更新
		player_->Update();

		// エネミーの更新
		UpdateEnemies();

		// ブロックの更新
		UpdateBlocks();
	} break;
	case GameScene::Phase::kPlay: {
		// ゲームプレイフェーズ
		// --------------------------------------

		// カメラコントローラーの処理
		cameraController_->Update();

		// skydomeの処理
		skydome_->Update();

		// 自キャラの更新
		player_->Update();

		// エネミーの更新
		UpdateEnemies();

		// ブロックの更新
		UpdateBlocks();

		// 当たり判定処理
		CheckAkkCollisions();

		// パーティクル処理
		UpdateParticles();

	} break;
	case GameScene::Phase::kDeath:
		// デス演出フェーズ
		// --------------------------------------

		// skydomeの処理
		skydome_->Update();

		// パーティクル処理
		UpdateParticles();

		// エネミーの更新
		UpdateEnemies();

		// ブロックの更新
		UpdateBlocks();

		if (deathParticles_) {
			deathParticles_->Update();
		}
		break;

	default:
		break;
	}

#ifdef _DEBUG
	if (Input::GetKeyTrigger(DIK_P)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}

#endif // _DEBUG

#ifdef USE_IMGUI

	// ------------------------------------------
	// デバッグテキストの表示
	ImGui::Begin("Debug1");

	// 初期化
	if (ImGui::Button("Reoad")) {
		reloadRequested_ = true;
	}
	ImGui::End();

#endif // USE_IMGUI

	// カメラの処理
	if (isDebugCameraActive_) {
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

void GameScene::Draw() {
	// ------------------------------------------
	// 3Dモデル描画前処理
	Model::PreDraw();

	// skydomeの描画
	skydome_->Draw();

	// ブロックの描画
	for (std ::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			blockModel_->Draw(*worldTransformBlock, camera_);
		}
	}

	// エネミーの描画
	for (BaseEnemy* enemy : enemies_) {
		enemy->Draw();
	}

	// 自キャラの描画
	player_->Draw();

	// パーティクル描画
	if (deathParticles_) {
		deathParticles_->Draw();
	}

	// ヒットエフェクト描画
	for (HitEffect* hitEffect : hitEffects_) {
		hitEffect->Draw();
	}

	// ガードエフェクト描画
	for (GuardEffect* guardEffect : guardEffects_) {
		guardEffect->Draw();
	}

	// 3Dモデル描画後処理
	Model::PostDraw();

	// フェード
	fade_->Draw();
}

void GameScene::GenerateFieldObjects() {
	uint32_t numBlockVertical = mapChipField_->GetNumBlockVertical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素数を変更する
	// 列数を設定(縦方向のブロック数)
	worldTransformBlocks_.resize(numBlockVertical);
	for (uint32_t i = 0; i < numBlockVertical; ++i) {
		// 1列の要素数を設定(横方向のブロック数)
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	// ブロックの生成
	for (uint32_t y = 0; y < numBlockVertical; ++y) {
		for (uint32_t x = 0; x < numBlockHorizontal; ++x) {
			switch (mapChipField_->GetMapChipTypeByIndex(x, y)) {
			case MapChipType::kBlock: {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[y][x] = worldTransform;
				worldTransformBlocks_[y][x]->translation_ = mapChipField_->GetMapChipPositionByIndex(x, y);
				break;
			}
			case MapChipType::kPlayer: {
				assert(player_ == nullptr && "自キャラを二重に配置しようとしています。");
				// 自キャラの作成

				// 自キャラの生成・初期化
				player_ = new Player();

				// 座標を指定
				Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(x, y);

				player_->Init(modelPlayer_, modelPlayerAttack_, &camera_, playerPosition);
				player_->SetMapChipField(mapChipField_);
				break;
			}
			case MapChipType::kEnemy: {
				uint8_t subID = mapChipField_->GetMapChipSubIDByIndex(x, y);
				switch (subID) {
				case 0: {

					Enemy* newEnemy = new Enemy();
					Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(x, y);
					newEnemy->Init(modelEnemy_, &camera_, enemyPosition);
					newEnemy->SetMapChipField(mapChipField_);
					newEnemy->SetGameScene(this);
					enemies_.push_back(newEnemy);
					break;
				}
				case 1: {

					ShieldEnemy* newEnemy = new ShieldEnemy();
					Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(x, y);
					newEnemy->Init(modelShieldEnemy_, &camera_, enemyPosition);
					newEnemy->SetMapChipField(mapChipField_);
					newEnemy->SetGameScene(this);
					enemies_.push_back(newEnemy);
					break;
				}
				default: {
					break;
				}
				}
				break;
			}
			default: {
				break;
			}
			}
		}
	}
}

void GameScene::CreateHitEffect(const KujakuEngine::Vector3 spawnPos, const KujakuEngine::Vector4 color) {
	HitEffect* newHitEffect = HitEffect::Create(spawnPos, color);
	hitEffects_.push_back(newHitEffect);
}

void GameScene::CreateGuardEffect(const KujakuEngine::Vector3 spawnPos) {
	GuardEffect* newGuardEffect = GuardEffect::Create(spawnPos);
	guardEffects_.push_back(newGuardEffect);
}

void GameScene::CheckAkkCollisions() {
#pragma region 自キャラと敵キャラの当たり判定
	{
		// 判定対象1と2の座標
		AABB aabb1, aabb2;

		// 自キャラの座標
		aabb1 = player_->GetAABB();

		// 自キャラと敵弾全ての当たり判定
		for (BaseEnemy* enemy : enemies_) {
			if (enemy->IsCollisionDisabled()) {
				continue;
			}

			// 敵弾の座標
			aabb2 = enemy->GetAABB();

			// AABB同士の交差判定
			if (IsCollision(aabb1, aabb2)) {
				enemy->OnCollision(player_);
				player_->OnCollision(enemy);
			}
		}
	}
#pragma endregion
}

void GameScene::ChangePhase() {
	switch (phase_) {
	case GameScene::Phase::kFadeIn: // フェードイン
		if (fade_->IsFinished()) {
			phase_ = GameScene::Phase::kPlay;
		}
		break;
	case GameScene::Phase::kPlay: // ゲームプレイ

		if (player_->IsDead()) {
			phase_ = Phase::kDeath;
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();

			deathParticles_ = new DeathParticles;
			deathParticles_->Init(modelDeathParticle_, &camera_, deathParticlesPosition);
		}

		break;
	case GameScene::Phase::kDeath: // デス演出フェーズ

		// デス演出フェーズ終了
		if (deathParticles_ && deathParticles_->IsFinished()) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, kFadeDuration);
		}
		break;
	case GameScene::Phase::kFadeOut:
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	default:
		break;
	}
}

void GameScene::UpdateBlocks() {
	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			// アフィン変換行列の作成
			Matrix4x4 affine = Matrix4x4::MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

			worldTransformBlock->matWorld_ = affine;

			// 定数バッファに転送する
			worldTransformBlock->TransferMatrix(camera_);
		}
	}
}

void GameScene::UpdateEnemies() { // エネミーの更新
	for (BaseEnemy* enemy : enemies_) {
		enemy->Update();
	}

	// デスフラグの立った敵を削除
	enemies_.remove_if([](BaseEnemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});
}

void GameScene::UpdateParticles() {
	// パーティクル処理

	for (HitEffect* hitEffect : hitEffects_) {
		hitEffect->Update();
	}

	// デスフラグの立ったヒットエフェクトを削除
	hitEffects_.remove_if([](HitEffect* effect) {
		if (effect->IsDead()) {
			delete effect;
			return true;
		}
		return false;
	});

	for (GuardEffect* hitEffect : guardEffects_) {
		hitEffect->Update();
	}

	// デスフラグの立ったヒットエフェクトを削除
	guardEffects_.remove_if([](GuardEffect* effect) {
		if (effect->IsDead()) {
			delete effect;
			return true;
		}
		return false;
	});
}

#include "GameScene.h"
using namespace KujakuEngine;

GameScene::GameScene() {}

GameScene::~GameScene() {}

void GameScene::Init() {

	// --- カメラ初期化 ---
	camera_.Initialize();
	camera_.translation_ = {15.0f, 9.0f, -40.0f};

	debugCamera_.Initialize(camera_.rotation_, camera_.translation_);

	// --- プレイヤー初期化 ---
	playerModel_ = Model::CreateFromOBJ("resources/player", "player.obj");
	player_ = new Player();
	player_->Init(playerModel_, &camera_);

	// --- マップチップ ---
	blockModel_ = Model::CreateFromOBJ("resources/block", "block.obj");
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	GenerateBlocks();

	// --- スカイドーム ---
	modelSkydome_ = Model::CreateFromOBJ("resources/sky_sphere", "sky_sphere.obj");
	skydome_ = new Skydome();
	skydome_->Init(modelSkydome_, &camera_);
}

void GameScene::Update() {

	// プレイヤー更新
	player_->Update();

	// スカイドーム更新
	skydome_->Update();
	
	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			// アフィン変換行列の作成
			Matrix4x4 affine = Matrix4x4::MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

			worldTransformBlock->matWorld_ = affine;

			// 定数バッファに転送する
			worldTransformBlock->UpdateMatrix(camera_);
		}
	}

#ifdef _DEBUG
	if (Input::GetKey(DIK_P)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif // _DEBUG
	
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
	Model::PreDraw();

	// プレイヤー描画
	player_->Draw();

	// スカイドーム描画
	skydome_->Draw();

	// ブロック描画
	for (std ::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			blockModel_->Draw(*worldTransformBlock, camera_);
		}
	}
}

void GameScene::GenerateBlocks() {

	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素数を変更する
	// 列数を設定(縦方向のブロック数)
	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		// 1列の要素数を設定(横方向のブロック数)
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	// ブロックの生成
	for (uint32_t y = 0; y < numBlockVirtical; ++y) {
		for (uint32_t x = 0; x < numBlockHorizontal; ++x) {
			if (mapChipField_->GetMapChipTypeByIndex(x, y) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[y][x] = worldTransform;
				worldTransformBlocks_[y][x]->translation_ = mapChipField_->GetMapChipPositionByIndex(x, y);
			}
		}
	}
}

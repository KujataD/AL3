#include "GameScene.h"
using namespace KujakuEngine;

GameScene::GameScene() {}

GameScene::~GameScene() { delete sprite_; }

void GameScene::Init() {
	// --- モデル初期化 ---
	model_ = Model::Create("resources/uvChecker.png");
	modelWorldTransform_.Initialize();

	// --- カメラ初期化 ---
	camera_.Initialize();
	camera_.translation_ = {0.0f, 0.0f, -5.0f};

	debugCamera_.Initialize(camera_.rotation_, camera_.translation_);
}

void GameScene::Update() {

	// モデル回転
	modelWorldTransform_.rotation_.y += 0.01f;
	modelWorldTransform_.UpdateMatrix(camera_);
	
	// カメラ更新
	debugCamera_.Update();
	debugCamera_.UpdateViewMatrix();
	camera_.matView = debugCamera_.GetViewMatrix();
	camera_.UpdateProjectionMatrix();
	camera_.TransferConstBuffer();
}

void GameScene::Draw() {

	Model::PreDraw();
	model_->Draw(modelWorldTransform_, camera_);
}

#include "GameScene.h"
using namespace KujakuEngine;

GameScene::GameScene() {}

GameScene::~GameScene() {  }

void GameScene::Init() {

	// --- カメラ初期化 ---
	camera_.Initialize();
	camera_.translation_ = {0.0f, 0.0f, -5.0f};

	debugCamera_.Initialize(camera_.rotation_, camera_.translation_);
	
	// --- プレイヤー初期化 ---
	model_ = Model::Create("resources/uvChecker.png");
	player_ = new Player();
	player_->Init(model_, &camera_);
}

void GameScene::Update() {
	
	// プレイヤー更新
	player_->Update();

	// カメラ更新
	debugCamera_.Update();
	debugCamera_.UpdateViewMatrix();
	camera_.matView = debugCamera_.GetViewMatrix();
	camera_.UpdateProjectionMatrix();
	camera_.TransferConstBuffer();
}

void GameScene::Draw() {
	Model::PreDraw();

	// プレイヤー描画
	player_->Draw(); 
}

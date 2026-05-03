#include "GameScene.h"

using namespace KujakuEngine;

GameScene::~GameScene() {
	delete player_;
	delete modelPlayer_;
}

void GameScene::Initialize() {

	// カメラ
	// ------------------------------------------
	camera_.Initialize();
	camera_.translation_ = {0.0f, 0.0f, -30.0f};

	// プレイヤー
	// ------------------------------------------
	modelPlayer_ = Model::CreateFromOBJ("airship");
	player_ = new Player;
	player_->Initialize(modelPlayer_, &camera_);
}

void GameScene::Update() {
	camera_.UpdateMatrix();
	player_->Update();
}

void GameScene::Draw() {
	Model::PreDraw();
	player_->Draw(); 
}

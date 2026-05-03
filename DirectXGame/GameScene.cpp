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

	// プレイヤー
	// ------------------------------------------
	modelPlayer_ = Model::CreateFromOBJ("player");
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

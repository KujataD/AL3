#include "GameScene.h"
using namespace KujakuEngine;

GameScene::GameScene() {}

GameScene::~GameScene() { delete sprite_; }

void GameScene::Init() {
	sprite_ = Sprite::Create("Resources/uvChecker.png", {0.0f, 0.0f});

	sprite_->SetPosition({0.0f, 0.0f});
	sprite_->SetUVScale({1.0f, 1.0f});
	sprite_->SetSize({0.5f, 0.5f});

	model_ = Model::Create("resources/uvChecker.png");
	modelPlayer_ = Model::CreateFromOBJ("resources/player", "player.obj");
	modelWorldTransform_.Initialize();
	playerWorldTransform_.Initialize();
	playerWorldTransform_.translation_ = {0.0f, -0.2f, 0.0f};
	modelWorldTransform_.translation_ = {2.0f, 1.0f, 0.0f};
	modelWorldTransform_.rotation_ = {-0.01f, 0.0f, 0.0f};

	camera_.Initialize();
	camera_.translation_ = {0.0f, 0.0f, -5.0f};
	camera_.UpdateMatrix();
}

void GameScene::Update() {
	modelWorldTransform_.rotation_.y += 0.01f;
	modelWorldTransform_.UpdateMatrix(camera_);
	playerWorldTransform_.rotation_.y -= 0.01f;
	playerWorldTransform_.UpdateMatrix(camera_);
}


void GameScene::Draw() {
	Sprite::PreDraw();
	sprite_->Draw();

	Model::PreDraw();
	model_->Draw(modelWorldTransform_, camera_);
	modelPlayer_->Draw(playerWorldTransform_, camera_);
}

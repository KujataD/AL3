#include "GameScene.h"
using namespace KujakuEngine;

GameScene::GameScene() {}

GameScene::~GameScene() { delete sprite_; }

void GameScene::Init() {
	sprite_ = Sprite::Create("Resources/uvChecker.png", {0.0f, 0.0f});

	sprite_->SetPosition({0.0f, 0.0f});
	sprite_->SetUVScale({1.0f, 1.0f});
}

void GameScene::Update() {}

void GameScene::Draw() {
	Sprite::PreDraw();
	sprite_->Draw();
}

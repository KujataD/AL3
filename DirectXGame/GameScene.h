#pragma once
#include "KujakuEngine/KujakuEngine.h"

class GameScene {
public:
	GameScene();
	~GameScene();

	// 初期化
	void Init();

	// 更新
	void Update();

	// 描画
	void Draw();

private:
	 KujakuEngine::Sprite* sprite_ = nullptr;
};

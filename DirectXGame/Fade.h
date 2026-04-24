#pragma once
#include "KujakuEngine/KujakuEngine.h"

class Fade {
public:
	enum class Status {
		None,		// フェードなし
		FadeIn,		// フェードイン中
		FadeOut,	// フェードアウト中
	};
public:

	Fade() = default;
	~Fade() = default;

	void Init();
	
	void Update();
	
	void Draw();

	void Start(Status status, float duration);

	void Stop();

	bool IsFinished() const;

private:
	// 黒色スプライト
	KujakuEngine::Sprite* sprite_ = nullptr;

	// 白板テクスチャハンドル
	uint32_t textureHandleWhite1x1 = 0;

	// 現在のフェードの状態
	Status status_ = Status::None;

	// フェードの持続時間
	float duration_ = 0.0f;

	// 経過時間カウンター
	float counter_ = 0.0f;

};

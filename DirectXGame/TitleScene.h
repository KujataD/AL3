#pragma once
#include "Fade.h"
#include "KujakuEngine/KujakuEngine.h"

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene {
public:
	enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン部
		kFadeOut, // フェードアウト
	};

public:
	TitleScene() = default;
	~TitleScene();

	void Init();

	void Update();

	void Draw();

	// Accessors

	// --- get ---

	bool IsFinished() const { return finished_; }

private: // 定数
	// --- タイトルフォント ---

	// アニメ－ション間隔（秒）
	static inline const float kTitleFontAnimDuration = 2.0f;

	// アニメーションの上下動幅（距離）
	static inline const float kTitileFontAnimVarticalRange = 0.5f;

	// フェード秒数
	static inline const float kFadeDuration = 1.0f;

private: // 変数
	// --- プレイヤー ---

	// モデル
	KujakuEngine::Model* playerModel_ = nullptr;

	// トランスフォーム
	KujakuEngine::WorldTransform playerWorldTransform_;

	// --- タイトルフォント ---

	// モデル
	KujakuEngine::Model* titleFontModel_ = nullptr;

	// トランスフォーム
	KujakuEngine::WorldTransform titleFontWorldTransform_;

	// アニメーションカウンター
	float titleFontAnimCounter_ = 0.0f;

	// アニメーション開始座標Y
	float titleFontAnimStartPosY_ = 0.0f;

	// --- カメラ ---

	KujakuEngine::Camera camera_;

	// --- シーン ---

	// 終了フラグ
	bool finished_ = false;

	// --- トランジション ---
	Fade* fade_ = nullptr;

	// フェーズ
	Phase phase_ = Phase::kFadeIn;
};

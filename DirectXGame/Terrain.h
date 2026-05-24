#pragma once
#include "KujakuEngine/KujakuEngine.h"

class Terrain {
public:
	Terrain() = default;
	~Terrain() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(KujakuEngine::Model* model, KujakuEngine::Camera* camera);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	// ワールド変換データ
	KujakuEngine::WorldTransform worldTransform_;

	// モデル
	KujakuEngine::Model* model_ = nullptr;

	// カメラ
	KujakuEngine::Camera* camera_ = nullptr;
};

#pragma once
#include <KujakuEngine.h>

/// <summary>
/// 自キャラの弾
/// </summary>
class PlayerBullet {
public:
	struct ParamKey {
		
	};

	struct Param {
		
	};

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model"></param>
	/// <param name="camera"></param>
	/// <param name="position"></param>
	void Initialize(KujakuEngine::Model* model, KujakuEngine::Camera* camera, const KujakuEngine::Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	static void RegisterGlobalVariant();
	static void ApplyGlobalVariant();

private:
	KujakuEngine::Model* model_;
	KujakuEngine::Camera* camera_;
	KujakuEngine::WorldTransform worldTransform_;
};

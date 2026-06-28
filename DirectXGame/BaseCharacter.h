#pragma once
#include <KujakuEngine.h>
#include <vector>

class BaseCharacter {
public:
	virtual ~BaseCharacter() = default;

	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize(const std::vector<KujakuEngine::Model*>& models, const KujakuEngine::Camera* camera);

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画関数
	/// </summary>
	virtual void Draw();

protected:
	std::vector<KujakuEngine::Model*> models_;
	const KujakuEngine::Camera* camera_ = nullptr;
	KujakuEngine::WorldTransform worldTransform_;
	
};


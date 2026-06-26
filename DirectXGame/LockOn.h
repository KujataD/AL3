#pragma once
#include <KujakuEngine.h>
#include <memory>
#include <list>

class Enemy;
class Player;

/// <summary>
/// ロックオンシステム
/// </summary>
class LockOn {
public:

	void Initialize();

	void Update(Player* player, std::list<Enemy*>& enemies, const KujakuEngine::Camera& camera);

	void Draw();

	// --- get ---
	Enemy* GetTarget() const { return target_; }

private:
	 std::unique_ptr<KujakuEngine::Sprite> spriteLockOn_ = nullptr;
	 Enemy* target_ = nullptr;

};

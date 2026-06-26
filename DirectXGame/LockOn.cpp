#include "LockOn.h"
#include "Enemy.h"
#include "Player.h"
using namespace KujakuEngine;

void LockOn::Initialize() {
	uint32_t textureIndexLockOn = TextureManager::GetInstance()->LoadTexture("Resources/lockOn.png");
	spriteLockOn_.reset(Sprite::Create(textureIndexLockOn, {0.0f, 0.0f}, 128.0f, 128.0f, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f}));
}

void LockOn::Update(Player* player, std::list<Enemy*>& enemies, const KujakuEngine::Camera& camera) {
	// 自機のワールドを取得する
	Vector3 playerWorldPos = player->GetWorldPosition();

	// ビュー座標に変換する
	Vector3 playerViewPos = Transform(playerWorldPos, camera.matView);

	// ターゲット
	std::list<std::pair<float, Enemy*>> targets;

	for (Enemy* enemy : enemies) {
		Vector3 positionWorld = enemy->GetWorldPosition();

		// ビュー座標系に変換する
		Vector3 enemyViewPos = Transform(positionWorld, camera.matView);
		if (enemyViewPos.z <= playerViewPos.z) {
			continue; // 自機より手前にいる場合は除外
		}

		// ワールド座標からスクリーン座標に変換
		Vector3 positionScreen = Project(positionWorld, 0.0f, 0.0f, static_cast<float>(WinApp::kWindowWidth), static_cast<float>(WinApp::kWindowHeight), camera.matView, camera.matProjection);

		// Vector2に格納
		Vector2 positionScreenV2(positionScreen.x, positionScreen.y);
		// スプライトの中心からの距離
		float distance = Distance(player->Get2DReticlePosition(), positionScreenV2);
		// ロックオン距離の限界値(スクリーン座標ベース)
		const float kDistanceLockOn = 200.0f;
		// 2Dレティクルからのスクリーン距離が規定範囲内ならば
		if (distance <= kDistanceLockOn) {
			targets.emplace_back(std::make_pair(distance, enemy));
		}
	}

	// ターゲットをNULLで更新
	target_ = nullptr;
	if (!targets.empty()) {
		// 距離の昇順ソート
		targets.sort();
		// 距離がいちばん小さい敵をロックオン対象とする。
		target_ = targets.front().second;
	}

	// ターゲティングされている敵がいたら
	if (target_) {
		// 敵のワールド座標を取得する
		Vector3 targetWorldPos = target_->GetWorldPosition();

		// ワールド座標からスクリーン座標に変換する
		Vector3 targetScreenPos = Project(targetWorldPos, 0.0f, 0.0f, static_cast<float>(WinApp::kWindowWidth), static_cast<float>(WinApp::kWindowHeight), camera.matView, camera.matProjection);

		// ロックオンマークの座標を設定する
		spriteLockOn_->SetPosition({targetScreenPos.x, targetScreenPos.y});
	}
}

void LockOn::Draw() {
	if (!target_) {
		return;
	}
	spriteLockOn_->Draw();
}

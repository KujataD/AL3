#pragma once
#include "IPlayerBehavior.h"
#include "KujakuEngine/KujakuEngine.h"
#include "LRDirection.h"
#include <memory>

class MapChipField;
class BaseEnemy;

/// <summary>
/// ビヘイビア変更をリクエストできるクラスをあらかじめ設定する
/// </summary>
class BehaviorChangeToken {
private:
	BehaviorChangeToken() = default;
	friend class Player;
	friend class PlayerBehaviorRoot;
	friend class PlayerBehaviorAttack;
	friend class PlayerBehaviorKnockback;
};

class Player {
public:
	// 列挙型・構造体
	// ------------------------------------------

	// 角
	enum Corner {
		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上

		kNumCorner // 要素数
	};

	// マップとの当たり判定情報
	struct CollisionMapInfo {
		bool isCeiling = false;           // 天井衝突フラグ
		bool isGround = false;            // 着地フラグ
		bool isWall = false;              // 壁接触フラグ
		KujakuEngine::Vector3 moveAmount; // 移動量
	};

public:
	Player();
	~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init(KujakuEngine::Model* model, KujakuEngine::Model* modelAttack, KujakuEngine::Camera* camera, const KujakuEngine::Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	// Accessor
	// ------------------------------------------

	// --- set ---

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }
	void SetActiveAttackFX(bool isActive) { isActiveAttackFX_ = isActive; }
	void DecreaseAttackCount(int value) { attackCounter_ -= value; }

	// --- get ---

	const KujakuEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const KujakuEngine::Vector3& GetVelocity() const { return velocity_; }
	bool IsDead() const { return isDead_; };
	bool CanAttack() const { return attackCoolTimer_ <= 0.0f && attackCounter_ > 0; }
	bool IsAttack() const { return behavior_->GetType() == IPlayerBehavior::BehaviorType::kAttack; }

	LRDirection GetLRDirection() const { return lrDirection_; }

	KujakuEngine::Vector3 GetWorldPosition() const;
	KujakuEngine::AABB GetAABB();

	// 外部API
	// ------------------------------------------
	void OnCollision(const BaseEnemy* enemy);

	/// <summary>
	/// ふるまいを変更します。
	/// </summary>
	/// <param name="newBehavior"></param>
	void RequestBehavior(IPlayerBehavior* newBehavior, BehaviorChangeToken token);

	/// <summary>
	/// ノックバックビヘイビアをリクエストします
	/// </summary>
	/// <param name="newBehavior"></param>
	void RequestKnockbackBehavior();

	/// <summary>
	/// 移動入力
	/// </summary>
	void Move();

	/// <summary>
	/// 移動入力
	/// </summary>
	void Knockback();

	/// <summary>
	/// 旋回制御
	/// </summary>
	void TurnControl();

	/// <summary>
	/// 物理的挙動の更新
	/// </summary>
	void UpdateCollision(const KujakuEngine::Vector3& move);

	/// <summary>
	/// ワールドトランスフォーム更新（state用API）
	/// </summary>
	void UpdateTransform() { worldTransform_.UpdateMatrix(*camera_); }

	/// <summary>
	/// ブリンク攻撃時の速度を取得
	/// </summary>
	/// <param name="speed"></param>
	float GetBlinkVelocityX();
	
	/// <summary>
	/// ブリンク攻撃時の速度を取得
	/// </summary>
	/// <param name="speed"></param>
	float GetKnockbackVelocityX();

	/// <summary>
	/// X速度停止 (API)
	/// </summary>
	void StopVelocityX() { velocity_.x = 0.0f; }

	/// <summary>
	/// 速度停止 (API)
	/// </summary>
	void StopVelocity() { velocity_ = KujakuEngine::Vector3(0.0f, 0.0f, 0.0f); }

	/// <summary>
	/// クールタイム開始(API)
	/// </summary>
	void StartAttackCoolTime() { attackCoolTimer_ = kAttackBlinkCoolTime; }

	void SetSizeEaseOut(const KujakuEngine::Vector3& startSize, const KujakuEngine::Vector3& endSize, float t);

	void InitSize() { worldTransform_.scale_ = kInitSize; }


private:
	/// <summary>
	/// ふるまいを変更します。
	/// </summary>
	/// <param name="newBehavior"></param>
	void ChangeBehavior(IPlayerBehavior* newBehavior);

	/// <summary>
	/// マップ衝突判定する
	/// </summary>
	void CheckMapCollision(CollisionMapInfo& info);

	/// <summary>
	/// 判定結果を反映して移動させる
	/// </summary>
	void ApplyCollision(const CollisionMapInfo& info);

	/// <summary>
	/// 天井に接触している場合の処理
	/// </summary>
	void OnCeiling(const CollisionMapInfo& info);

	/// <summary>
	/// 壁に接触している場合の処理
	/// </summary>
	void OnWall(const CollisionMapInfo& info);

	/// <summary>
	/// 接地状態の切り替え
	/// </summary>
	void UpdateGroundState(const CollisionMapInfo& info);

	/// <summary>
	/// マップ衝突判定上方向
	/// </summary>
	/// <param name="info"></param>
	void CheckMapCollisionUp(CollisionMapInfo& info);

	/// <summary>
	/// マップ衝突判定下方向
	/// </summary>
	/// <param name="info"></param>
	void CheckMapCollisionDown(CollisionMapInfo& info);

	/// <summary>
	/// マップ衝突判定右方向
	/// </summary>
	/// <param name="info"></param>
	void CheckMapCollisionRight(CollisionMapInfo& info);

	/// <summary>
	/// マップ衝突判定左方向
	/// </summary>
	/// <param name="info"></param>
	void CheckMapCollisionLeft(CollisionMapInfo& info);

	/// <summary>
	/// 指定した角の座標をとる
	/// </summary>
	/// <param name="center"></param>
	/// <param name="corner"></param>
	/// <returns></returns>
	KujakuEngine::Vector3 CornerPosition(const KujakuEngine::Vector3& center, Corner corner);

	/// <summary>
	/// 着地しているときの処理
	/// </summary>
	/// <param name="info"></param>
	void OnGround(const CollisionMapInfo& info);

	/// <summary>
	/// 通常行動更新
	/// </summary>
	void BehaviorRootUpdate();

public:
	// 定数
	// ------------------------------------------

	// 加速度
	static inline const float kAcceleration = 0.01f;

	// 摩擦
	static inline const float kAttenuation = 0.05f;

	// 着地時の速度減衰率
	static inline const float kAttenuationLanding = 0.3f;

	// 接壁時の速度減衰率
	static inline const float kAttenuationWall = 0.5f;

	// 最高ダッシュ速度
	static inline const float kLimitRunSpeed = 0.5f;

	// 旋回時間<秒>
	static inline const float kTimeTurn = 0.3f;

	// 重力加速度(下方向)
	static inline const float kGravityAcceleration = 0.025f;

	// 最大落下速度(下方向)
	static inline const float kLimitFallSpeed = 0.3f;

	// ジャンプ初速(上方向)
	static inline const float kJumpAcceleration = 0.5f;

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	// キャラクターの見た目サイズ
	static inline const KujakuEngine::Vector3 kInitSize = {1.0f, 1.0f, 1.0f};

	static inline const float kBlank = 0.01f;

	// 接地判定時のごくわずかな空白
	static inline const float kGroundCheckEpsilon = 0.02f;

	// --- アタック ---

	// ブリンクの溜め時間
	static inline const float kAttackBlinkPrepDuration = 0.1f;

	// ブリンクの持続時間
	static inline const float kAttackBlinkDashDuration = 0.1f;

	// ブリンクの余韻時間
	static inline const float kAttackBlinkAfterDuration = 0.1f;

	// ブリンクの移動速度
	static inline const float kAttackBlinkSpeed = 0.75f;

	// ブリンクのクールタイム
	static inline const float kAttackBlinkCoolTime = 0.2f;

	// ブリンク溜めサイズ
	static inline const KujakuEngine::Vector3 kAttackBlinkDashSize = {1.0f, 1.6f, 0.3f};
	static inline const KujakuEngine::Vector3 kAttackBlinkAfterSize = {1.0f, 0.7f, 1.3f};

	// 連続アタック回数
	static inline const int32_t kAttackMaxCount = 1;

	// --- ノックバック ---

	// ノックバックの移動速度
	static inline const float kKnockbackSpeed = 0.75f;
	
	// ノックバックの持続時間
	static inline const float kKnockbackDuration = 0.08f;
private:
	// 変数
	// ------------------------------------------

	// --- 外部参照 ---

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	// モデル
	KujakuEngine::Model* model_ = nullptr;

	// テクスチャモデル
	// uint32_t textureHandle_ = 0u;

	// カメラ
	KujakuEngine::Camera* camera_ = nullptr;

	KujakuEngine::Model* modelAttack_ = nullptr;
	KujakuEngine::WorldTransform worldTransformAttack_;

	// --- 内部プロパティ ---

	// ワールド変換データ
	KujakuEngine::WorldTransform worldTransform_;

	// 速度
	KujakuEngine::Vector3 velocity_ = {};

	// 左右
	LRDirection lrDirection_ = LRDirection::kRight;

	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;

	// 旋回タイマー
	float turnTimer_ = 0.0f;

	// 接地状態フラグ
	bool onGround_ = true;

	// デスフラグ
	bool isDead_ = false;

	// --- ステート ---

	std::unique_ptr<IPlayerBehavior> behavior_;

	// 次のビヘイビアを予約
	IPlayerBehavior* behaviorRequest_ = nullptr; 

	// --- アタック ---

	// アタッククールタイマー
	float attackCoolTimer_ = 0.0f;

	// アタックエフェクトのアクティブ状態
	bool isActiveAttackFX_ = false;

	// 連続アタック回数
	int32_t attackCounter_ = 0;
};

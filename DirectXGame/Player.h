#pragma once
#include "KujakuEngine/KujakuEngine.h"

class MapChipField;

class Player {
public:
	// 列挙型・構造体
	// ------------------------------------------

	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
	};

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
	Player() = default;
	~Player() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Init(KujakuEngine::Model* model, KujakuEngine::Camera* camera, const KujakuEngine::Vector3& position);

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

	// --- get ---

	const KujakuEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const KujakuEngine::Vector3& GetVelocity() const { return velocity_; }
	
private:
	/// <summary>
	/// 移動入力
	/// </summary>
	void Move();

	/// <summary>
	/// マップ衝突判定する
	/// </summary>
	void CheckMapCollision(CollisionMapInfo& info);

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
	void OnWall();

	/// <summary>
	/// 接地状態の切り替え
	/// </summary>
	void UpdateGroundState();

	/// <summary>
	/// 旋回制御
	/// </summary>
	void TurnControl();

	/// <summary>
	/// 行列計算
	/// </summary>
	void UpdateMatrix();

	/// <summary>
	/// カメラ内のクランプ
	/// </summary>
	void ClampInCameraVisible();

private:
	// 定数
	// ------------------------------------------

	// 加速度
	static inline const float kAcceleration = 0.01f;

	// 摩擦
	static inline const float kAttenuation = 0.05f;

	// 最高ダッシュ速度
	static inline const float kLimitRunSpeed = 0.1f;

	// 旋回時間<秒>
	static inline const float kTimeTurn = 0.3f;

	// 重力加速度(下方向)
	static inline const float kGravityAcceleration = 0.01f;

	// 最大落下速度(下方向)
	static inline const float kLimitFallSpeed = 0.2f;

	// ジャンプ初速(上方向)
	static inline const float kJumpAcceleration = 0.2f;

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	static inline const float kBlank = 0.1f;

	// 変数
	// ------------------------------------------

	// --- 外部参照 ---

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	// ワールド変換データ
	KujakuEngine::WorldTransform worldTransform_;

	// モデル
	KujakuEngine::Model* model_ = nullptr;

	// カメラ
	KujakuEngine::Camera* camera_ = nullptr;

	// --- 内部プロパティ ---

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
};

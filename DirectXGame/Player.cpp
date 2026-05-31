#include "Player.h"
#include <algorithm>
#include <numbers>

using namespace KujakuEngine;

Player::~Player() {}

void Player::Initialize(KujakuEngine::Model* model, KujakuEngine::Model* modelBullet, KujakuEngine::Camera* camera) {
	assert(model);
	assert(camera);

	// 各要素のセット
	camera_ = camera;
	model_ = model;
	modelBullet_ = modelBullet;

	// 各ワールドトランスフォームの初期化・設定
	// ---------------------------------------------
	worldTransform_.Initialize();
	worldTransform_.translation_ = { 0.0f, 0.0f, 50.0f };
	worldTransform_.rotation_.y = std::numbers::pi_v<float>;
	worldTransform_.UpdateMatrix(*camera_);

	// 3Dレティクル
	worldTransform3DReticle_.Initialize();
	model3DReticle_.reset(Model::CreateSphere("Resources/white1x1.png"));

	// 衝突設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(~kCollisionAttributePlayer);
}

void Player::Update() {
	// IMGUI
	ManageImGui();

	// 移動処理
	Move();

	// 回転処理
	Rotate();

	// クランプ処理
	ClampInWindow();

	// トランスフォーム更新
	worldTransform_.UpdateMatrix(*camera_);

	// レティクル
	Update3DReticle();

	// アタック
	Fire();

	// 弾
	UpdateBullets();
}

void Player::Draw() {
	model_->Draw(worldTransform_, *camera_);

	// 弾描画
	for (auto& bullet : bullets_) {
		bullet->Draw();
	}

	model3DReticle_->Draw(worldTransform3DReticle_, *camera_, kFillModeWireframe);
}

void Player::RegisterGlobalVariables() {
	GlobalVariables* gv = GlobalVariables::GetInstance();

	gv->AddItem(ParamKey::kGroupKey, ParamKey::kSpeed, Param::speed_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kMoveLimitBlank, Param::moveLimitBlank_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kRotateSpeed, Param::rotateSpeed_);
	gv->AddItem(ParamKey::kGroupKey, ParamKey::kBulletSpeed, Param::bulletSpeed_);
}

void Player::ApplyGlobalVariables() {
	GlobalVariables* gv = GlobalVariables::GetInstance();

	Param::speed_ = gv->GetValue<float>(ParamKey::kGroupKey, ParamKey::kSpeed);
	Param::moveLimitBlank_ = gv->GetValue<float>(ParamKey::kGroupKey, ParamKey::kMoveLimitBlank);
	Param::rotateSpeed_ = gv->GetValue<float>(ParamKey::kGroupKey, ParamKey::kRotateSpeed);
	Param::bulletSpeed_ = gv->GetValue<float>(ParamKey::kGroupKey, ParamKey::kBulletSpeed);
}

void Player::OnCollision() {}

void Player::Move() { // 移動ベクトル
	Vector3 move = { 0, 0, 0 };

	if (Input::GetKey(DIK_LEFT)) {
		move.x -= 1.0f;
	}
	else if (Input::GetKey(DIK_RIGHT)) {
		move.x += 1.0f;
	}
	if (Input::GetKey(DIK_DOWN)) {
		move.y -= 1.0f;
	}
	else if (Input::GetKey(DIK_UP)) {
		move.y += 1.0f;
	}

	worldTransform_.translation_ += Normalize(move) * Param::speed_;
}

void Player::Rotate() {
	if (Input::GetKey(DIK_A)) {
		worldTransform_.rotation_.y -= Param::rotateSpeed_;
	}
	else if (Input::GetKey(DIK_D)) {
		worldTransform_.rotation_.y += Param::rotateSpeed_;
	}
}

void Player::ManageImGui() {
	#ifdef USE_IMGUI
	ImGui::Begin("Player");
	ImGui::DragFloat3("Translate", &worldTransform_.translation_.x, 0.01f);
	ImGui::End();
	#endif // USE_IMGUI
}

void Player::ClampInWindow() {
	Rect bounds = camera_->GetVisibleRect(worldTransform_.translation_.z - camera_->translation_.z, Param::moveLimitBlank_);

	worldTransform_.translation_.x = std::clamp(worldTransform_.translation_.x, bounds.left, bounds.right);
	worldTransform_.translation_.y = std::clamp(worldTransform_.translation_.y, bounds.bottom, bounds.top);
}

void Player::Fire() {
	if (Input::GetKeyTrigger(DIK_SPACE)) {
		// 弾の速度
		Vector3 velocity;

		// 時期から照準オブジェクトへのベクトル
		velocity = worldTransform3DReticle_.GetWorldPosition() - worldTransform_.GetWorldPosition();
		velocity = Normalize(velocity) * Param::bulletSpeed_;

		// 弾を生成し、初期化
		std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
		newBullet->Initialize(modelBullet_, camera_, worldTransform_.GetWorldPosition(), velocity);

		// 弾を登録する
		bullets_.push_back(std::move(newBullet));
	}
}

void Player::UpdateBullets() {

	// 更新
	for (auto& bullet : bullets_) {
		bullet->Update();
	}

	// デスフラグの立った弾を排除
	bullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet) {
		if (bullet->IsDead()) {
			return true;
		}
		return false;
	});
}

void Player::Update3DReticle() {
	//自機のワールド座標から3Dレティクルのワールド座標を計算
	{
		// 自機から3Dレティクルへの距離
		const float kDistancePlayerTo3DReticle = 50.0f;
		//自機から3Dレティクルへのオフセット(Z+向き)
		Vector3 offset = { 0, 0, 1.0f };
		// 自機のワールド行列の回転を反映
		offset = TransformNormal(offset, worldTransform_.matWorld_);
		// ベクトルの長さを整える
		offset = Normalize(offset) * -kDistancePlayerTo3DReticle;
		// 3Dレティクルの座標を設定
		worldTransform3DReticle_.translation_ = worldTransform_.GetWorldPosition() + offset;
		worldTransform3DReticle_.UpdateMatrix(*camera_);
	}
}

#include "Player.h"
#include <algorithm>
#include <cmath>
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
	worldTransform_.translation_ = { 0.0f, 0.0f, 30.0f };
	worldTransform_.rotation_.y = std::numbers::pi_v<float>;
	worldTransform_.UpdateMatrix(*camera_);

	// 3Dレティクル
	worldTransform3DReticle_.Initialize();
	model3DReticle_.reset(Model::CreateSphere("Resources/white1x1.png"));

	uint32_t textureIndex2DReticle = TextureManager::GetInstance()->LoadTexture("Resources/reticle.png");
	sprite2DReticle_.reset(Sprite::Create(textureIndex2DReticle, { 0.0f, 0.0f }, 128.0f, 128.0f, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.5f, 0.5f }));

	// 衝突設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(~kCollisionAttributePlayer);
}

void Player::Update() {
	UpdateControlType();

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
	UpdateCursorReticle();

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

	// 3Dレティクル
	if (isAcitiveDraw3dReticle) {
		model3DReticle_->Draw(worldTransform3DReticle_, *camera_, kFillModeWireframe);
	}
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

Vector3 Player::GetFirstPersonCameraPosition() const {
	// The model faces local -Z. Place the camera just ahead of its nose.
	constexpr float kNoseOffset = 2.0f;
	const Vector3 forward = Normalize(TransformNormal({ 0.0f, 0.0f, -1.0f }, worldTransform_.matWorld_));
	return worldTransform_.GetWorldPosition() + forward * kNoseOffset;
}

Vector3 Player::GetFirstPersonCameraRotation() const {
	// The player model uses a pi-radian Y rotation to correct its imported orientation.
	Vector3 rotation = worldTransform_.rotation_;
	rotation.y -= std::numbers::pi_v<float>;
	return rotation;
}

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

	Vector2 leftStick = Input::GetLeftStick();
	move.x += leftStick.x;
	move.y += leftStick.y;

	if (Length(move) > 1.0f) {
		move = Normalize(move);
	}

	worldTransform_.translation_ += move * Param::speed_;
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
	const float distanceToCamera = worldTransform_.translation_.z - camera_->translation_.z;
	// FPS view places the camera at or ahead of the player. Such a view has no
	// meaningful screen-space movement bounds for the player model.
	if (distanceToCamera <= camera_->nearZ) {
		return;
	}

	Rect bounds = camera_->GetVisibleRect(distanceToCamera, Param::moveLimitBlank_);

	worldTransform_.translation_.x = std::clamp(worldTransform_.translation_.x, bounds.left, bounds.right);
	worldTransform_.translation_.y = std::clamp(worldTransform_.translation_.y, bounds.bottom, bounds.top);
}

void Player::Fire() {
	bool isRightTriggerPressed = Input::GetRightTrigger() > 0.5f;
	bool isFireTriggered = Input::GetClickTrigger(0) || Input::GetKeyTrigger(DIK_SPACE) || (isRightTriggerPressed && !wasRightTriggerPressed_);
	wasRightTriggerPressed_ = isRightTriggerPressed;

	if (isFireTriggered) {
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
		Vector3 offset;
		Vector2 rightStick = Input::GetRightStick();

		if (Vector2::Length(rightStick) > 0.0f) {
			offset = Normalize(Vector3{ rightStick.x, rightStick.y, 1.0f }) * kDistancePlayerTo3DReticle;
			offset.x *= 0.2f;
			offset.y *= 0.2f;
		}
		else {
			offset = { 0, 0, 1.0f };
			// 自機のワールド行列の回転を反映
			offset = TransformNormal(offset, worldTransform_.matWorld_);
			// ベクトルの長さを整える
			offset = Normalize(offset) * -kDistancePlayerTo3DReticle;
		}

		// 3Dレティクルの座標を設定
		worldTransform3DReticle_.translation_ = worldTransform_.GetWorldPosition() + offset;
		worldTransform3DReticle_.UpdateMatrix(*camera_);
	}
}

void Player::Update2DReticle() {

	// 3Dレティクルのワールド座標から2Dレティクルのスクリーン座標を計算
	{
		Vector3 positionReticle = worldTransform3DReticle_.GetWorldPosition();

		// ビューポート行列
		Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

		// ビュー行列とプロジェクション行列、ビューポート行列を合成する
		Matrix4x4 matViewProjectionViewport = camera_->matView * camera_->matProjection * matViewport;

		// ワールド>スクリーン座標変換(ここで3Dから2Dになる)
		positionReticle = Transform(positionReticle, matViewProjectionViewport);

		// スプライトのレティクルに座標設定
		sprite2DReticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));
	}
}

void Player::UpdateCursorReticle() {
	// マウスカーソルのスクリーン座標を取得して3dレティクル配置
	{
		if (controlType_ == ControlType::kControlTypeKeyboard) {
			//マウス座標(クライアントエリア座標)を取得する
			Vector2 mousePos = Input::GetMouseClientPos();

			//	マウス座標を2Dレティクルのスプライトに代入する
			sprite2DReticle_->SetPosition(mousePos);
		}
		else if (controlType_ == ControlType::kControlTypeGamepad) {
			// 右スティックの入力を取得する
			Vector2 rightStick = Input::GetRightStick();
			rightStick.y *= -1.0f; // スクリーン座標なのでY軸を反転する

			// 右スティックの入力に応じて2Dレティクルの位置を更新する
			sprite2DReticle_->SetPosition(sprite2DReticle_->GetPosition() + rightStick * 10.0f);
		}

		//	ビュー行列、プロジェクション行列、ビューポート行列の合成行列を計算する
		Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
		Matrix4x4 matVPV = camera_->matView * camera_->matProjection * matViewport;

		//	合成行列の逆行列を計算する
		Matrix4x4 matInverseVPV = Inverse(matVPV);

		// スクリーン座標
		Vector3 posNear = Vector3(sprite2DReticle_->GetPosition().x, sprite2DReticle_->GetPosition().y, 0);
		Vector3 posFar = Vector3(sprite2DReticle_->GetPosition().x, sprite2DReticle_->GetPosition().y, 1);

		//	ニアクリップ面上のワールド座標を得る(スクリーン→ワールド変換)
		posNear = Transform(posNear, matInverseVPV);

		//	ファークリップ面上のワールド座標を得る(スクリーン→ワールド変換)
		posFar = Transform(posFar, matInverseVPV);

		//	マウスの前方ベクトルを計算する
		Vector3 mouseDirection = Normalize(posFar - posNear);

		// カメラから照準オブジェクトの距離
		const float kDistanceTestObject = -camera_->translation_.z + worldTransform_.translation_.z + 50.0f;

		//	ニアクリップ面上のワールド座標から一定距離前進したところに3Dレティクルを配置
		worldTransform3DReticle_.translation_ = posNear + mouseDirection * kDistanceTestObject;
		worldTransform3DReticle_.UpdateMatrix(*camera_);


		ImGui::Begin("Player");
		ImGui::Text("2DReticle: (%f,%f)", sprite2DReticle_->GetPosition().x, sprite2DReticle_->GetPosition().y);
		ImGui::Text("Near: (%+.2f,%+.2f,%+.2f )", posNear.x, posNear.y, posNear.z);
		ImGui::Text("Far : (%+ .2f,%+.2f,%+.2f)", posFar.x, posFar.y, posFar.z);
		ImGui::Text("3DReticle : (%+.2f,%+.2f,%+.2f )", worldTransform3DReticle_.translation_.x, worldTransform3DReticle_.translation_.y, worldTransform3DReticle_.translation_.z);
		ImGui::End();
	}
}

void Player::UpdateControlType() {
	// マウス座標が異なる場合はキーボード操作とみなす
	if (Input::GetMouseClientPos() != Input::GetMousePreClientPos()) {
		controlType_ = ControlType::kControlTypeKeyboard;
	}

	// 右スティックの入力がある場合はゲームパッド操作とみなす
	if (Vector2::Length(Input::GetRightStick()) > 0.0f) {
		controlType_ = ControlType::kControlTypeGamepad;
	}
}

void Player::DrawUI() {
	sprite2DReticle_->Draw();
}


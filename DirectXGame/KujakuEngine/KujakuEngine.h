#pragma once

#include <cstdint>
#include <string>

// エンジン内部モジュール
#include "base/DirectXCommon.h"
#include "base/WinApp.h"

#include "2d/ImGuiManager.h"
#include "2d/Sprite.h"

#include "3d/GraphicsPipeline.h"
#include "3d/Model.h"
#include "3d/WorldTransform.h"
#include "3d/Camera.h"

#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Matrix3x3.h"
#include "math/Matrix4x4.h"

namespace KujakuEngine {

/// <summary>
/// エンジンの初期化
/// </summary>
/// <param name="title">ウィンドウタイトル</param>
/// <param name="enableDebugLayer">デバッグレイヤーを有効にするか</param>
void Initialize(const std::wstring& title = L"KujakuEngine", bool enableDebugLayer = false);

/// <summary>
/// エンジンの終了処理
/// </summary>
void Finalize();

/// <summary>
/// フレーム更新（メッセージ処理）
/// </summary>
/// <returns>falseで終了リクエスト</returns>
bool Update();

/// <summary>
/// 描画前処理
/// GraphicsPipeline・DirectXCommon のPreDrawをまとめて呼ぶ
/// </summary>
void PreDraw();

/// <summary>
/// 描画後処理
/// ImGui描画 + DirectXCommon のPostDrawをまとめて呼ぶ
/// </summary>
void PostDraw();

} // namespace KujakuEngine

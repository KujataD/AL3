#include "KujakuEngine/KujakuEngine.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include <memory>

// Windowsアプリでのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    // エンジン初期化
	KujakuEngine::Initialize(L"KujakuEngine", true);

    // ゲームループ
	while (KujakuEngine::Update()) {
		
        KujakuEngine::ImGuiManager::GetInstance()->Begin();

        ///
        /// ↓↓↓ 更新処理ここから ↓↓↓
        ///

        ///
        /// ↑↑↑ 更新処理ここまで ↑↑↑
        ///

		KujakuEngine::PreDraw();

        ///
        /// ↓↓↓ 描画処理ここから ↓↓↓
        ///

        ///
        /// ↑↑↑ 描画処理ここまで ↑↑↑
        ///		
		
        KujakuEngine::PostDraw();
	}

	// エンジンの終了処理
	KujakuEngine::Finalize();

	return 0;
}
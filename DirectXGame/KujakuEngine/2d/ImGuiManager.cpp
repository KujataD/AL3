#include "ImGuiManager.h"
#include "../base/DirectXCommon.h"
#include "../base/WinApp.h"

#include "../../externals/imgui/imgui.h"
#include "../../externals/imgui/imgui_impl_dx12.h"
#include "../../externals/imgui/imgui_impl_win32.h"

namespace KujakuEngine {

ImGuiManager* ImGuiManager::GetInstance() {
	static ImGuiManager instance;
	return &instance;
}

void ImGuiManager::Initialize() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	WinApp* winApp = WinApp::GetInstance();

	// ImGui初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHwnd());
	ImGui_ImplDX12_Init(
	    dxCommon->GetDevice(),
	    2, // BufferCount（ダブルバッファ）
	    DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, dxCommon->GetSrvDescriptorHeap(),
	    // SRVヒープの先頭（index=0）をImGuiが使用する
	    dxCommon->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), dxCommon->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
}

void ImGuiManager::Begin() {
	// main.cpp のフレーム先頭処理に対応
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::End() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList());
}

void ImGuiManager::Finalize() {
	// main.cpp の終了処理と同じ順序（初期化の逆順）
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

} // namespace KujakuEngine
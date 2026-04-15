#pragma once

#include <Windows.h>
#include <chrono>
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <string>
#include <vector>
#include <wrl.h>

#include "WinApp.h"

namespace KujakuEngine {

/// <summary>
/// DirectX汎用
/// </summary>
class DirectXCommon {
public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	static DirectXCommon* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="winApp">ウィンドウ管理クラスのポインタ</param>
	/// <param name="backBufferWidth">バックバッファ幅</param>
	/// <param name="backBufferHeight">バックバッファ高さ</param>
	/// <param name="enableDebugLayer">デバッグレイヤーの有効化</param>
	void Initialize(WinApp* winApp, int32_t backBufferWidth = WinApp::kWindowWidth, int32_t backBufferHeight = WinApp::kWindowHeight, bool enableDebugLayer = false);

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// レンダーターゲットのクリア
	/// </summary>
	void ClearRenderTarget();

	/// <summary>
	/// 深度バッファのクリア
	/// </summary>
	void ClearDepthBuffer();

	/// <summary>
	/// 初期化済みかどうか
	/// </summary>
	bool IsInitialized() const { return initialized_; }

	// --- ゲッター ---
	ID3D12Device* GetDevice() const { return device_.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }
	ID3D12DescriptorHeap* GetSrvDescriptorHeap() const { return srvDescriptorHeap_.Get(); }
	ID3D12DescriptorHeap* GetRtvDescriptorHeap() const { return rtvDescriptorHeap_.Get(); }
	ID3D12DescriptorHeap* GetDsvDescriptorHeap() const { return dsvDescriptorHeap_.Get(); }
	int32_t GetBackBufferWidth() const { return backBufferWidth_; }
	int32_t GetBackBufferHeight() const { return backBufferHeight_; }

private:
	DirectXCommon() = default;
	~DirectXCommon() = default;
	DirectXCommon(const DirectXCommon&) = delete;
	const DirectXCommon& operator=(const DirectXCommon&) = delete;

	// --- 初期化フロー ---
	void InitializeDXGIDevice(bool enableDebugLayer);
	void InitializeCommand();
	void CreateSwapChain();
	void CreateFinalRenderTargets();
	void CreateDepthBuffer();
	void CreateFence();

	// --- ユーティリティ ---
	void Log(const std::string& message) { OutputDebugStringA(message.c_str()); }
	std::wstring ConvertString(const std::string& str);

private:
	WinApp* winApp_ = nullptr;
	bool initialized_ = false;

	// Direct3D関連
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;

	// リソース関連
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2];
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;

	// ディスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;

	// 同期関連
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
	uint64_t fenceValue_ = 0;
	HANDLE fenceEvent_ = nullptr;

	uint32_t backBufferIndex_ = 0;
	int32_t backBufferWidth_ = 0;
	int32_t backBufferHeight_ = 0;
};

} // namespace KujakuEngine

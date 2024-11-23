/*!
@file BaseDevice.h
@brief デバイスクラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
*/

#pragma once

#include "stdafx.h"


namespace basecross {

	class BaseUI;
	class BaseScene;
	class Scene;


#ifndef USE_DXGI_1_6
#define DXGI_GPU_PREFERENCE_UNSPECIFIED 0
#define DXGI_GPU_PREFERENCE_MINIMUM_POWER 1
#define DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE 2
#define DXGI_GPU_PREFERENCE UINT
#endif

	//--------------------------------------------------------------------------------------
	//	デバイス
	//--------------------------------------------------------------------------------------
	class BaseDevice : public PrimDevice
	{
	public:
		BaseDevice(UINT width, UINT height, std::wstring name);
		~BaseDevice();

		static const UINT FrameCount = 3; //フレーム数
		//前回のターンからの経過時間の取得
		double GetElapsedTime() {
			return m_timer.GetElapsedSeconds();
		}
		//デバイス（自分自身）の取得
		static BaseDevice* GetBaseDevice() {
			return s_device;
		}
		//シーンの取得
		static std::shared_ptr<Scene> GetScene() {
			return GetBaseDevice()->m_scene;
		}
		//ID3D12Deviceの取得
		ComPtr<ID3D12Device> GetID3D12Device() const {
			return m_device;
		}
	protected:
		//初期化
		virtual void OnInit();
		//キーボードを押す
		virtual void OnKeyDown(UINT8 key);
		//キーボードを離す
		virtual void OnKeyUp(UINT8 key);
		//サイズが変えられた
		virtual void OnSizeChanged(UINT width, UINT height, bool minimized);
		//更新処理
		virtual void OnUpdate();
		//描画処理
		virtual void OnRender();
		//終了処理
		virtual void OnDestroy();
		//スワップチェーンの取得
		virtual IDXGISwapChain* GetSwapchain() { return m_swapChain.Get(); }
	private:
		// 以下GPU関連
		struct DxgiAdapterInfo
		{
			DXGI_ADAPTER_DESC1 desc;
			bool supportsDx12FL11;
		};
		DXGI_GPU_PREFERENCE m_activeGpuPreference;
		std::map<DXGI_GPU_PREFERENCE, std::wstring> m_gpuPreferenceToName;
		UINT m_activeAdapter;
		LUID m_activeAdapterLuid;
		std::vector<DxgiAdapterInfo> m_gpuAdapterDescs;
		bool m_manualAdapterSelection;
		HANDLE m_adapterChangeEvent;
		DWORD m_adapterChangeRegistrationCookie;
		// 以下D3D オブジェクト
		ComPtr<ID3D12Device> m_device;
		ComPtr<ID3D12CommandQueue> m_commandQueue;
#ifdef USE_DXGI_1_6
		ComPtr<IDXGISwapChain4> m_swapChain;
		ComPtr<IDXGIFactory6>   m_dxgiFactory;
#else
		ComPtr<IDXGISwapChain3> m_swapChain;
		ComPtr<IDXGIFactory2>   m_dxgiFactory;
#endif
		UINT m_dxgiFactoryFlags;
		ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
		ComPtr<ID3D12Fence> m_fence;
		//シーン
		std::shared_ptr<Scene> m_scene;
		// UI文字列
		std::unique_ptr<BaseUI> m_uiLayer;
		bool m_bCtrlKeyIsPressed;
		float m_fps;
		//タイマー
		StepTimer m_timer;
		// Frame 同期オブジェクト
		UINT   m_frameIndex;
		HANDLE m_fenceEvent;
		UINT64 m_fenceValues[FrameCount];

		// Windowステート
		bool m_windowVisible;
		bool m_windowedMode;

		// 自分自身のポインタ
		static BaseDevice* s_device;
		//パイプラインオブジェクトの設定
		void LoadPipeline();
		//アセットオブジェクトの設定
		void LoadAssets();
		//ウインドウ依存リソースの設定
		void LoadSizeDependentResources();
		//ウインドウ依存リソースの解放
		void ReleaseSizeDependentResources();
		// UI文字列の更新
		void UpdateUI();
		//D3Dオブジェクトの再構築
		void RecreateD3Dresources();
		//D3Dオブジェクトのリリース
		void ReleaseD3DObjects();
		//以下GPU関連
		void EnumerateGPUadapters();
		void GetGPUAdapter(UINT adapterIndex, IDXGIAdapter1** ppAdapter);
		bool QueryForAdapterEnumerationChanges();
		HRESULT ValidateActiveAdapter();
		bool RetrieveAdapterIndex(UINT* adapterIndex, LUID prevActiveAdapterLuid);
		void SelectAdapter(UINT index);
		void SelectGPUPreference(UINT index);
		//fpsの計算
		void CalculateFrameStats();
		//GPUのウエイト
		void WaitForGpu(ID3D12CommandQueue* pCommandQueue);
		//次のフレームへの移行
		void MoveToNextFrame();
	};
}
using namespace basecross;
//end namespace basecross

/*!
@file BaseDevice.h
@brief デバイスクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	ダミーシェーダー
	//--------------------------------------------------------------------------------------
	DECLARE_DX12SHADER(VSPNTStaticInit)
	DECLARE_DX12SHADER(PSPNTStaticInit)

	struct BaseFrame;

	//--------------------------------------------------------------------------------------
	///	デバイスクラス
	//--------------------------------------------------------------------------------------
	class BaseDevice
	{
		//幅
		UINT m_width;
		//高さ
		UINT m_height;
		//縦横比
		float m_aspectRatio;
		//ESCキーで終了させるかどうか
		bool m_QuiteEscapeKey;
		//ラップモードかどうか
		bool m_useWarpDevice;
		//ウインドウタイトル
		wstring m_title;
		//フレーム数
		static const UINT m_frameCount = 3;
		//ライトの数
		static const UINT m_numLights = 3;

		// Pipeline objects.
		CD3DX12_VIEWPORT m_viewport;
		CD3DX12_RECT m_scissorRect;

		ComPtr<ID3D12Device> m_device;
		ComPtr<ID3D12CommandQueue> m_commandQueue;
		ComPtr<IDXGISwapChain3> m_swapChain;
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
		ComPtr<ID3D12DescriptorHeap> m_cbvSrvUavHeap;
		ComPtr<ID3D12DescriptorHeap> m_samplerHeap;


		ComPtr<ID3D12RootSignature> m_rootSignature;
		ComPtr<ID3D12Resource> m_renderTargets[m_frameCount];
		ComPtr<ID3D12Resource> m_depthStencil;
		//初期化用のパイプライステート
		ComPtr<ID3D12PipelineState> m_initPipelineState;
		//汎用的なコマンドリスト
		ComPtr<ID3D12GraphicsCommandList> m_initCommandList;

		ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		ComPtr<ID3D12PipelineState> m_pipelineStateShadowMap;

		//rtv管理用
		UINT m_rtvDescriptorIncrementSize;
		//dsv管理用
		UINT m_dsvDescriptorIncrementSize;
		//sampler管理用
		map<wstring, UINT> m_samplerMap;
		UINT m_samplerDescriptorIncrementSize;
		const UINT m_samplerMax = 512;
		UINT m_samplerSendIndex;
		//cbvSrvUav管理用
		UINT m_cbvSrvUavDescriptorIncrementSize;
		const UINT m_cbvSrvUavMax = 4096;
		const UINT m_srvStartIndex = 0;
		const UINT m_srvMax = 1024;
		UINT m_srvSendIndex;
		const UINT m_cbvUavStartIndex = 1024;
		const UINT m_cbvUavMax = 4096;
		UINT m_cbvUavSendIndex;

		//同期オブジェクト
		ComPtr<ID3D12Fence> m_fence;
		UINT m_frameIndex;
		HANDLE m_fenceEvent;
		UINT64 m_fenceValue;

		//初期化処理が終わったかどうか
		bool m_isInited;

		// フレーム
		BaseFrame* m_baseFrame[m_frameCount];
		int m_currentBaseFrameIndex;

		//ルートシグネチャで設定されるGPUスロットのマップ
		map<wstring, UINT> m_gpuSlotMap;

		//アプリケーションリソース
		BaseTimer m_timer;

		//ハードウェアアダプターを得る
		void GetHardwareAdapter(
			_In_ IDXGIFactory1* pFactory,
			_Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
			bool requestHighPerformanceAdapter = false);
		//パイプラインオブジェクトの作成
		void LoadPipeline();
		//アセットオブジェクトの作成
		void LoadAssets();
		//ルート署名の作成
		void CreateRootSignature();
		//レンダリングターゲットビューの作成
		void CreateRenderTargetViews();
		//デプスステンシルビューの作成
		void CreateDepthStencilView();
		//サンプラーの作成
		void CreateSamplers();
		//構築用のパイプラインの作成
		void CreateInitPipelineState();
		//フレームの作成
		void CreateBaseFrame();
		//コマンドリストを集める
		void PopulateCommandList(BaseFrame* pBaseFrame);
		//同期オブジェクトの作成
		void CreateSynchronizationObjects();
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンストラクタ
		@param[in]	width	ゲーム幅
		@param[in]	height	ゲーム高さ
		@param[in]	title	ウインドウタイトル
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		BaseDevice(UINT width, UINT height,const wstring& title);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デストラクタ
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual ~BaseDevice();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デバイスの初期化時に呼ばれるイベント
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void OnInit();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デバイスの更新時に呼ばれるイベント
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void OnUpdate();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デバイスの描画時に呼ばれるイベント
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void OnRender();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デバイスの破棄時に呼ばれるイベント
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void OnDestroy();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	キーが押された時に呼ばれるイベント
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void OnKeyDown(UINT8 key);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	キーが離された時に呼ばれるイベント
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void OnKeyUp(UINT8 key);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ゲーム幅の取得
		@return	ゲーム幅
		*/
		//--------------------------------------------------------------------------------------
		UINT GetWidth() const { return m_width; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ゲーム高さの取得
		@return	ゲーム高さ
		*/
		//--------------------------------------------------------------------------------------
		UINT GetHeight() const { return m_height; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ウインドウタイトルの取得
		@return	ウインドウタイトル
		*/
		//--------------------------------------------------------------------------------------
		const wstring GetTitle() const { return m_title; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ビューポートの取得
		@return	ビューポート
		*/
		//--------------------------------------------------------------------------------------
		CD3DX12_VIEWPORT GetViewport() const { return m_viewport; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	シーザーレクトの取得
		@return	シーザーレクト
		*/
		//--------------------------------------------------------------------------------------
		CD3DX12_RECT GetScissorRect() const { return m_scissorRect; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Escで終了するかどうかの取得
		@return	終了するならtrue
		*/
		//--------------------------------------------------------------------------------------
		bool IsQuiteEscapeKey() const {return m_QuiteEscapeKey;}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Escで終了するかどうかの設定
		@param[in]	b	true/false
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetQuiteEscapeKey(bool b) {m_QuiteEscapeKey = b;}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コマンドラインをパースする
		@param[in]	argv	コマンドライン文字列のポインタの配列
		@param[in]	argc	コマンドラインの文字列数
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void ParseCommandLineArgs(_In_reads_(argc) WCHAR* argv[], int argc);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	スワップチェーンの取得
		@return	スワップチェーンのComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<IDXGISwapChain3> GetSwapChain() const {
			return m_swapChain;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ID3D12Deviceの取得
		@return	ID3D12DeviceのComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12Device> GetID3D12Device() const {
			return m_device;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ルート認証の取得
		@return	ルート認証のComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12RootSignature> GetRootSignature() const {
			return m_rootSignature;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	CbvSrvUavディスクリプタヒープの取得
		@return	CbvSrvUavディスクリプタヒープのComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12DescriptorHeap> GetCbvSrvUavDescriptorHeap() const {
			return m_cbvSrvUavHeap;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	CbvSrvUavディスクリプタヒープのインクリメントサイズの取得
		@return	CbvSrvUavディスクリプタヒープのインクリメントサイズ
		*/
		//--------------------------------------------------------------------------------------
		UINT GetCbvSrvUavDescriptorHandleIncrementSize() const {
			return m_cbvSrvUavDescriptorIncrementSize;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Srvの次のインデックスの取得
		@return	Srvの次のインデックス
		*/
		//--------------------------------------------------------------------------------------
		UINT GetSrvNextIndex();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Cbvの次のインデックスの取得
		@return	Cbvの次のインデックス
		*/
		//--------------------------------------------------------------------------------------
		UINT GetCbvUavNextIndex();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	サンプラーディスクリプタヒープの取得
		@return	サンプラーディスクリプタヒープのComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12DescriptorHeap> GetSamplerDescriptorHeap() const {
			return m_samplerHeap;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	サンプラーディスクリプタヒープのインクリメントサイズの取得
		@return	サンプラーディスクリプタヒープのインクリメントサイズ
		*/
		//--------------------------------------------------------------------------------------
		UINT GetSamplerDescriptorHandleIncrementSize() const {
			return m_samplerDescriptorIncrementSize;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	サンプラーの次のインデックスの取得
		@return	サンプラーの次のインデックス
		*/
		//--------------------------------------------------------------------------------------
		UINT GetSamplerNextIndex();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	指定したサンプラーのインデックスの取得
		@param[in]	key	LinearWrapなどのキー
		@return	指定したサンプラーのインデックス（存在しなかったらしたらUINT_MAXを返す）
		*/
		//--------------------------------------------------------------------------------------
		UINT GetSamplerIndex(const wstring& key);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	汎用的なコマンドリストの取得
		@return	汎用的コマンドリストのComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12GraphicsCommandList> GetComandList()const {
			if (!m_isInited) {
				return m_initCommandList;
			}
			else {
				return GetCurrentBaseFrame()->m_commandList;
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	GPUのスロットIDの設定
		@param[in]	key	s0,t0などのキー
		@param[in]	val	ID
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetGpuSlot(const wstring& key, UINT val) {
			m_gpuSlotMap[key] = val;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	GPUのスロットIDの取得
		@param[in]	key	s0,t0などのキー
		@return	keyで指定したキーのID
		*/
		//--------------------------------------------------------------------------------------
		UINT GetGpuSlotID(const wstring& key) {
			auto it = m_gpuSlotMap.begin();
			while (it != m_gpuSlotMap.end()) {
				if (it->first == key) {
					return it->second;
				}
				it++;
			}
			throw BaseException(
				L"そのキーはGPUスロットにありません",
				key,
				L"BaseDevice::GetGpuSlotID()"
			);
			return 0;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ElapsedTimeの取得
		@return	ElapsedTime
		*/
		//--------------------------------------------------------------------------------------
		double GetElapsedTime() const {
			return m_timer.GetElapsedSeconds();
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	今使用しているBaseFrameの取得
		@return	BaseFrameのポインタ
		*/
		//--------------------------------------------------------------------------------------
		BaseFrame* GetCurrentBaseFrame() const {
			return m_baseFrame[m_currentBaseFrameIndex];
		}
	};
}
// end namespace basecross

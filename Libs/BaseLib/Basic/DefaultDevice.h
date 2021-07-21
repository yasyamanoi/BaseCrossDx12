/*!
@file DefaultDevice.h
@brief 基本的なデバイスクラス
@copyright Copyright (c) 2021 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	ダミーシェーダー
	//--------------------------------------------------------------------------------------
	DECLARE_DX12SHADER(VSPNTStaticDammy)
	DECLARE_DX12SHADER(PSPNTStaticDammy)

	struct FrameResource;

	//--------------------------------------------------------------------------------------
	///	基本的なデバイスクラス
	//--------------------------------------------------------------------------------------
	class DefaultDevice : public BaseDevice
	{
	public:
		DefaultDevice(UINT width, UINT height, std::wstring name);
		virtual ~DefaultDevice();

		static DefaultDevice* Get() { return s_app; }

		virtual void OnInit()override;
		virtual void OnUpdate()override;
		virtual void OnRender()override;
		virtual void OnDestroy()override;
		virtual void OnKeyDown(UINT8 key) override;
		virtual void OnKeyUp(UINT8 key)override;

		static const UINT m_FrameCount = 3;
		static const UINT m_NumLights = 3;

		ComPtr<IDXGISwapChain3> GetSwapChain() const {
			return m_swapChain;
		}
		ComPtr<ID3D12Device> GetID3D12Device() const {
			return m_device;
		}
		ComPtr<ID3D12RootSignature> GetRootSignature() const {
			return m_rootSignature;
		}
		ComPtr<ID3D12DescriptorHeap> GetCbvSrvUavDescriptorHeap() const {
			return m_cbvSrvHeap;
		}
		UINT GetCbvSrvUavDescriptorHandleIncrementSize() const {
			return m_cbvSrvDescriptorIncrementSize;
		}
		ComPtr<ID3D12DescriptorHeap> GetSamplerDescriptorHeap() const {
			return m_samplerHeap;
		}
		ComPtr<ID3D12GraphicsCommandList> GetComandList() const{
			return m_commandList;
		}
		UINT GetCbvSrvUavNextIndex();
		void SetGpuSlot(const wstring& key, UINT val) {
			m_gpuSlotMap[key] = val;
		}
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

		double GetElapsedTime() const {
			return m_timer.GetElapsedSeconds();
		}

		FrameResource* GetCurrentFrameResource() const {
			return m_pCurrentFrameResource;
		}

	protected:
		//ルートシグネチャで設定されるGPUスロットのマップ
		map<wstring, UINT> m_gpuSlotMap;
		//DefaultDeviceのシングルトンオブジェクト
		static DefaultDevice* s_app;
		//パイプラインオブジェクト
		CD3DX12_VIEWPORT m_viewport;
		CD3DX12_RECT m_scissorRect;
		ComPtr<ID3D12Device> m_device;
		ComPtr<ID3D12CommandQueue> m_commandQueue;
		ComPtr<IDXGISwapChain3> m_swapChain;
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		UINT m_rtvDescriptorIncrementSize;
		ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
		ComPtr<ID3D12DescriptorHeap> m_cbvSrvHeap;
		UINT m_cbvSrvDescriptorIncrementSize;
		const UINT m_cbvSrvUavMax = 2048;
		UINT m_cbvSrvUavSendIndex;
		ComPtr<ID3D12DescriptorHeap> m_samplerHeap;
		ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		ComPtr<ID3D12RootSignature> m_rootSignature;
		ComPtr<ID3D12Resource> m_renderTargets[m_FrameCount];
		ComPtr<ID3D12Resource> m_depthStencil;
		ComPtr<ID3D12GraphicsCommandList> m_commandList;
		//ダミーのパイプライステート
		ComPtr<ID3D12PipelineState> m_pipelineStateDammy;

		//アプリケーションリソース
		StepTimer m_timer;

		//同期オブジェクト
		ComPtr<ID3D12Fence> m_fence;
		UINT m_frameIndex;
		UINT m_frameCounter;
		HANDLE m_fenceEvent;
		UINT64 m_fenceValue;

		// フレームリソース
		FrameResource* m_frameResources[m_FrameCount];
		FrameResource* m_pCurrentFrameResource;
		int m_currentFrameResourceIndex;

		void LoadPipeline();
		void LoadAssets();
		void CreateRootSignature();
		void CreateRenderTargetViews();
		void CreateDepthStencil();
		void CreateSamplers();
		void CreateFrameResources();
		void CreateSynchronizationObjects();

		void CreatePipelineDammyState();

		void PopulateCommandList(FrameResource* pFrameResource);

	};

}
// end namespace basecross


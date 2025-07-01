//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

/*!
@file BaseDevice.h
@brief �f�o�C�X�N���X
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#pragma once

#include "stdafx.h"
namespace basecross {

	class UILayer;
	class BaseScene;
	class Scene;

#ifndef USE_DXGI_1_6
#define DXGI_GPU_PREFERENCE_UNSPECIFIED 0
#define DXGI_GPU_PREFERENCE_MINIMUM_POWER 1
#define DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE 2
#define DXGI_GPU_PREFERENCE UINT
#endif

	//--------------------------------------------------------------------------------------
	///	�f�o�C�X�N���X
	//--------------------------------------------------------------------------------------
	class BaseDevice : public PrimDevice
	{
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�R���X�g���N�^
		@param[in]	width	��
		@param[in]	height	����
		@param[in]	name	�E�C���h�E�N���X��
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		BaseDevice(UINT width, UINT height, std::wstring name);
		~BaseDevice();

		static const UINT FrameCount = 3;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�X���b�v�`�F�[���𓾂�
		@return	IDXGISwapChain�̃|�C���^
		*/
		//--------------------------------------------------------------------------------------
		virtual IDXGISwapChain* GetSwapchain() { return m_swapChain.Get(); }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	D3D12�f�o�C�X�𓾂�
		@return	ID3D12Device�̃|�C���^
		*/
		//--------------------------------------------------------------------------------------
		virtual ID3D12Device* GetD3D12Device() { return m_device.Get(); }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ComPtr��D3D12�f�o�C�X�𓾂�
		@return	ID3D12Device��ComPtr
		*/
		//--------------------------------------------------------------------------------------
		virtual ComPtr<ID3D12Device> GetID3D12Device() { return m_device; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�V�[���i�e�N���X�j�𓾂�
		@return	�V�[���i�e�N���X�j��unique_ptr�̎Q��
		*/
		//--------------------------------------------------------------------------------------
		static std::unique_ptr<BaseScene>& GetScene(){
			return s_app->m_scene;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�������g�̃C���X�^���X�̃|�C���^�𓾂�
		@return	BaseDevice�̃|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static BaseDevice* GetBaseDevice() {
			return s_app;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	1�b�ɂP��X�V�������肵��FPS�𓾂�
		@return	���肵��FPS�𓾂�
		*/
		//--------------------------------------------------------------------------------------
		float GetStableFps() const {
			return m_fps;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	1�b�ɂP��X�V�������肵��ElapsedTime�𓾂�
		@return	���肵��ElapsedTime�𓾂�
		*/
		//--------------------------------------------------------------------------------------
		double GetStableElapsedTime() const {
			return m_elapsedTime;
		}
	protected:
		virtual void OnInit();
		virtual void OnKeyDown(UINT8 key);
		virtual void OnKeyUp(UINT8 key);
		virtual void OnSizeChanged(UINT width, UINT height, bool minimized);
		virtual void OnUpdate();
		virtual void OnRender();
		virtual void SetToBefore();
		virtual void OnDestroy();

	private:
		// GPU adapter management.
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

		// D3D objects.
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

		// BaseScene rendering resources
		std::unique_ptr<BaseScene> m_scene;

		// UILayer
		std::unique_ptr<UILayer> m_uiLayer;
		bool m_bCtrlKeyIsPressed;
		float m_fps;
		double m_elapsedTime;

		StepTimer m_timer;

		// Frame synchronization objects
		UINT   m_frameIndex;
		HANDLE m_fenceEvent;
		UINT64 m_fenceValues[FrameCount];

		// Window state
		bool m_windowVisible;
		bool m_windowedMode;

		// Singleton object so that worker threads can share members.
		static BaseDevice* s_app;

		void LoadPipeline();
		void LoadAssets();
		void LoadSizeDependentResources();
		void ReleaseSizeDependentResources();
		void UpdateUI();
		void RecreateD3Dresources();
		void ReleaseD3DObjects();
		void EnumerateGPUadapters();
		void GetGPUAdapter(UINT adapterIndex, IDXGIAdapter1** ppAdapter);
		bool QueryForAdapterEnumerationChanges();
		HRESULT ValidateActiveAdapter();
		bool RetrieveAdapterIndex(UINT* adapterIndex, LUID prevActiveAdapterLuid);
		void SelectAdapter(UINT index);
		void SelectGPUPreference(UINT index);
		void CalculateFrameStats();
		void WaitForGpu(ID3D12CommandQueue* pCommandQueue);
		void MoveToNextFrame();
	};
}
// end namespace basecross

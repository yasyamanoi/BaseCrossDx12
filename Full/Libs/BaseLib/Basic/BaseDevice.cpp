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
@file BaseDevice.cpp
@brief デバイスクラス　実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"

namespace basecross {


	

	BaseDevice* BaseDevice::s_app = nullptr;

	BaseDevice::BaseDevice(UINT width, UINT height, std::wstring name) :
		PrimDevice(width, height, name),
		m_frameIndex(0),
		m_activeAdapter(0),
		m_bCtrlKeyIsPressed(false),
		m_activeGpuPreference(DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE),
		m_fps(0.0f),
		m_elapsedTime(0.0),
		m_manualAdapterSelection(false),
		m_adapterChangeEvent(NULL),
		m_adapterChangeRegistrationCookie(0),
		m_fenceValues{},
		m_windowVisible(true),
		m_dxgiFactoryFlags(0),
		m_windowedMode(true)
	{
		ThrowIfFailed(DXGIDeclareAdapterRemovalSupport());
		if (!m_tearingSupport)
		{
			// Sample shows handling of use cases with tearing support, which is OS dependent and has been supported since Threshold II.
			// Since the sample demonstrates APIs available in Redstone 4, we don't need to handle non-tearing cases.
			// Tearing being supported or not matters in how adapter switching is handled in fullscreen presentations.
			OutputDebugString(L"Sample must be run on an OS with tearing support.");
			exit(EXIT_FAILURE);
		}

		if (m_useWarpDevice)
		{
			OutputDebugString(L"Sample doesn't support warp parameter.");
			exit(EXIT_FAILURE);
		}

		s_app = this;

		m_gpuPreferenceToName[DXGI_GPU_PREFERENCE_UNSPECIFIED] = L"Unspecified";
		m_gpuPreferenceToName[DXGI_GPU_PREFERENCE_MINIMUM_POWER] = L"Minimum power";
		m_gpuPreferenceToName[DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE] = L"High performance";
	}

	BaseDevice::~BaseDevice()
	{
	}

	void BaseDevice::OnInit()
	{
		LoadPipeline();
		LoadAssets();
		LoadSizeDependentResources();
	}

	// Load the rendering pipeline dependencies.
	void BaseDevice::LoadPipeline()
	{
		m_dxgiFactoryFlags = 0;

#if defined(_DEBUG)
		// Enable the debug layer (requires the Graphics Tools "optional feature").
		{
			ComPtr<ID3D12Debug> debugController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
			{
				debugController->EnableDebugLayer();

				// Enable additional debug layers.
				m_dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			}
		}
#endif
		ThrowIfFailed(CreateDXGIFactory2(m_dxgiFactoryFlags, IID_PPV_ARGS(&m_dxgiFactory)));

#ifdef USE_DXGI_1_6
		ComPtr<IDXGIFactory7> spDxgiFactory7;
		if (SUCCEEDED(m_dxgiFactory->QueryInterface(IID_PPV_ARGS(&spDxgiFactory7))))
		{
			m_adapterChangeEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (m_adapterChangeEvent == nullptr)
			{
				ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
			}
			ThrowIfFailed(spDxgiFactory7->RegisterAdaptersChangedEvent(m_adapterChangeEvent, &m_adapterChangeRegistrationCookie));
		}
#endif

		EnumerateGPUadapters();

		ComPtr<IDXGIAdapter1> hardwareAdapter;
		GetGPUAdapter(m_activeAdapter, &hardwareAdapter);
		ThrowIfFailed(D3D12CreateDevice(
			hardwareAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_device)
		));
		m_activeAdapterLuid = m_gpuAdapterDescs[m_activeAdapter].desc.AdapterLuid;

		// Describe and create the command queue.
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
		NAME_D3D12_OBJECT(m_commandQueue);

		// Describe and create the swap chain.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = FrameCount;
		swapChainDesc.Width = m_width;
		swapChainDesc.Height = m_height;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		// It is recommended to always use the tearing flag when it is available.
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

		ComPtr<IDXGISwapChain1> swapChain;
		// DXGI does not allow creating a swapchain targeting a window which has fullscreen styles(no border + topmost).
		// Temporarily remove the topmost property for creating the swapchain.
		bool prevIsFullscreen = App::IsFullscreen();
		if (prevIsFullscreen)
		{
			App::SetWindowZorderToTopMost(false);
		}
		ThrowIfFailed(m_dxgiFactory->CreateSwapChainForHwnd(
			m_commandQueue.Get(),		// Swap chain needs the queue so that it can force a flush on it.
			App::GetHwnd(),
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain
		));

		if (prevIsFullscreen)
		{
			App::SetWindowZorderToTopMost(true);
		}

		// With tearing support enabled we will handle ALT+Enter key presses in the
		// window message loop rather than let DXGI handle it by calling SetFullscreenState.
		m_dxgiFactory->MakeWindowAssociation(App::GetHwnd(), DXGI_MWA_NO_ALT_ENTER);

		ThrowIfFailed(swapChain.As(&m_swapChain));
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		// Create synchronization objects.
		{
			ThrowIfFailed(m_device->CreateFence(m_fenceValues[m_frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
			m_fenceValues[m_frameIndex]++;

			// Create an event handle to use for frame synchronization.
			m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (m_fenceEvent == nullptr)
			{
				ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
			}
		}
	}

	// Load assets required for rendering.
	void BaseDevice::LoadAssets()
	{
		if (!m_scene)
		{
			m_scene = std::make_unique<Scene>(FrameCount, this);
		}

		// Create a temporary command queue and command list for initializing data on the GPU.
		// Performance tip: Copy command queues are optimized for transfer over PCIe.
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;

		ComPtr<ID3D12CommandQueue> copyCommandQueue;
		ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&copyCommandQueue)));
		NAME_D3D12_OBJECT(copyCommandQueue);

		ComPtr<ID3D12CommandAllocator> commandAllocator;
		ThrowIfFailed(m_device->CreateCommandAllocator(queueDesc.Type, IID_PPV_ARGS(&commandAllocator)));
		NAME_D3D12_OBJECT(commandAllocator);

		ComPtr<ID3D12GraphicsCommandList> commandList;
		ThrowIfFailed(m_device->CreateCommandList(0, queueDesc.Type, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));
		NAME_D3D12_OBJECT(commandList);

		m_scene->Initialize(m_device.Get(), m_commandQueue.Get(), commandList.Get(), m_frameIndex);

		ThrowIfFailed(commandList->Close());

		ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
		copyCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		// Wait until assets have been uploaded to the GPU.
		WaitForGpu(copyCommandQueue.Get());
	}

	// Load resources that are dependent on the size of the main window.
	void BaseDevice::LoadSizeDependentResources()
	{
		for (UINT i = 0; i < FrameCount; i++)
		{
			ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])));
		}

		m_scene->LoadSizeDependentResources(m_device.Get(), m_renderTargets, m_width, m_height);

		if (m_enableUI)
		{
			if (!m_uiLayer)
			{
				m_uiLayer = std::make_unique<UILayer>(FrameCount, m_device.Get(), m_commandQueue.Get());
			}
			m_uiLayer->Resize(m_renderTargets, m_width, m_height);
		}
	}

	// Release resources that are dependent on the size of the main window.
	void BaseDevice::ReleaseSizeDependentResources()
	{
		m_scene->ReleaseSizeDependentResources();
		if (m_enableUI)
		{
			m_uiLayer.reset();
		}
		for (UINT i = 0; i < FrameCount; i++)
		{
			m_renderTargets[i].Reset();
		}
	}

	void BaseDevice::UpdateUI()
	{
		//UI文字列はシーンに任せる
		m_scene->UpdateUI(m_uiLayer);
	}

	// Release sample's D3D objects.
	void BaseDevice::ReleaseD3DObjects()
	{
		m_scene->ReleaseD3DObjects();
		if (m_enableUI)
		{
			m_uiLayer.reset();
		}
		m_fence.Reset();

		ResetComPtrArray(&m_renderTargets);
		m_commandQueue.Reset();
		m_swapChain.Reset();
		m_device.Reset();

#ifdef USE_DXGI_1_6
		ComPtr<IDXGIFactory7> spDxgiFactory7;
		if (m_adapterChangeRegistrationCookie != 0 && SUCCEEDED(m_dxgiFactory->QueryInterface(IID_PPV_ARGS(&spDxgiFactory7))))
		{
			ThrowIfFailed(spDxgiFactory7->UnregisterAdaptersChangedEvent(m_adapterChangeRegistrationCookie));
			m_adapterChangeRegistrationCookie = 0;
			CloseHandle(m_adapterChangeEvent);
			m_adapterChangeEvent = NULL;
		}
#endif
		m_dxgiFactory.Reset();

#if defined(_DEBUG)
		{
			ComPtr<IDXGIDebug1> dxgiDebug;
			if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
			{
				dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
			}
		}
#endif
	}

	void BaseDevice::OnKeyUp(UINT8 key)
	{
		m_scene->KeyUp(key);
		switch (key)
		{
		case VK_CONTROL:
			m_bCtrlKeyIsPressed = false;
			break;
		default:
			break;
		}
	}

	void BaseDevice::OnKeyDown(UINT8 key)
	{
		m_scene->KeyDown(key);
		switch (key)
		{
		case VK_CONTROL:
			m_bCtrlKeyIsPressed = true;
			break;
		default:
			break;
		}
	}

	void BaseDevice::OnSizeChanged(UINT width, UINT height, bool minimized)
	{
		// Determine if the swap buffers and other resources need to be resized or not.
		if ((width != m_width || height != m_height) && !minimized)
		{
			UpdateForSizeChange(width, height);

			if (!m_swapChain)
			{
				return;
			}

			try
			{
				// Flush all current GPU commands.
				WaitForGpu(m_commandQueue.Get());

				// Release the resources holding references to the swap chain (requirement of
				// IDXGISwapChain::ResizeBuffers) and reset the frame fence values to the
				// current fence value.
				ReleaseSizeDependentResources();
				for (UINT i = 0; i < FrameCount; i++)
				{
					m_fenceValues[i] = m_fenceValues[m_frameIndex];
				}

				// Resize the swap chain to the desired dimensions.
				DXGI_SWAP_CHAIN_DESC1 desc = {};
				ThrowIfFailed(m_swapChain->GetDesc1(&desc));
				ThrowIfFailed(m_swapChain->ResizeBuffers(FrameCount, width, height, desc.Format, desc.Flags));

				BOOL fullscreenState;
				ThrowIfFailed(m_swapChain->GetFullscreenState(&fullscreenState, nullptr));
				m_windowedMode = !fullscreenState;

				// Reset the frame index to the current back buffer index.
				m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
				m_scene->SetFrameIndex(m_frameIndex);

				LoadSizeDependentResources();

				m_windowVisible = !minimized;
			}
			catch (HrException& e)
			{
				if ((e.Error() == DXGI_ERROR_DEVICE_REMOVED || e.Error() == DXGI_ERROR_DEVICE_RESET))
				{
					m_width = width;
					m_height = height;
					RecreateD3Dresources();
				}
				else
				{
					throw;
				}
			}
		}
	}

	void BaseDevice::OnUpdate()
	{
		m_timer.Tick();
		CalculateFrameStats();
		m_scene->Update(m_timer.GetElapsedSeconds());
		if (m_enableUI)
		{
			UpdateUI();
		}
	}

	// Render the scene.
	void BaseDevice::OnRender()
	{
		if (m_windowVisible)
		{
			try
			{
				// Check for any adapter changes, such as a new adapter being available.
				if (QueryForAdapterEnumerationChanges())
				{
					// Dxgi factory needs to be recreated on a change.
					ThrowIfFailed(CreateDXGIFactory2(m_dxgiFactoryFlags, IID_PPV_ARGS(&m_dxgiFactory)));

#ifdef USE_DXGI_1_6
					ComPtr<IDXGIFactory7> spDxgiFactory7;
					if (SUCCEEDED(m_dxgiFactory->QueryInterface(IID_PPV_ARGS(&spDxgiFactory7))))
					{
						m_adapterChangeEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
						if (m_adapterChangeEvent == nullptr)
						{
							ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
						}
						ThrowIfFailed(spDxgiFactory7->RegisterAdaptersChangedEvent(m_adapterChangeEvent, &m_adapterChangeRegistrationCookie));
					}
#endif

					// Check if the application should switch to a different adapter.
					ThrowIfFailed(ValidateActiveAdapter());
				}

				// UILayer will transition backbuffer to a present state.
				bool bSetBackbufferReadyForPresent = !m_enableUI;
				m_scene->Render(m_commandQueue.Get(), bSetBackbufferReadyForPresent);

				if (m_enableUI)
				{
					m_uiLayer->Render(m_frameIndex);
				}

				// Present and update the frame index for the next frame.
				// When using sync interval 0, it is recommended to always pass the tearing flag when it is supported.
				ThrowIfFailed(m_swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING));

				MoveToNextFrame();
			}
			catch (HrException& e)
			{
				if (e.Error() == DXGI_ERROR_DEVICE_REMOVED || e.Error() == DXGI_ERROR_DEVICE_RESET)
				{
					RecreateD3Dresources();
				}
				else
				{
					throw;
				}
			}
		}
	}

	void BaseDevice::SetToBefore() {
		m_scene->SetToBefore();
	}

	// Tears down D3D resources and reinitializes them.
	void BaseDevice::RecreateD3Dresources()
	{
		// Give GPU a chance to finish its execution in progress.
		try
		{
			WaitForGpu(m_commandQueue.Get());
		}
		catch (HrException&)
		{
			// Do nothing, currently attached adapter is unresponsive.
		}
		ReleaseD3DObjects();
		OnInit();
	}

	void BaseDevice::OnDestroy()
	{
		// Ensure that the GPU is no longer referencing resources that are about to be
		// cleaned up by the destructor.
		try
		{
			m_scene->Destroy();
			WaitForGpu(m_commandQueue.Get());
		}
		catch (HrException&)
		{
			// Do nothing, currently attached adapter is unresponsive.
		}
		CloseHandle(m_fenceEvent);
		if (m_adapterChangeEvent)
		{
			CloseHandle(m_adapterChangeEvent);
		}
	}

	void BaseDevice::SelectAdapter(UINT index)
	{
		if (index != m_activeAdapter && index < m_gpuAdapterDescs.size() && m_gpuAdapterDescs[index].supportsDx12FL11)
		{
			m_activeAdapter = index;
			RecreateD3Dresources();
		}
	}

	// Looks up an adapter index for a given adapter luid.
	// Returns false if no such adapter is currently enumerated.
	bool BaseDevice::RetrieveAdapterIndex(UINT* adapterIndex, LUID adapterLuid)
	{
		for (UINT i = 0; i < m_gpuAdapterDescs.size(); i++)
		{
			if (memcmp(&m_gpuAdapterDescs[i].desc.AdapterLuid, &adapterLuid, sizeof(adapterLuid)) == 0)
			{
				*adapterIndex = i;
				return true;
			}
		}
		return false;
	}

	void BaseDevice::SelectGPUPreference(UINT index)
	{
		if (static_cast<DXGI_GPU_PREFERENCE>(index) != m_activeGpuPreference && index < m_gpuPreferenceToName.size())
		{
			// Regenerate adapter list with the new preference mode selected.
			m_activeGpuPreference = static_cast<DXGI_GPU_PREFERENCE>(index);

			// Verify the active adapter still meets the new gpu preference setting. 
			if (FAILED(ValidateActiveAdapter()))
			{
				// It does not, recreate the resources on a matching adapter.
				RecreateD3Dresources();
			}
		}
	}

	// Rechecks active adapter availability and it matching application settings.
	// Returns an error, if the adapter is no longer valid.
	HRESULT BaseDevice::ValidateActiveAdapter()
	{
		EnumerateGPUadapters();

		if (!RetrieveAdapterIndex(&m_activeAdapter, m_activeAdapterLuid))
		{
			// The adapter is no longer available, default to 0.
			m_activeAdapter = 0;
			return DXGI_ERROR_DEVICE_RESET;
		}

		// Enforce adapter 0 being used, unless manual adapter selection is enabled.
		if (!m_manualAdapterSelection && m_activeAdapter != 0)
		{
			// A different adapter has become adapter 0, switch to it.
			m_activeAdapter = 0;
			return DXGI_ERROR_DEVICE_RESET;
		}

		return S_OK;
	}

	// Returns whether there have been adapter enumeration changes in the system
	bool BaseDevice::QueryForAdapterEnumerationChanges()
	{
		bool bChangeInAdapterEnumeration = false;
		if (m_adapterChangeEvent)
		{
#ifdef USE_DXGI_1_6
			// If QueryInterface for IDXGIFactory7 succeeded, then use RegisterAdaptersChangedEvent notifications.
			DWORD waitResult = WaitForSingleObject(m_adapterChangeEvent, 0);
			bChangeInAdapterEnumeration = (waitResult == WAIT_OBJECT_0);

			if (bChangeInAdapterEnumeration)
			{
				// Before recreating the factory, unregister the adapter event
				ComPtr<IDXGIFactory7> spDxgiFactory7;
				if (SUCCEEDED(m_dxgiFactory->QueryInterface(IID_PPV_ARGS(&spDxgiFactory7))))
				{
					ThrowIfFailed(spDxgiFactory7->UnregisterAdaptersChangedEvent(m_adapterChangeRegistrationCookie));
					m_adapterChangeRegistrationCookie = 0;
					CloseHandle(m_adapterChangeEvent);
					m_adapterChangeEvent = NULL;
				}
			}
#endif
		}
		else
		{
			// Otherwise, IDXGIFactory7 doesn't exist, so continue using the polling solution of IsCurrent.
			bChangeInAdapterEnumeration = !m_dxgiFactory->IsCurrent();
		}

		return bChangeInAdapterEnumeration;
	}

	// Retrieves information about available GPU adapters.
	void BaseDevice::EnumerateGPUadapters()
	{
		m_gpuAdapterDescs.clear();

		ComPtr<IDXGIAdapter1> adapter;
#ifdef USE_DXGI_1_6
		for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != m_dxgiFactory->EnumAdapterByGpuPreference(adapterIndex, m_activeGpuPreference, IID_PPV_ARGS(&adapter)); ++adapterIndex)
#else
		for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != m_dxgiFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
#endif
		{
			DxgiAdapterInfo adapterInfo;
			ThrowIfFailed(adapter->GetDesc1(&adapterInfo.desc));
			adapterInfo.supportsDx12FL11 = SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr));
			m_gpuAdapterDescs.push_back(std::move(adapterInfo));
		}
	}

	// Acquire selected adapter.
	// If adapter cannot be found, *ppAdapter will be set to nullptr.
	void BaseDevice::GetGPUAdapter(
		UINT adapterIndex,
		IDXGIAdapter1** ppAdapter)
	{
		ComPtr<IDXGIAdapter1> adapter;
		*ppAdapter = nullptr;
#ifdef USE_DXGI_1_6
		if (m_dxgiFactory->EnumAdapterByGpuPreference(adapterIndex, m_activeGpuPreference, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND)
#else
		if (m_dxgiFactory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND)
#endif
		{
			DXGI_ADAPTER_DESC1 desc;
			ThrowIfFailed(adapter->GetDesc1(&desc));

			// Check to see if the adapter supports Direct3D 12.
			ThrowIfFailed(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr));

			*ppAdapter = adapter.Detach();
		}
	}

	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.
	void BaseDevice::CalculateFrameStats()
	{
		static int frameCnt = 0;
		static double elapsedTime = 0.0f;
		double totalTime = m_timer.GetTotalSeconds();
		frameCnt++;

		// Compute averages over one second period.
		if ((totalTime - elapsedTime) >= 1.0f)
		{
			float diff = static_cast<float>(totalTime - elapsedTime);
			m_fps = static_cast<float>(frameCnt) / diff; // Normalize to an exact second.
			if (m_fps > 0.0f) {
				m_elapsedTime = 1.0 / (double)m_fps;
			}
			frameCnt = 0;
			elapsedTime = totalTime;

			// Display FPS on the window title bar if UI is disabled.
			if (!m_enableUI)
			{
				std::wstring fpsStr = std::to_wstring(m_fps);
				std::wstring windowText = L"	fps: " + fpsStr;
				SetCustomWindowText(windowText.c_str());
			}
		}
	}

	// Wait for pending GPU work to complete.
	void BaseDevice::WaitForGpu(ID3D12CommandQueue* pCommandQueue)
	{
		// Schedule a Signal command in the queue.
		ThrowIfFailed(pCommandQueue->Signal(m_fence.Get(), m_fenceValues[m_frameIndex]));

		// Wait until the fence has been processed.
		ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
		WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

		// Increment the fence value for the current frame.
		m_fenceValues[m_frameIndex]++;
	}

	// Prepare to render the next frame.
	void BaseDevice::MoveToNextFrame()
	{
		// Schedule a Signal command in the queue.
		const UINT64 currentFenceValue = m_fenceValues[m_frameIndex];
		ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), currentFenceValue));

		// Update the frame index.
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		// If the next frame is not ready to be rendered yet, wait until it is ready.
		if (m_fence->GetCompletedValue() < m_fenceValues[m_frameIndex])
		{
			ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
			WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
		}
		m_scene->SetFrameIndex(m_frameIndex);

		// Set the fence value for the next frame.
		m_fenceValues[m_frameIndex] = currentFenceValue + 1;
	}
}
// end namespace basecross


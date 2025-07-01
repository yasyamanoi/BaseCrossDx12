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
@file PrimDevice.cpp
@brief デバイス親クラス　実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"

namespace basecross {

	using namespace Microsoft::WRL;
	

	PrimDevice::PrimDevice(UINT width, UINT height, std::wstring name) :
		m_width(width),
		m_height(height),
		m_windowBounds{ 0,0,0,0 },
		m_title(name),
		m_aspectRatio(0.0f),
		m_quiteEscapeKey(true),
		m_useWarpDevice(false),
		m_enableUI(true)
	{
		WCHAR assetsPath[512];
		GetAssetsPath(assetsPath, _countof(assetsPath));
		m_assetsPath = assetsPath;

		UpdateForSizeChange(width, height);
		CheckTearingSupport();
	}

	PrimDevice::~PrimDevice()
	{
	}

	void PrimDevice::UpdateForSizeChange(UINT clientWidth, UINT clientHeight)
	{
		m_width = clientWidth;
		m_height = clientHeight;
		m_aspectRatio = static_cast<float>(clientWidth) / static_cast<float>(clientHeight);
	}

	// Helper function for resolving the full path of assets.
	std::wstring PrimDevice::GetAssetFullPath(LPCWSTR assetName)
	{
		return m_assetsPath + assetName;
	}

	// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
	// If no such adapter can be found, *ppAdapter will be set to nullptr.
	_Use_decl_annotations_
		void PrimDevice::GetHardwareAdapter(
			IDXGIFactory1* pFactory,
			IDXGIAdapter1** ppAdapter,
			bool requestHighPerformanceAdapter)
	{
		*ppAdapter = nullptr;

		ComPtr<IDXGIAdapter1> adapter;

		ComPtr<IDXGIFactory6> factory6;
		if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
		{
			for (
				UINT adapterIndex = 0;
				SUCCEEDED(factory6->EnumAdapterByGpuPreference(
					adapterIndex,
					requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
					IID_PPV_ARGS(&adapter)));
					++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					// Don't select the Basic Render Driver adapter.
					// If you want a software adapter, pass in "/warp" on the command line.
					continue;
				}

				// Check to see whether the adapter supports Direct3D 12, but don't create the
				// actual device yet.
				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
				{
					break;
				}
			}
		}

		if (adapter.Get() == nullptr)
		{
			for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					// Don't select the Basic Render Driver adapter.
					// If you want a software adapter, pass in "/warp" on the command line.
					continue;
				}

				// Check to see whether the adapter supports Direct3D 12, but don't create the
				// actual device yet.
				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
				{
					break;
				}
			}
		}

		*ppAdapter = adapter.Detach();
	}

	// Helper function for setting the window's title text.
	void PrimDevice::SetCustomWindowText(LPCWSTR text)
	{
		std::wstring windowText = m_title + L": " + text;
		SetWindowText(App::GetHwnd(), windowText.c_str());
	}

	// Helper function for parsing any supplied command line args.
	_Use_decl_annotations_
		void PrimDevice::ParseCommandLineArgs(WCHAR* argv[], int argc)
	{
		for (int i = 1; i < argc; ++i)
		{
			if (_wcsnicmp(argv[i], L"-warp", wcslen(argv[i])) == 0 ||
				_wcsnicmp(argv[i], L"/warp", wcslen(argv[i])) == 0)
			{
				m_useWarpDevice = true;
				m_title = m_title + L" (WARP)";
			}
			else if (_wcsnicmp(argv[i], L"-disableUI", wcslen(argv[i])) == 0 ||
				_wcsnicmp(argv[i], L"/disableUI", wcslen(argv[i])) == 0)
			{
				m_enableUI = false;
			}
		}
	}

	// Determines whether tearing support is available for fullscreen borderless windows.
	void PrimDevice::CheckTearingSupport()
	{
		ComPtr<IDXGIFactory6> factory;
		HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
		BOOL allowTearing = FALSE;
		if (SUCCEEDED(hr))
		{
			hr = factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
		}

		m_tearingSupport = SUCCEEDED(hr) && allowTearing;
	}

	void PrimDevice::SetWindowBounds(int left, int top, int right, int bottom)
	{
		m_windowBounds.left = static_cast<LONG>(left);
		m_windowBounds.top = static_cast<LONG>(top);
		m_windowBounds.right = static_cast<LONG>(right);
		m_windowBounds.bottom = static_cast<LONG>(bottom);
	}


	//スレッド用の変数
	static bool g_threadEnded;
	static std::string g_msg;
	//ミューテックス
	static std::mutex g_mtx;

	//メッセージボックス用のスレッド
	static void MsgBoxFunc() {
		g_mtx.lock();
		g_threadEnded = false;
		g_mtx.unlock();

		MessageBoxA(nullptr, g_msg.c_str(), "エラー", MB_OK);
		g_mtx.lock();
		g_threadEnded = true;
		g_mtx.unlock();
	}



	//ランタイムルーチン
	//エラー表示用に、スレッドを新しく切ってメッセージボックスを表示
	void PrimDevice::OnUpdateDraw() {

		int retCode = 0;
		try {
#if defined(_DEBUG)
			static bool loopStart = false;
			if (!loopStart) {
				OutputDebugStringA("loopStart\n");
				loopStart = true;

			}
#endif
			App::GetInputDevice().ResetControlerState();
			OnUpdate();
			OnRender();
			SetToBefore();
		}
		catch (BaseException& e) {
			//デバッグ出力をする。
			std::string str = e.what_m() + "\n";
			OutputDebugStringA(str.c_str());
			//メッセージボックス
			g_msg = e.what_m() + "\n";
			//メッセージボックススレッドのスタート
			std::thread MsgThread(MsgBoxFunc);
			MsgThread.join();
			retCode = 1;
		}
		catch (std::runtime_error& e) {
			//デバッグ出力をする。
			std::string str(e.what());
			str += "\n";
			OutputDebugStringA(str.c_str());
			//メッセージボックス
			g_msg = e.what();
			g_msg += "\n";
			//メッセージボックススレッドのスタート
			std::thread MsgThread(MsgBoxFunc);
			MsgThread.join();
			retCode = 1;
		}
		catch (std::exception& e) {
			//STLエラー
			std::string str(e.what());
			str += "\n";
			OutputDebugStringA(str.c_str());
			//メッセージボックス
			g_msg = e.what();
			g_msg += "\n";
			//メッセージボックススレッドのスタート
			std::thread MsgThread(MsgBoxFunc);
			MsgThread.join();

			retCode = 1;
		}
		catch (...) {
			OutputDebugStringA("原因不明のエラー\n");

			//メッセージボックス
			g_msg = "原因不明のエラー\n";
			//メッセージボックススレッドのスタート
			std::thread MsgThread(MsgBoxFunc);
			MsgThread.join();

			retCode = 1;
		}
		if (retCode) {
			PostQuitMessage(1);
		}
	}



}
// end namespace basecross



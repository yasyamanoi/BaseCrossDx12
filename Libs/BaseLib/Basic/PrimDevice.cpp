/*!
@file PrimDevice.cpp
@brief �v���~�e�B�u�ȃf�o�C�X�N���X
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
*/

#include "stdafx.h"

using namespace Microsoft::WRL;

namespace basecross {

	PrimDevice::PrimDevice(UINT width, UINT height, std::wstring name) :
		m_width(width),
		m_height(height),
		m_windowBounds{ 0,0,0,0 },
		m_title(name),
		m_aspectRatio(0.0f),
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

}
//end namespace basecross 

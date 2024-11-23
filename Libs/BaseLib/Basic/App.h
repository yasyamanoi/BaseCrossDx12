/*!
@file App.h
@brief アプリケーションクラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
*/

#pragma once

#include "stdafx.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	///	アプリケーションクラス(static呼び出しをする)
	//--------------------------------------------------------------------------------------
	class App
	{
		static HWND m_hwnd;
		static bool m_fullscreenMode;
		static const UINT m_windowStyle = WS_OVERLAPPEDWINDOW;
		static RECT m_windowRect;
		static PrimDevice* m_pPrimDevice;

		static std::shared_ptr<EventDispatcher> m_EventDispatcher;

		static std::wstring m_wstrModulePath;
		static std::wstring m_wstrDir;
		static std::wstring m_wstrMediaDir;
		static std::wstring m_wstrShadersDir;
		static std::wstring m_wstrRelativeMediaDir;
		static std::wstring m_wstrRelativeShadersDir;
		static std::wstring	m_wstrRelativeAssetsDir;
		static void SetInitData();
	public:
		static int Run(PrimDevice* pPrimDevice, HINSTANCE hInstance, int nCmdShow);
		static void ToggleFullscreenWindow(IDXGISwapChain* pOutput = nullptr);
		static void SetWindowZorderToTopMost(bool setToTopMost);
		static HWND GetHwnd() { return m_hwnd; }
		static bool IsFullscreen() { return m_fullscreenMode; }

		//ID3D12Deviceの取得
		static ComPtr<ID3D12Device> GetID3D12Device(){
			auto dev = BaseDevice::GetBaseDevice();
			if (!dev) {
				return nullptr;
			}
			return dev->GetID3D12Device();
		}
		//ElapsedTimeを得る
		static double GetElapsedTime() {
			auto dev = BaseDevice::GetBaseDevice();
			if (!dev) {
				return 0.000001;

			}
			return dev->GetElapsedTime();
		}
		//イベントディスパッチャーを得る
		static std::shared_ptr<EventDispatcher> GetEventDispatcher() {
			return m_EventDispatcher;
		}
		//モジュール名フルパスを得る
		static const std::wstring& GetModulePath() { return m_wstrModulePath; }
		//モジュールがあるディレクトリを得る
		static const std::wstring& GetModuleDir() { return m_wstrDir; }
		//絶対パスのメディアディレクトリを得る
		static const std::wstring& GetMediaDir() { return m_wstrMediaDir; }
		//絶対パスのシェーダディレクトリを得る
		static const std::wstring& GetShadersDir() { return m_wstrShadersDir; }
		//相対パスのメディアディレクトリを得る
		static const std::wstring& GetRelativeMediaDir() { return m_wstrRelativeMediaDir; }
		//相対パスのシェーダディレクトリを得る
		static const std::wstring& GetRelativeShadersDir() { return m_wstrRelativeShadersDir; }
		//相対パスのアセットディレクトリを得る
		static const std::wstring& GetRelativeAssetsDir() { return m_wstrRelativeAssetsDir; }
		//ウインドウの幅を得る
		static LONG GetWidth() {
			return m_windowRect.right;
		}
		//ウインドウの高さを得る
		static LONG GetHeight() {
			return m_windowRect.bottom;
		}

	protected:
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	};
}
using namespace basecross;
//namespace basecross


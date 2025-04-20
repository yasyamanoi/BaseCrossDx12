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
@file App.h
@brief アプリケーションクラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#pragma once

#include "stdafx.h"

namespace basecross {


	class PrimDevice;

	class App
	{
	public:
		static int Run(PrimDevice* pPrimDevice, HINSTANCE hInstance, int nCmdShow);
		static void ToggleFullscreenWindow(IDXGISwapChain* pOutput = nullptr);
		static void SetWindowZorderToTopMost(bool setToTopMost);
		static HWND GetHwnd() { return m_hwnd; }
		static bool IsFullscreen() { return m_fullscreenMode; }
		static PrimDevice* GetPrimDevice() { return m_pPrimDevice; }
		static ID3D12Device* GetD3D12Device();
		static ComPtr<ID3D12Device> GetID3D12Device();

		//--------------------------------------------------------------------------------------
		/*!
		@brief	モジュール名フルパスを得る
		@return モジュール名フルパス
		*/
		//--------------------------------------------------------------------------------------
		static const std::wstring& GetModulePath() { return m_wstrModulePath; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	モジュールがあるディレクトリを得る
		@return モジュールがあるディレクトリ
		*/
		//--------------------------------------------------------------------------------------
		static const std::wstring& GetModuleDir() { return m_wstrDir; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	絶対パスのメディアディレクトリを得る
		@return 絶対パスのメディアディレクトリ
		*/
		//--------------------------------------------------------------------------------------
		static const std::wstring& GetMediaDir() { return m_wstrMediaDir; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	絶対パスのシェーダディレクトリを得る
		@return 絶対パスのシェーダディレクトリ
		*/
		//--------------------------------------------------------------------------------------
		static const std::wstring& GetShadersDir() { return m_wstrShadersDir; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	相対パスのメディアディレクトリを得る
		@return 相対パスのメディアディレクトリ
		*/
		//--------------------------------------------------------------------------------------
		static const std::wstring& GetRelativeMediaDir() { return m_wstrRelativeMediaDir; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	相対パスのシェーダディレクトリを得る
		@return 相対パスのシェーダディレクトリ
		*/
		//--------------------------------------------------------------------------------------
		static const std::wstring& GetRelativeShadersDir() { return m_wstrRelativeShadersDir; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	相対パスのアセットディレクトリを得る
		@return 相対パスのアセットディレクトリ
		*/
		//--------------------------------------------------------------------------------------
		static const std::wstring& GetRelativeAssetsDir() { return m_wstrRelativeAssetsDir; }



	protected:
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		static HWND m_hwnd;
		static bool m_fullscreenMode;
		static const UINT m_windowStyle = WS_OVERLAPPEDWINDOW;
		static RECT m_windowRect;
		static PrimDevice* m_pPrimDevice;

		static std::wstring m_wstrModulePath;		///< モジュール名フルパス
		static std::wstring m_wstrDir;				///< モジュールがあるディレクトリ
		static std::wstring m_wstrMediaDir;			///< 絶対パスのメディアディレクトリ
		static std::wstring m_wstrShadersDir;		///< 絶対パスのシェーダディレクトリ
		static std::wstring m_wstrRelativeMediaDir;	///< 相対パスのメディアディレクトリ
		static std::wstring m_wstrRelativeShadersDir;	///< 相対パスのシェーダディレクトリ
		static std::wstring	m_wstrRelativeAssetsDir;	///< 相対パスのアセットディレクトリ

		static void SetInitData();


	};
}
// end namespace basecross



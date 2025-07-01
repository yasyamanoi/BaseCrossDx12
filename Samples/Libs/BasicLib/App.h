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

	//--------------------------------------------------------------------------------------
	///	アプリケーションクラス
	//--------------------------------------------------------------------------------------
	class App
	{
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	アプリケーションの実行
		@param[in]	pPrimDevice	基本デバイス
		@param[in]	hInstance	インスタンスのハンドル
		@param[in]	nCmdShow	初期表示状態
		@return	実行結果
		*/
		//--------------------------------------------------------------------------------------
		static int Run(PrimDevice* pPrimDevice, HINSTANCE hInstance, int nCmdShow);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	フルスクリーンとウィンドウの切り替え
		@param[in]	pOutput	スワップチェーンのポインタ
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		static void ToggleFullscreenWindow(IDXGISwapChain* pOutput = nullptr);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Zオーダーを変更する
		@param[in]	setToTopMost	TopMostにするかどうか
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		static void SetWindowZorderToTopMost(bool setToTopMost);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Windowのハンドルを得る
		@return	Windowのハンドル
		*/
		//--------------------------------------------------------------------------------------
		static HWND GetHwnd() { return m_hwnd; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	フルスクリーンかどうか
		@return	フルスクリーンならtrue
		*/
		//--------------------------------------------------------------------------------------
		static bool IsFullscreen() { return m_fullscreenMode; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	基本デバイスを得る
		@return	基本デバイスのポインタ
		*/
		//--------------------------------------------------------------------------------------
		static PrimDevice* GetPrimDevice() { return m_pPrimDevice; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ID3D12Deviceを得る
		@return	ID3D12Deviceのポインタ
		*/
		//--------------------------------------------------------------------------------------
		static ID3D12Device* GetD3D12Device();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ComPtrとしてのID3D12Deviceを得る
		@return	ID3D12DeviceのComPtr
		*/
		//--------------------------------------------------------------------------------------
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

		//--------------------------------------------------------------------------------------
		/*!
		@brief	入力デバイスを得る
		@return 入力デバイス
		*/
		//--------------------------------------------------------------------------------------
		static InputDevice& GetInputDevice() {
			return m_inputDevice;
		}
	protected:
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	private:
		static HWND m_hwnd;
		static bool m_fullscreenMode;
		static const UINT m_windowStyle = WS_OVERLAPPEDWINDOW;
		static RECT m_windowRect;
		static PrimDevice* m_pPrimDevice;
		static InputDevice m_inputDevice;

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



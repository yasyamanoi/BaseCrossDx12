/*!
@file App.h
@brief アプリケーションクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	アプリケーションクラス
	//--------------------------------------------------------------------------------------
	class App
	{
		static HWND m_hwnd;
		static BaseScene* m_pBaseScene;

		static wstring m_wstrModulePath;		///< モジュール名フルパス
		static wstring m_wstrDir;				///< モジュールがあるディレクトリ
		static wstring m_wstrMediaDir;			///< 絶対パスのメディアディレクトリ
		static wstring m_wstrShadersDir;		///< 絶対パスのシェーダディレクトリ
		static wstring m_wstrRelativeMediaDir;	///< 相対パスのメディアディレクトリ
		static wstring m_wstrRelativeShadersDir;	///< 相対パスのシェーダディレクトリ
		static wstring	m_wstrRelativeAssetsDir;	///< 相対パスのアセットディレクトリ

		static void SetAssetsPath();
	protected:
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	アプリケーションクラスの実行
		@param[in]	pBaseDevice	デバイスクラス
		@param[in]	pBaseScene	シーンの親クラス
		@param[in]	hInstance	インスタンスのハンドル
		@param[in]	nCmdShow	表示モード
		@return アプリケーションの終了コード
		*/
		//--------------------------------------------------------------------------------------
		static int Run(BaseDevice* pBaseDevice, BaseScene* pBaseScene, HINSTANCE hInstance, int nCmdShow);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ウインドウのハンドルを得る
		@return ウインドウのハンドル
		*/
		//--------------------------------------------------------------------------------------
		static HWND GetHwnd() { return m_hwnd; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	シーンの親クラスを得る
		@return シーンの親クラス
		*/
		//--------------------------------------------------------------------------------------
		static BaseScene* GetBaseScene() { return m_pBaseScene; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デバイスクラスを得る
		@return デバイスクラス
		*/
		//--------------------------------------------------------------------------------------
		static BaseDevice* GetBaseDevice() {
			return reinterpret_cast<BaseDevice*>(GetWindowLongPtr(App::GetHwnd(), GWLP_USERDATA));
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ID3D12Deviceを得る
		@return ID3D12DeviceのComPtr
		*/
		//--------------------------------------------------------------------------------------
		static ComPtr<ID3D12Device> GetID3D12Device() {
			return GetBaseDevice()->GetID3D12Device();
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ElapsedTimeを得る
		@return ElapsedTime
		*/
		//--------------------------------------------------------------------------------------
		static float GetElapsedTime() {
			return (float)GetBaseDevice()->GetElapsedTime();
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	モジュール名フルパスを得る
		@return モジュール名フルパス
		*/
		//--------------------------------------------------------------------------------------
		static const wstring& GetModulePath() { return m_wstrModulePath; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	モジュールがあるディレクトリを得る
		@return モジュールがあるディレクトリ
		*/
		//--------------------------------------------------------------------------------------
		static const wstring& GetModuleDir() { return m_wstrDir; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	絶対パスのメディアディレクトリを得る
		@return 絶対パスのメディアディレクトリ
		*/
		//--------------------------------------------------------------------------------------
		static const wstring& GetMediaDir() { return m_wstrMediaDir; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	絶対パスのシェーダディレクトリを得る
		@return 絶対パスのシェーダディレクトリ
		*/
		//--------------------------------------------------------------------------------------
		static const wstring& GetShadersDir() { return m_wstrShadersDir; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	相対パスのメディアディレクトリを得る
		@return 相対パスのメディアディレクトリ
		*/
		//--------------------------------------------------------------------------------------
		static const wstring& GetRelativeMediaDir() { return m_wstrRelativeMediaDir; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	相対パスのシェーダディレクトリを得る
		@return 相対パスのシェーダディレクトリ
		*/
		//--------------------------------------------------------------------------------------
		static const wstring& GetRelativeShadersDir() { return m_wstrRelativeShadersDir; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	相対パスのアセットディレクトリを得る
		@return 相対パスのアセットディレクトリ
		*/
		//--------------------------------------------------------------------------------------
		static const wstring& GetRelativeAssetsDir() { return m_wstrRelativeAssetsDir; }


	};
}
// end namespace basecross
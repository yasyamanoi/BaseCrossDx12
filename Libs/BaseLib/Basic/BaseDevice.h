/*!
@file BaseDevice.h
@brief デバイス親クラス
@copyright Copyright (c) 2021 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	デバイス親クラス
	//--------------------------------------------------------------------------------------
	class BaseDevice
	{
	public:

		virtual void OnInit() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnRender() = 0;
		virtual void OnDestroy() = 0;

		virtual void OnKeyDown(UINT8 key) = 0;
		virtual void OnKeyUp(UINT8 key) = 0;

		UINT GetWidth() const { return m_width; }
		UINT GetHeight() const { return m_height; }
		const WCHAR* GetTitle() const { return m_title.c_str(); }
		bool IsQuiteEscapeKey() const {
			return m_QuiteEscapeKey;
		}
		void SetQuiteEscapeKey(bool b) {
			m_QuiteEscapeKey = b;
		}
		void ParseCommandLineArgs(_In_reads_(argc) WCHAR* argv[], int argc);


	protected:
		BaseDevice(UINT width, UINT height, std::wstring name);
		virtual ~BaseDevice();

		std::wstring GetAssetFullPath(LPCWSTR assetName);

		void GetHardwareAdapter(
			_In_ IDXGIFactory1* pFactory,
			_Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
			bool requestHighPerformanceAdapter = false);
		void SetCustomWindowText(LPCWSTR text);
		UINT m_width;
		UINT m_height;
		float m_aspectRatio;
		//ESCキーで終了させるかどうか
		bool m_QuiteEscapeKey;
		//ラップモードかどうか
		bool m_useWarpDevice;
	private:
		std::wstring m_assetsPath;
		std::wstring m_title;
	};
}
// end namespace basecross



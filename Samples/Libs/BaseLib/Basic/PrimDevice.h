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
@file PrimDevice.h
@brief デバイス親クラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#pragma once

#include "stdafx.h"
namespace basecross {


	class PrimDevice
	{
	public:
		PrimDevice(UINT width, UINT height, std::wstring name);
		virtual ~PrimDevice();

		virtual void OnInit() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnRender() = 0;
		virtual void SetToBefore() {}
		virtual void OnUpdateDraw();
		virtual void OnSizeChanged(UINT width, UINT height, bool minimized) = 0;
		virtual void OnDestroy() = 0;

		// Samples override the event handlers to handle specific messages.
		virtual void OnKeyDown(UINT8 /*key*/) {}
		virtual void OnKeyUp(UINT8 /*key*/) {}
		virtual void OnWindowMoved(int /*x*/, int /*y*/) {}
		virtual void OnMouseMove(UINT /*x*/, UINT /*y*/) {}
		virtual void OnLeftButtonDown(UINT /*x*/, UINT /*y*/) {}
		virtual void OnLeftButtonUp(UINT /*x*/, UINT /*y*/) {}
		virtual void OnDisplayChanged() {}
		// Accessors.
		UINT GetWidth() const { return m_width; }
		UINT GetHeight() const { return m_height; }
		const WCHAR* GetTitle() const { return m_title.c_str(); }
		bool GetTearingSupport() const { return m_tearingSupport; }
		RECT GetWindowsBounds() const { return m_windowBounds; }
		virtual IDXGISwapChain* GetSwapchain() { return nullptr; }
		virtual ID3D12Device* GetD3D12Device() { return nullptr; }
		virtual ComPtr<ID3D12Device> GetID3D12Device() { return nullptr; }
		bool IsQuiteEscapeKey() const { return m_quiteEscapeKey; }
		void SetQuiteEscapeKey(bool b) { m_quiteEscapeKey = b; }



		void ParseCommandLineArgs(_In_reads_(argc) WCHAR* argv[], int argc);
		void UpdateForSizeChange(UINT clientWidth, UINT clientHeight);
		void SetWindowBounds(int left, int top, int right, int bottom);
		std::wstring GetAssetFullPath(LPCWSTR assetName);

	protected:
		void GetHardwareAdapter(
			_In_ IDXGIFactory1* pFactory,
			_Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
			bool requestHighPerformanceAdapter = false);

		void SetCustomWindowText(LPCWSTR text);
		void CheckTearingSupport();

		// Viewport dimensions.
		UINT m_width;
		UINT m_height;
		float m_aspectRatio;

		//ESCキーで終了させるかどうか
		bool m_quiteEscapeKey;


		// Window bounds
		RECT m_windowBounds;

		// Whether or not tearing is available for fullscreen borderless windowed mode.
		bool m_tearingSupport;

		// Adapter info.
		bool m_useWarpDevice;

		// Override to be able to start without Dx11on12 UI for PIX. PIX doesn't support 11 on 12. 
		bool m_enableUI;

	private:
		// Root assets path.
		std::wstring m_assetsPath;

		// Window title.
		std::wstring m_title;
	};
}
// end namespace basecross

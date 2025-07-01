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
@file App.cpp
@brief アプリケーションクラス　実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"

namespace basecross {


	HWND App::m_hwnd = nullptr;
	bool App::m_fullscreenMode = false;
	RECT App::m_windowRect;
	PrimDevice* App::m_pPrimDevice = nullptr;
	using Microsoft::WRL::ComPtr;

	std::wstring App::m_wstrModulePath;		///< モジュール名フルパス
	std::wstring App::m_wstrDir;				///< モジュールがあるディレクトリ
	std::wstring App::m_wstrMediaDir;			///< 絶対パスのメディアディレクトリ
	std::wstring App::m_wstrShadersDir;		///< 絶対パスのシェーダディレクトリ
	std::wstring App::m_wstrRelativeMediaDir;	///< 相対パスのメディアディレクトリ
	std::wstring App::m_wstrRelativeShadersDir;	///< 相対パスのシェーダディレクトリ
	std::wstring App::m_wstrRelativeAssetsDir;	///< 相対パスのアセットディレクトリ

	InputDevice App::m_inputDevice;


	void App::SetInitData() {

		//基準ディレクトリの設定
		//相対パスにすると、ファイルダイアログでカレントパスが狂うので
		//絶対パス指定
		wchar_t Modulebuff[MAX_PATH];
		wchar_t Drivebuff[_MAX_DRIVE];
		wchar_t Dirbuff[_MAX_DIR];
		wchar_t FileNamebuff[_MAX_FNAME];
		wchar_t Extbuff[_MAX_EXT];

		::ZeroMemory(Modulebuff, sizeof(Modulebuff));
		::ZeroMemory(Drivebuff, sizeof(Drivebuff));
		::ZeroMemory(Dirbuff, sizeof(Dirbuff));
		::ZeroMemory(FileNamebuff, sizeof(FileNamebuff));
		::ZeroMemory(Extbuff, sizeof(Extbuff));

		//モジュール名（プログラムファイル名）を得る
		if (!::GetModuleFileName(nullptr, Modulebuff, sizeof(Modulebuff))) {
			throw std::runtime_error("モジュールが取得できません。");
		}
		m_wstrModulePath = Modulebuff;
		//モジュール名から、各ブロックに分ける
		_wsplitpath_s(Modulebuff,
			Drivebuff, _MAX_DRIVE,
			Dirbuff, _MAX_DIR,
			FileNamebuff, _MAX_FNAME,
			Extbuff, _MAX_EXT);

		//ドライブ名の取得
		m_wstrDir = Drivebuff;
		//ディレクトリ名の取得
		m_wstrDir += Dirbuff;
		//mediaディレクトリを探す
		m_wstrMediaDir = m_wstrDir;
		m_wstrMediaDir += L"media";
		//まず、実行ファイルと同じディレクトリを探す
		DWORD RetCode;
		RetCode = GetFileAttributes(m_wstrMediaDir.c_str());
		if (RetCode == 0xFFFFFFFF) {
			//失敗した
			m_wstrMediaDir = m_wstrDir;
			m_wstrMediaDir += L"..\\media";
			RetCode = GetFileAttributes(m_wstrMediaDir.c_str());
			if (RetCode == 0xFFFFFFFF) {
				//再び失敗した
				throw std::runtime_error("mediaディレクトリを確認できません。");
			}
			else {
				m_wstrMediaDir += L"\\";
				//相対パスの設定
				m_wstrRelativeMediaDir = L"..\\media\\";
			}
		}
		else {
			m_wstrMediaDir += L"\\";
			//相対パスの設定
			m_wstrRelativeMediaDir = L"media\\";
		}
		m_wstrShadersDir = m_wstrMediaDir + L"Shaders\\";
		m_wstrRelativeShadersDir = m_wstrRelativeMediaDir + L"Shaders\\";
		//Assetsディレクトリを探す
		m_wstrRelativeAssetsDir = L"..\\..\\Assets";
		//相対ディレクトリを探す
		RetCode = GetFileAttributes(m_wstrRelativeAssetsDir.c_str());
		if (RetCode == 0xFFFFFFFF) {
			//失敗した
			//アセットディレクトリをメディアディレクトリにする
			m_wstrRelativeAssetsDir = m_wstrRelativeMediaDir;
		}
		else {
			//成功した
			m_wstrRelativeAssetsDir += L"\\";
		}
		//乱数の初期化
		srand((unsigned)time(nullptr));

	}

	bool primDeviceActive = false;

	int App::Run(PrimDevice* pPrimDevice, HINSTANCE hInstance, int nCmdShow)
	{
		//終了コード
		int retCode = 0;
		//ウインドウ情報。メッセージボックス表示チェックに使用
		WINDOWINFO winInfo;
		ZeroMemory(&winInfo, sizeof(winInfo));
		try
		{
			// デバッグ時、deleteもれのチェック用
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
			//ロケールの設定
			setlocale(LC_ALL, "JPN");
			// Parse the command line parameters
			int argc;
			LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
			pPrimDevice->ParseCommandLineArgs(argv, argc);
			LocalFree(argv);
			m_pPrimDevice = pPrimDevice;
			// Initialize the window class.
			WNDCLASSEX windowClass = { 0 };
			windowClass.cbSize = sizeof(WNDCLASSEX);
			windowClass.style = CS_HREDRAW | CS_VREDRAW;
			windowClass.lpfnWndProc = WindowProc;
			windowClass.hInstance = hInstance;
			windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
			windowClass.lpszClassName = L"BaseCrossClass";
			RegisterClassEx(&windowClass);

			RECT windowRect = { 0, 0, static_cast<LONG>(pPrimDevice->GetWidth()), static_cast<LONG>(pPrimDevice->GetHeight()) };
			AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

			// Create the window and store a handle to it.
			m_hwnd = CreateWindow(
				windowClass.lpszClassName,
				pPrimDevice->GetTitle(),
				m_windowStyle,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				windowRect.right - windowRect.left,
				windowRect.bottom - windowRect.top,
				nullptr,		// We have no parent window.
				nullptr,		// We aren't using menus.
				hInstance,
				pPrimDevice);
			SetInitData();
			//COMの初期化
			//サウンドなどで使用する
			if (FAILED(::CoInitialize(nullptr))) {
				// 初期化失敗
				throw std::exception("Com初期化に失敗しました。");
			}

			// Initialize the sample. OnInit is defined in each child-implementation of DXSample.
			pPrimDevice->OnInit();
			primDeviceActive = true;
			ShowWindow(m_hwnd, nCmdShow);

			MSG msg = {};
			while (msg.message != WM_QUIT)
			{
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

			}
			pPrimDevice->OnDestroy();
			retCode = static_cast<char>(msg.wParam);
		}
		catch (BaseException& e) {
			//デバッグ出力をする。
			std::string str = e.what_m() + "\n";
			OutputDebugStringA(str.c_str());
			if (primDeviceActive) {
				pPrimDevice->OnDestroy();
			}
			if (GetWindowInfo(m_hwnd, &winInfo)) {
				//実行失敗した
				MessageBoxA(m_hwnd, e.what_m().c_str(), "エラー", MB_OK);
			}
			else {
				//実行失敗した
				MessageBoxA(nullptr, e.what_m().c_str(), "エラー", MB_OK);
			}
			retCode = 1;
		}
		catch (std::runtime_error& e) {
			//デバッグ出力をする。
			std::string str(e.what());
			str += "\n";
			OutputDebugStringA(str.c_str());
			if (primDeviceActive) {
				pPrimDevice->OnDestroy();
			}
			if (GetWindowInfo(m_hwnd, &winInfo)) {
				//実行失敗した
				MessageBoxA(m_hwnd, e.what(), "エラー", MB_OK);
			}
			else {
				//実行失敗した
				MessageBoxA(nullptr, e.what(), "エラー", MB_OK);
			}
			retCode = 1;
		}
		catch (std::exception& e) {
			//デバッグ出力をする。
			std::string str(e.what());
			str += "\n";
			OutputDebugStringA(str.c_str());
			if (primDeviceActive) {
				pPrimDevice->OnDestroy();
			}
			//STLエラー
			//マルチバイトバージョンのメッセージボックスを呼ぶ
			if (GetWindowInfo(m_hwnd, &winInfo)) {
				MessageBoxA(m_hwnd, e.what(), "エラー", MB_OK);
			}
			else {
				MessageBoxA(nullptr, e.what(), "エラー", MB_OK);
			}
			retCode = 1;
		}
		catch (...) {
			//デバッグ出力をする。
			OutputDebugStringA("原因不明のエラー\n");
			if (primDeviceActive) {
				pPrimDevice->OnDestroy();
			}
			//原因不明失敗した
			if (GetWindowInfo(m_hwnd, &winInfo)) {
				MessageBox(m_hwnd, L"原因不明のエラーです", L"エラー", MB_OK);
			}
			else {
				MessageBox(nullptr, L"原因不明のエラーです", L"エラー", MB_OK);
			}
			retCode = 1;
		}
		//例外処理終了
		//COMのリリース
		::CoUninitialize();
		if (retCode) {
			PostQuitMessage(0);
		}

		return retCode;
	}

	ID3D12Device* App::GetD3D12Device() { 
		return m_pPrimDevice->GetD3D12Device(); 
	}
	ComPtr<ID3D12Device> App::GetID3D12Device() {
		return m_pPrimDevice->GetID3D12Device();
	}

	// Convert a styled window into a fullscreen borderless window and back again.
	void App::ToggleFullscreenWindow(IDXGISwapChain* pSwapChain)
	{
		if (m_fullscreenMode)
		{
			// Restore the window's attributes and size.
			SetWindowLong(m_hwnd, GWL_STYLE, m_windowStyle);

			SetWindowPos(
				m_hwnd,
				HWND_NOTOPMOST,
				m_windowRect.left,
				m_windowRect.top,
				m_windowRect.right - m_windowRect.left,
				m_windowRect.bottom - m_windowRect.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);

			ShowWindow(m_hwnd, SW_NORMAL);
		}
		else
		{
			// Save the old window rect so we can restore it when exiting fullscreen mode.
			GetWindowRect(m_hwnd, &m_windowRect);

			// Make the window borderless so that the client area can fill the screen.
			SetWindowLong(m_hwnd, GWL_STYLE, m_windowStyle & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));

			RECT fullscreenWindowRect;
			try
			{
				if (pSwapChain)
				{
					// Get the settings of the display on which the app's window is currently displayed
					ComPtr<IDXGIOutput> pOutput;
					ThrowIfFailed(pSwapChain->GetContainingOutput(&pOutput));
					DXGI_OUTPUT_DESC Desc;
					ThrowIfFailed(pOutput->GetDesc(&Desc));
					fullscreenWindowRect = Desc.DesktopCoordinates;
				}
				else
				{
					// Fallback to EnumDisplaySettings implementation
					throw HrException(S_FALSE);
				}
			}
			catch (HrException& e)
			{
				UNREFERENCED_PARAMETER(e);

				// Get the settings of the primary display
				DEVMODE devMode = {};
				devMode.dmSize = sizeof(DEVMODE);
				EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devMode);

				fullscreenWindowRect = {
					devMode.dmPosition.x,
					devMode.dmPosition.y,
					devMode.dmPosition.x + static_cast<LONG>(devMode.dmPelsWidth),
					devMode.dmPosition.y + static_cast<LONG>(devMode.dmPelsHeight)
				};
			}

			SetWindowPos(
				m_hwnd,
				HWND_TOPMOST,
				fullscreenWindowRect.left,
				fullscreenWindowRect.top,
				fullscreenWindowRect.right,
				fullscreenWindowRect.bottom,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);


			ShowWindow(m_hwnd, SW_MAXIMIZE);
		}

		m_fullscreenMode = !m_fullscreenMode;
	}

	void App::SetWindowZorderToTopMost(bool setToTopMost)
	{
		RECT windowRect;
		GetWindowRect(m_hwnd, &windowRect);

		SetWindowPos(
			m_hwnd,
			(setToTopMost) ? HWND_TOPMOST : HWND_NOTOPMOST,
			windowRect.left,
			windowRect.top,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);
	}

	// Main message handler for the sample.
	LRESULT CALLBACK App::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		PrimDevice* pPrimDevice = reinterpret_cast<PrimDevice*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

		switch (message)
		{
		case WM_CREATE:
		{
			// Save the DXSample* passed in to CreateWindow.
			LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
		}
		return 0;

		case WM_KEYDOWN:
			if (pPrimDevice)
			{
				if (pPrimDevice->IsQuiteEscapeKey()) {
					switch (wParam) {
					case VK_ESCAPE:
						//ウインドウを破棄する
						DestroyWindow(hWnd);
						return 0;
					}
				}
				pPrimDevice->OnKeyDown(static_cast<UINT8>(wParam));
			}
			return 0;

		case WM_KEYUP:
			if (pPrimDevice)
			{
				pPrimDevice->OnKeyUp(static_cast<UINT8>(wParam));
			}
			return 0;

		case WM_SYSKEYDOWN:
			// Handle ALT+ENTER:
			if ((wParam == VK_RETURN) && (lParam & (1 << 29)))
			{
				if (pPrimDevice && pPrimDevice->GetTearingSupport())
				{
					ToggleFullscreenWindow(pPrimDevice->GetSwapchain());
					return 0;
				}
			}
			// Send all other WM_SYSKEYDOWN messages to the default WndProc.
			break;

		case WM_PAINT:
			if (pPrimDevice)
			{
				//更新描画処理
				pPrimDevice->OnUpdateDraw();
			}
			return 0;

		case WM_SIZE:
			if (pPrimDevice)
			{
				RECT windowRect = {};
				GetWindowRect(hWnd, &windowRect);
				pPrimDevice->SetWindowBounds(windowRect.left, windowRect.top, windowRect.right, windowRect.bottom);

				RECT clientRect = {};
				GetClientRect(hWnd, &clientRect);
				pPrimDevice->OnSizeChanged(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, wParam == SIZE_MINIMIZED);
			}
			return 0;

		case WM_MOVE:
			if (pPrimDevice)
			{
				RECT windowRect = {};
				GetWindowRect(hWnd, &windowRect);
				pPrimDevice->SetWindowBounds(windowRect.left, windowRect.top, windowRect.right, windowRect.bottom);

				int xPos = (int)(short)LOWORD(lParam);
				int yPos = (int)(short)HIWORD(lParam);
				pPrimDevice->OnWindowMoved(xPos, yPos);
			}
			return 0;

		case WM_DISPLAYCHANGE:
			if (pPrimDevice)
			{
				pPrimDevice->OnDisplayChanged();
			}
			return 0;

		case WM_MOUSEMOVE:
			if (pPrimDevice && static_cast<UINT8>(wParam) == MK_LBUTTON)
			{
				UINT x = LOWORD(lParam);
				UINT y = HIWORD(lParam);
				pPrimDevice->OnMouseMove(x, y);
			}
			return 0;

		case WM_LBUTTONDOWN:
		{
			UINT x = LOWORD(lParam);
			UINT y = HIWORD(lParam);
			pPrimDevice->OnLeftButtonDown(x, y);
		}
		return 0;

		case WM_LBUTTONUP:
		{
			UINT x = LOWORD(lParam);
			UINT y = HIWORD(lParam);
			pPrimDevice->OnLeftButtonUp(x, y);
		}
		return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}

		// Handle any messages the switch statement didn't.
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
// end namespace basecross


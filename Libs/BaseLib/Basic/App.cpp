/*!
@file App.cpp
@brief アプリケーションクラス
@copyright Copyright (c) 2021 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#include "stdafx.h"

namespace basecross {

	HWND App::m_hwnd = nullptr;
	BaseScene* App::m_pBaseScene = nullptr;


	wstring App::m_wstrModulePath;		///< モジュール名フルパス
	wstring App::m_wstrDir;				///< モジュールがあるディレクトリ
	wstring App::m_wstrDataPath;			///< 絶対パスのメディアディレクトリ
	wstring App::m_wstrShadersPath;		///< 絶対パスのシェーダディレクトリ
	wstring App::m_wstrRelativeDataPath;	///< 相対パスのメディアディレクトリ
	wstring App::m_wstrRelativeShadersPath;	///< 相対パスのシェーダディレクトリ
	wstring App::m_wstrRelativeAssetsPath;	///< 相対パスのアセットディレクトリ

	void App::SetAssetsPath() {

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
		    throw runtime_error("モジュールが取得できません。");
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
		m_wstrDataPath = m_wstrDir;
		m_wstrDataPath += L"media";
		//まず、実行ファイルと同じディレクトリを探す
		DWORD RetCode;
		RetCode = GetFileAttributes(m_wstrDataPath.c_str());
		if (RetCode == 0xFFFFFFFF) {
			//失敗した
			m_wstrDataPath = m_wstrDir;
			m_wstrDataPath += L"..\\media";
			RetCode = GetFileAttributes(m_wstrDataPath.c_str());
			if (RetCode == 0xFFFFFFFF) {
				//再び失敗した
				throw runtime_error("mediaディレクトリを確認できません。");
			}
			else {
				m_wstrDataPath += L"\\";
				//相対パスの設定
				m_wstrRelativeDataPath = L"..\\media\\";
			}
		}
		else {
			m_wstrDataPath += L"\\";
			//相対パスの設定
			m_wstrRelativeDataPath = L"media\\";
		}
		m_wstrShadersPath = m_wstrDataPath + L"Shaders\\";
		m_wstrRelativeShadersPath = m_wstrRelativeDataPath + L"Shaders\\";
		//Assetsディレクトリを探す
		m_wstrRelativeAssetsPath = m_wstrDir;
		m_wstrRelativeAssetsPath += L"..\\..\\Assets";
		//相対ディレクトリを探す
		RetCode = GetFileAttributes(m_wstrRelativeAssetsPath.c_str());
		if (RetCode == 0xFFFFFFFF) {
			//失敗した
			//アセットディレクトリをメディアディレクトリにする
			m_wstrRelativeAssetsPath = m_wstrRelativeDataPath;
		}
		else {
			//成功した
			m_wstrRelativeAssetsPath += L"\\";
		}
	}

	int App::Run(BaseDevice* pSample, BaseScene* pBaseScene, HINSTANCE hInstance, int nCmdShow)
	{
		m_pBaseScene = pBaseScene;

		// Parse the command line parameters
		int argc;
		LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
		pSample->ParseCommandLineArgs(argv, argc);
		LocalFree(argv);


		// Initialize the window class.
		WNDCLASSEX windowClass = { 0 };
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WindowProc;
		windowClass.hInstance = hInstance;
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.lpszClassName = L"BaseCrossDx12Class";
		RegisterClassEx(&windowClass);

		RECT windowRect = { 0, 0, static_cast<LONG>(pSample->GetWidth()), static_cast<LONG>(pSample->GetHeight()) };
		AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

		//ウィンドウの作成
		m_hwnd = CreateWindow(
			windowClass.lpszClassName,
			pSample->GetTitle(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			nullptr,
			nullptr,
			hInstance,
			pSample);

		//終了コード
		int retCode = 0;
		//ウインドウ情報。メッセージボックス表示チェックに使用
		WINDOWINFO winInfo;
		ZeroMemory(&winInfo, sizeof(winInfo));
		//例外処理開始
		try {
			SetAssetsPath();
			//COMの初期化
			//サウンドなどで使用する
			if (FAILED(::CoInitialize(nullptr))) {
				// 初期化失敗
				throw exception("Com初期化に失敗しました。");
			}
			pSample->OnInit();
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

			pSample->OnDestroy();
			retCode = static_cast<char>(msg.wParam);
		}
		catch (exception& e) {
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
		return retCode;
	}


    LRESULT CALLBACK App::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
		BaseDevice* pDevice = reinterpret_cast<BaseDevice*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

		switch (message)
		{
		case WM_CREATE:
		{
			LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
		}
		return 0;
		case WM_KEYDOWN:
			if (pDevice)
			{
				if (pDevice->IsQuiteEscapeKey()) {
					switch (wParam) {
					case VK_ESCAPE:
						//ウインドウを破棄する
						DestroyWindow(hWnd);
						return 0;
					}
				}
				pDevice->OnKeyDown(static_cast<UINT8>(wParam));
			}
			return 0;
        case WM_KEYUP:
			if (pDevice)
			{
				pDevice->OnKeyUp(static_cast<UINT8>(wParam));
			}
			return 0;
        case WM_PAINT:
			if (pDevice)
			{
				pDevice->OnUpdate();
				pDevice->OnRender();
			}
			return 0;
        case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}
// end namespace basecross

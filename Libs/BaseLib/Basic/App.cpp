/*!
@file App.cpp
@brief アプリケーションクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/
#include "stdafx.h"

namespace basecross {

	HWND App::m_hwnd = nullptr;

	BaseScene* App::m_pBaseScene = nullptr;
	shared_ptr<EventDispatcher> App::m_eventDispatcher(nullptr);	///< イベント送信オブジェクト

	InputDevice App::m_inputDevice;



	wstring App::m_wstrModulePath;		///< モジュール名フルパス
	wstring App::m_wstrDir;				///< モジュールがあるディレクトリ
	wstring App::m_wstrMediaDir;			///< 絶対パスのメディアディレクトリ
	wstring App::m_wstrShadersDir;		///< 絶対パスのシェーダディレクトリ
	wstring App::m_wstrRelativeMediaDir;	///< 相対パスのメディアディレクトリ
	wstring App::m_wstrRelativeShadersDir;	///< 相対パスのシェーダディレクトリ
	wstring App::m_wstrRelativeAssetsDir;	///< 相対パスのアセットディレクトリ

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
				throw runtime_error("mediaディレクトリを確認できません。");
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
		//イベント配送クラス
		m_eventDispatcher = make_shared<EventDispatcher>();
		//乱数の初期化
		srand((unsigned)time(nullptr));

	}

	int App::Run(BaseDevice* pBaseDevice, BaseScene* pBaseScene, HINSTANCE hInstance, int nCmdShow)
	{
		// デバッグ時、deleteもれのチェック用
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		//ロケールの設定
		setlocale(LC_ALL, "JPN");

		m_pBaseScene = pBaseScene;

		// Parse the command line parameters
		int argc;
		LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
		pBaseDevice->ParseCommandLineArgs(argv, argc);
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

		RECT windowRect = { 0, 0, static_cast<LONG>(pBaseDevice->GetWidth()), static_cast<LONG>(pBaseDevice->GetHeight()) };
		AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

		//ウィンドウの作成
		m_hwnd = CreateWindow(
			windowClass.lpszClassName,
			pBaseDevice->GetTitle().c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			nullptr,
			nullptr,
			hInstance,
			pBaseDevice);

		//終了コード
		int retCode = 0;
		//ウインドウ情報。メッセージボックス表示チェックに使用
		WINDOWINFO winInfo;
		ZeroMemory(&winInfo, sizeof(winInfo));
		//例外処理開始
		try {
			SetInitData();
			//COMの初期化
			//サウンドなどで使用する
			if (FAILED(::CoInitialize(nullptr))) {
				// 初期化失敗
				throw exception("Com初期化に失敗しました。");
			}
			pBaseDevice->OnInit();
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
			pBaseDevice->OnDestroy();
			retCode = static_cast<char>(msg.wParam);
		}
		catch (BaseException& e) {
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
		catch (runtime_error& e) {
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
		if (retCode) {
			PostQuitMessage(0);
		}
		return retCode;
	}

	LRESULT CALLBACK App::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		BaseDevice* pDevice = reinterpret_cast<BaseDevice*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		WINDOWINFO winInfo;
		ZeroMemory(&winInfo, sizeof(winInfo));

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
				//更新描画処理
				pDevice->OnUpdateRender();
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

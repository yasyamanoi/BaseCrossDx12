/*!
@file App.cpp
@brief �A�v���P�[�V�����N���X
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/
#include "stdafx.h"

namespace basecross {

	HWND App::m_hwnd = nullptr;

	BaseScene* App::m_pBaseScene = nullptr;
	shared_ptr<EventDispatcher> App::m_EventDispatcher = nullptr;	///< �C�x���g���M�I�u�W�F�N�g


	InputDevice App::m_inputDevice;



	wstring App::m_wstrModulePath;		///< ���W���[�����t���p�X
	wstring App::m_wstrDir;				///< ���W���[��������f�B���N�g��
	wstring App::m_wstrMediaDir;			///< ��΃p�X�̃��f�B�A�f�B���N�g��
	wstring App::m_wstrShadersDir;		///< ��΃p�X�̃V�F�[�_�f�B���N�g��
	wstring App::m_wstrRelativeMediaDir;	///< ���΃p�X�̃��f�B�A�f�B���N�g��
	wstring App::m_wstrRelativeShadersDir;	///< ���΃p�X�̃V�F�[�_�f�B���N�g��
	wstring App::m_wstrRelativeAssetsDir;	///< ���΃p�X�̃A�Z�b�g�f�B���N�g��

	void App::SetInitData() {

		//��f�B���N�g���̐ݒ�
		//���΃p�X�ɂ���ƁA�t�@�C���_�C�A���O�ŃJ�����g�p�X�������̂�
		//��΃p�X�w��
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

		//���W���[�����i�v���O�����t�@�C�����j�𓾂�
		if (!::GetModuleFileName(nullptr, Modulebuff, sizeof(Modulebuff))) {
			throw runtime_error("���W���[�����擾�ł��܂���B");
		}
		m_wstrModulePath = Modulebuff;
		//���W���[��������A�e�u���b�N�ɕ�����
		_wsplitpath_s(Modulebuff,
			Drivebuff, _MAX_DRIVE,
			Dirbuff, _MAX_DIR,
			FileNamebuff, _MAX_FNAME,
			Extbuff, _MAX_EXT);

		//�h���C�u���̎擾
		m_wstrDir = Drivebuff;
		//�f�B���N�g�����̎擾
		m_wstrDir += Dirbuff;
		//media�f�B���N�g����T��
		m_wstrMediaDir = m_wstrDir;
		m_wstrMediaDir += L"media";
		//�܂��A���s�t�@�C���Ɠ����f�B���N�g����T��
		DWORD RetCode;
		RetCode = GetFileAttributes(m_wstrMediaDir.c_str());
		if (RetCode == 0xFFFFFFFF) {
			//���s����
			m_wstrMediaDir = m_wstrDir;
			m_wstrMediaDir += L"..\\media";
			RetCode = GetFileAttributes(m_wstrMediaDir.c_str());
			if (RetCode == 0xFFFFFFFF) {
				//�Ăю��s����
				throw runtime_error("media�f�B���N�g�����m�F�ł��܂���B");
			}
			else {
				m_wstrMediaDir += L"\\";
				//���΃p�X�̐ݒ�
				m_wstrRelativeMediaDir = L"..\\media\\";
			}
		}
		else {
			m_wstrMediaDir += L"\\";
			//���΃p�X�̐ݒ�
			m_wstrRelativeMediaDir = L"media\\";
		}
		m_wstrShadersDir = m_wstrMediaDir + L"Shaders\\";
		m_wstrRelativeShadersDir = m_wstrRelativeMediaDir + L"Shaders\\";
		//Assets�f�B���N�g����T��
		m_wstrRelativeAssetsDir = L"..\\..\\Assets";
		//���΃f�B���N�g����T��
		RetCode = GetFileAttributes(m_wstrRelativeAssetsDir.c_str());
		if (RetCode == 0xFFFFFFFF) {
			//���s����
			//�A�Z�b�g�f�B���N�g�������f�B�A�f�B���N�g���ɂ���
			m_wstrRelativeAssetsDir = m_wstrRelativeMediaDir;
		}
		else {
			//��������
			m_wstrRelativeAssetsDir += L"\\";
		}
		//�����̏�����
		srand((unsigned)time(nullptr));

	}

	int App::Run(BaseDevice* pBaseDevice, BaseScene* pBaseScene, HINSTANCE hInstance, int nCmdShow)
	{
		// �f�o�b�O���Adelete����̃`�F�b�N�p
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		//���P�[���̐ݒ�
		setlocale(LC_ALL, "JPN");

		m_pBaseScene = pBaseScene;
		m_EventDispatcher = shared_ptr<EventDispatcher>(new EventDispatcher);

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

		//�E�B���h�E�̍쐬
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

		//�I���R�[�h
		int retCode = 0;
		//�E�C���h�E���B���b�Z�[�W�{�b�N�X�\���`�F�b�N�Ɏg�p
		WINDOWINFO winInfo;
		ZeroMemory(&winInfo, sizeof(winInfo));
		//��O�����J�n
		try {
			SetInitData();
			//COM�̏�����
			//�T�E���h�ȂǂŎg�p����
			if (FAILED(::CoInitialize(nullptr))) {
				// ���������s
				throw exception("Com�������Ɏ��s���܂����B");
			}
			pBaseDevice->OnCreate();
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
			//�f�o�b�O�o�͂�����B
			string str = e.what_m() + "\n";
			OutputDebugStringA(str.c_str());

			if (GetWindowInfo(m_hwnd, &winInfo)) {
				//���s���s����
				MessageBoxA(m_hwnd, e.what_m().c_str(), "�G���[", MB_OK);
			}
			else {
				//���s���s����
				MessageBoxA(nullptr, e.what_m().c_str(), "�G���[", MB_OK);
			}
			retCode = 1;
		}
		catch (runtime_error& e) {
			//�f�o�b�O�o�͂�����B
			string str(e.what());
			str += "\n";
			OutputDebugStringA(str.c_str());

			if (GetWindowInfo(m_hwnd, &winInfo)) {
				//���s���s����
				MessageBoxA(m_hwnd, e.what(), "�G���[", MB_OK);
			}
			else {
				//���s���s����
				MessageBoxA(nullptr, e.what(), "�G���[", MB_OK);
			}
			retCode = 1;
		}
		catch (exception& e) {
			//�f�o�b�O�o�͂�����B
			string str(e.what());
			str += "\n";
			OutputDebugStringA(str.c_str());
			//STL�G���[
			//�}���`�o�C�g�o�[�W�����̃��b�Z�[�W�{�b�N�X���Ă�
			if (GetWindowInfo(m_hwnd, &winInfo)) {
				MessageBoxA(m_hwnd, e.what(), "�G���[", MB_OK);
			}
			else {
				MessageBoxA(nullptr, e.what(), "�G���[", MB_OK);
			}
			retCode = 1;
		}
		catch (...) {
			//�f�o�b�O�o�͂�����B
			OutputDebugStringA("�����s���̃G���[\n");
			//�����s�����s����
			if (GetWindowInfo(m_hwnd, &winInfo)) {
				MessageBox(m_hwnd, L"�����s���̃G���[�ł�", L"�G���[", MB_OK);
			}
			else {
				MessageBox(nullptr, L"�����s���̃G���[�ł�", L"�G���[", MB_OK);
			}
			retCode = 1;
		}
		//��O�����I��
		//COM�̃����[�X
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
						//�E�C���h�E��j������
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
				//�X�V�`�揈��
				pDevice->OnUpdateDraw();
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
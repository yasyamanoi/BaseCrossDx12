/*!
@file App.h
@brief �A�v���P�[�V�����N���X
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	�A�v���P�[�V�����N���X
	//--------------------------------------------------------------------------------------
	class App
	{
		static HWND m_hwnd;
		static BaseScene* m_pBaseScene;
		static shared_ptr<EventDispatcher> m_eventDispatcher;	///< �C�x���g���M�I�u�W�F�N�g
		static InputDevice m_inputDevice;


		static wstring m_wstrModulePath;		///< ���W���[�����t���p�X
		static wstring m_wstrDir;				///< ���W���[��������f�B���N�g��
		static wstring m_wstrMediaDir;			///< ��΃p�X�̃��f�B�A�f�B���N�g��
		static wstring m_wstrShadersDir;		///< ��΃p�X�̃V�F�[�_�f�B���N�g��
		static wstring m_wstrRelativeMediaDir;	///< ���΃p�X�̃��f�B�A�f�B���N�g��
		static wstring m_wstrRelativeShadersDir;	///< ���΃p�X�̃V�F�[�_�f�B���N�g��
		static wstring	m_wstrRelativeAssetsDir;	///< ���΃p�X�̃A�Z�b�g�f�B���N�g��

		static void SetInitData();

	protected:
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�A�v���P�[�V�����N���X�̎��s
		@param[in]	pBaseDevice	�f�o�C�X�N���X
		@param[in]	pBaseScene	�V�[���̐e�N���X
		@param[in]	hInstance	�C���X�^���X�̃n���h��
		@param[in]	nCmdShow	�\�����[�h
		@return �A�v���P�[�V�����̏I���R�[�h
		*/
		//--------------------------------------------------------------------------------------
		static int Run(BaseDevice* pBaseDevice, BaseScene* pBaseScene, HINSTANCE hInstance, int nCmdShow);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�E�C���h�E�̃n���h���𓾂�
		@return �E�C���h�E�̃n���h��
		*/
		//--------------------------------------------------------------------------------------
		static HWND GetHwnd() { return m_hwnd; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�V�[���̐e�N���X�𓾂�
		@return �V�[���̐e�N���X
		*/
		//--------------------------------------------------------------------------------------
		static BaseScene* GetBaseScene() { return m_pBaseScene; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�f�o�C�X�N���X�𓾂�
		@return �f�o�C�X�N���X
		*/
		//--------------------------------------------------------------------------------------
		static BaseDevice* GetBaseDevice() {
			return reinterpret_cast<BaseDevice*>(GetWindowLongPtr(App::GetHwnd(), GWLP_USERDATA));
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ID3D12Device�𓾂�
		@return ID3D12Device��ComPtr
		*/
		//--------------------------------------------------------------------------------------
		static ComPtr<ID3D12Device> GetID3D12Device() {
			return GetBaseDevice()->GetID3D12Device();
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ElapsedTime�𓾂�
		@return ElapsedTime
		*/
		//--------------------------------------------------------------------------------------
		static float GetElapsedTime() {
			return (float)GetBaseDevice()->GetElapsedTime();
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�C�x���g�z���I�u�W�F�N�g�𓾂�
		@return �C�x���g�z���I�u�W�F�N�g
		*/
		//--------------------------------------------------------------------------------------
		static shared_ptr<EventDispatcher> GetEventDispatcher() {
			return m_eventDispatcher;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���̓f�o�C�X�𓾂�
		@return ���̓f�o�C�X
		*/
		//--------------------------------------------------------------------------------------
		static InputDevice& GetInputDevice() {
			return m_inputDevice;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���W���[�����t���p�X�𓾂�
		@return ���W���[�����t���p�X
		*/
		//--------------------------------------------------------------------------------------
		static const wstring& GetModulePath() { return m_wstrModulePath; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���W���[��������f�B���N�g���𓾂�
		@return ���W���[��������f�B���N�g��
		*/
		//--------------------------------------------------------------------------------------
		static const wstring& GetModuleDir() { return m_wstrDir; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	��΃p�X�̃��f�B�A�f�B���N�g���𓾂�
		@return ��΃p�X�̃��f�B�A�f�B���N�g��
		*/
		//--------------------------------------------------------------------------------------
		static const wstring& GetMediaDir() { return m_wstrMediaDir; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	��΃p�X�̃V�F�[�_�f�B���N�g���𓾂�
		@return ��΃p�X�̃V�F�[�_�f�B���N�g��
		*/
		//--------------------------------------------------------------------------------------
		static const wstring& GetShadersDir() { return m_wstrShadersDir; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���΃p�X�̃��f�B�A�f�B���N�g���𓾂�
		@return ���΃p�X�̃��f�B�A�f�B���N�g��
		*/
		//--------------------------------------------------------------------------------------
		static const wstring& GetRelativeMediaDir() { return m_wstrRelativeMediaDir; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���΃p�X�̃V�F�[�_�f�B���N�g���𓾂�
		@return ���΃p�X�̃V�F�[�_�f�B���N�g��
		*/
		//--------------------------------------------------------------------------------------
		static const wstring& GetRelativeShadersDir() { return m_wstrRelativeShadersDir; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���΃p�X�̃A�Z�b�g�f�B���N�g���𓾂�
		@return ���΃p�X�̃A�Z�b�g�f�B���N�g��
		*/
		//--------------------------------------------------------------------------------------
		static const wstring& GetRelativeAssetsDir() { return m_wstrRelativeAssetsDir; }


	};
}
// end namespace basecross
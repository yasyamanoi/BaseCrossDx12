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
@brief �A�v���P�[�V�����N���X
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#pragma once

#include "stdafx.h"

namespace basecross {


	class PrimDevice;

	//--------------------------------------------------------------------------------------
	///	�A�v���P�[�V�����N���X
	//--------------------------------------------------------------------------------------
	class App
	{
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�A�v���P�[�V�����̎��s
		@param[in]	pPrimDevice	��{�f�o�C�X
		@param[in]	hInstance	�C���X�^���X�̃n���h��
		@param[in]	nCmdShow	�����\�����
		@return	���s����
		*/
		//--------------------------------------------------------------------------------------
		static int Run(PrimDevice* pPrimDevice, HINSTANCE hInstance, int nCmdShow);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�t���X�N���[���ƃE�B���h�E�̐؂�ւ�
		@param[in]	pOutput	�X���b�v�`�F�[���̃|�C���^
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		static void ToggleFullscreenWindow(IDXGISwapChain* pOutput = nullptr);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Z�I�[�_�[��ύX����
		@param[in]	setToTopMost	TopMost�ɂ��邩�ǂ���
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		static void SetWindowZorderToTopMost(bool setToTopMost);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Window�̃n���h���𓾂�
		@return	Window�̃n���h��
		*/
		//--------------------------------------------------------------------------------------
		static HWND GetHwnd() { return m_hwnd; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�t���X�N���[�����ǂ���
		@return	�t���X�N���[���Ȃ�true
		*/
		//--------------------------------------------------------------------------------------
		static bool IsFullscreen() { return m_fullscreenMode; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	��{�f�o�C�X�𓾂�
		@return	��{�f�o�C�X�̃|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static PrimDevice* GetPrimDevice() { return m_pPrimDevice; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ID3D12Device�𓾂�
		@return	ID3D12Device�̃|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static ID3D12Device* GetD3D12Device();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ComPtr�Ƃ��Ă�ID3D12Device�𓾂�
		@return	ID3D12Device��ComPtr
		*/
		//--------------------------------------------------------------------------------------
		static ComPtr<ID3D12Device> GetID3D12Device();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���W���[�����t���p�X�𓾂�
		@return ���W���[�����t���p�X
		*/
		//--------------------------------------------------------------------------------------
		static const std::wstring& GetModulePath() { return m_wstrModulePath; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���W���[��������f�B���N�g���𓾂�
		@return ���W���[��������f�B���N�g��
		*/
		//--------------------------------------------------------------------------------------
		static const std::wstring& GetModuleDir() { return m_wstrDir; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	��΃p�X�̃��f�B�A�f�B���N�g���𓾂�
		@return ��΃p�X�̃��f�B�A�f�B���N�g��
		*/
		//--------------------------------------------------------------------------------------
		static const std::wstring& GetMediaDir() { return m_wstrMediaDir; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	��΃p�X�̃V�F�[�_�f�B���N�g���𓾂�
		@return ��΃p�X�̃V�F�[�_�f�B���N�g��
		*/
		//--------------------------------------------------------------------------------------
		static const std::wstring& GetShadersDir() { return m_wstrShadersDir; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���΃p�X�̃��f�B�A�f�B���N�g���𓾂�
		@return ���΃p�X�̃��f�B�A�f�B���N�g��
		*/
		//--------------------------------------------------------------------------------------
		static const std::wstring& GetRelativeMediaDir() { return m_wstrRelativeMediaDir; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���΃p�X�̃V�F�[�_�f�B���N�g���𓾂�
		@return ���΃p�X�̃V�F�[�_�f�B���N�g��
		*/
		//--------------------------------------------------------------------------------------
		static const std::wstring& GetRelativeShadersDir() { return m_wstrRelativeShadersDir; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���΃p�X�̃A�Z�b�g�f�B���N�g���𓾂�
		@return ���΃p�X�̃A�Z�b�g�f�B���N�g��
		*/
		//--------------------------------------------------------------------------------------
		static const std::wstring& GetRelativeAssetsDir() { return m_wstrRelativeAssetsDir; }

		//--------------------------------------------------------------------------------------
		/*!
		@brief	���̓f�o�C�X�𓾂�
		@return ���̓f�o�C�X
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

		static std::wstring m_wstrModulePath;		///< ���W���[�����t���p�X
		static std::wstring m_wstrDir;				///< ���W���[��������f�B���N�g��
		static std::wstring m_wstrMediaDir;			///< ��΃p�X�̃��f�B�A�f�B���N�g��
		static std::wstring m_wstrShadersDir;		///< ��΃p�X�̃V�F�[�_�f�B���N�g��
		static std::wstring m_wstrRelativeMediaDir;	///< ���΃p�X�̃��f�B�A�f�B���N�g��
		static std::wstring m_wstrRelativeShadersDir;	///< ���΃p�X�̃V�F�[�_�f�B���N�g��
		static std::wstring	m_wstrRelativeAssetsDir;	///< ���΃p�X�̃A�Z�b�g�f�B���N�g��

		static void SetInitData();


	};
}
// end namespace basecross



/*!
@file InputDevice.h
@brief ���̓f�o�C�X�N���X
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	/// XBox�R���g���[���[�̃X�e�[�^�X�BXINPUT_STATE�h��
	//--------------------------------------------------------------------------------------
	struct CONTROLER_STATE : public XINPUT_STATE
	{

		WORD wButtons;		///< �{�^�����
		BYTE bLeftTrigger;	///< ���g���K
		BYTE bRightTrigger;	///< �E�g���K

		float fThumbRX;	///< �E�X�e�B�b�NX��flort�ϊ�
		float fThumbRY;	///< �E�X�e�B�b�NY��flort�ϊ�
		float fThumbLX;	///< ���X�e�B�b�NX��flort�ϊ�
		float fThumbLY;	///< ���X�e�B�b�NY��flort�ϊ�
		WORD wNowUpdateButtons;	///< ��Ԃ��ς�����Ƃ��Ƀt���O�����{�^��
		WORD wPressedButtons;	///< �����ꂽ�u�Ԃ���������{�^��
		WORD wReleasedButtons;	///< �����ꂽ�u�Ԃ���������{�^��
		WORD wLastButtons;	///< ��O�̃{�^��
		bool bConnected;	///< �ڑ����Ă��邩�ǂ���
	};

	//--------------------------------------------------------------------------------------
	///	���͋@��
	//--------------------------------------------------------------------------------------
	class InputDevice {
		const DWORD MAX_CONTROLLERS = 4; // �R���g���[����
		std::vector<CONTROLER_STATE> m_State; // �R���g���[���X�e�[�g�̔z��
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief �R���X�g���N�^
		*/
		//--------------------------------------------------------------------------------------
		InputDevice() :
			m_State(MAX_CONTROLLERS)
		{
			for (DWORD i = 0; i < MAX_CONTROLLERS; i++) {
				::ZeroMemory(&m_State[i], sizeof(CONTROLER_STATE));
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief �f�X�g���N�^
		*/
		//--------------------------------------------------------------------------------------
		~InputDevice() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief �R���g���[���[�̏�Ԃ�m_State�Ɏ擾����
		*/
		//--------------------------------------------------------------------------------------
		void ResetControlerState() {
			DWORD dwResult;
			for (DWORD i = 0; i < MAX_CONTROLLERS; i++)
			{
				//��O�̏�Ԃ�ۑ�
				m_State[i].wLastButtons = m_State[i].Gamepad.wButtons;
				// XInput���猻�݂̃X�e�[�^�X�𓾂�
				XINPUT_STATE State;
				dwResult = XInputGetState(i, &State);
				if (dwResult == ERROR_SUCCESS) {
					m_State[i].dwPacketNumber = State.dwPacketNumber;
					m_State[i].Gamepad = State.Gamepad;
					m_State[i].bConnected = true;
					//�{�^���̐ݒ�
					//�R�s�[
					m_State[i].wButtons = m_State[i].Gamepad.wButtons;
					//�X�V�{�^��
					m_State[i].wNowUpdateButtons = m_State[i].Gamepad.wButtons;
					m_State[i].bLeftTrigger = m_State[i].Gamepad.bLeftTrigger;
					m_State[i].bRightTrigger = m_State[i].Gamepad.bRightTrigger;
					//�O����ω����Ă���{�^���̂݃t���O�𗧂Ă�
					//XOR���Z�ɂ�����
					m_State[i].wNowUpdateButtons ^= m_State[i].wLastButtons;
					//�����ꂽ�u�Ԃ��Z�b�g
					m_State[i].wPressedButtons = m_State[i].wNowUpdateButtons & m_State[i].wButtons;
					//�����ꂽ�u�Ԃ��Z�b�g
					m_State[i].wReleasedButtons = m_State[i].wNowUpdateButtons & (m_State[i].wButtons ^ 0xffff);
					//���o�[�̃f�b�h�]�[���̐ݒ�
					if ((m_State[i].Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
						m_State[i].Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
						(m_State[i].Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
							m_State[i].Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
					{
						m_State[i].Gamepad.sThumbLX = 0;
						m_State[i].Gamepad.sThumbLY = 0;
					}
					if (m_State[i].Gamepad.sThumbLX >= 0) {
						m_State[i].fThumbLX = (float)m_State[i].Gamepad.sThumbLX / (float)SHRT_MAX;
					}
					else {
						m_State[i].fThumbLX = (float)m_State[i].Gamepad.sThumbLX / (float)(SHRT_MAX + 1);
					}
					if (m_State[i].Gamepad.sThumbLY >= 0) {
						m_State[i].fThumbLY = (float)m_State[i].Gamepad.sThumbLY / (float)SHRT_MAX;
					}
					else {
						m_State[i].fThumbLY = (float)m_State[i].Gamepad.sThumbLY / (float)(SHRT_MAX + 1);
					}
					if ((m_State[i].Gamepad.sThumbRX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
						m_State[i].Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
						(m_State[i].Gamepad.sThumbRY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
							m_State[i].Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
					{
						m_State[i].Gamepad.sThumbRX = 0;
						m_State[i].Gamepad.sThumbRY = 0;
					}
					if (m_State[i].Gamepad.sThumbRX >= 0) {
						m_State[i].fThumbRX = (float)m_State[i].Gamepad.sThumbRX / (float)SHRT_MAX;
					}
					else {
						m_State[i].fThumbRX = (float)m_State[i].Gamepad.sThumbRX / (float)(SHRT_MAX + 1);
					}
					if (m_State[i].Gamepad.sThumbRY >= 0) {
						m_State[i].fThumbRY = (float)m_State[i].Gamepad.sThumbRY / (float)SHRT_MAX;
					}
					else {
						m_State[i].fThumbRY = (float)m_State[i].Gamepad.sThumbRY / (float)(SHRT_MAX + 1);
					}
				}
				else {
					m_State[i].bConnected = false;
				}
			}
		}
		//	�A�N�Z�T
		//--------------------------------------------------------------------------------------
		/*!
		@brief �R���g���[���X�e�[�g�̎擾
		@return	�R���g���[���X�e�[�g
		*/
		//--------------------------------------------------------------------------------------
		const std::vector<CONTROLER_STATE>& GetControlerVec()const {
			return m_State; 
		}
	};


}
// end namespace basecross

/*!
@file Event.h
@brief �C�x���g
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	class ObjectFactory;
	class ObjectInterface;

	//--------------------------------------------------------------------------------------
	///	�C�x���g�\����
	//--------------------------------------------------------------------------------------
	struct Event {
		///	�x�����ԁiSendEvent�̏ꍇ�͏��0�j
		float m_DispatchTime;
		///	���̃��b�Z�[�W�𑗂����I�u�W�F�N�g
		weak_ptr<ObjectInterface> m_Sender;
		///	�󂯎��I�u�W�F�N�g�inullptr�̏ꍇ�̓A�N�e�B�u�X�e�[�W�����ׂĂ������̓L�[���[�h�Ŏ��ʂ���I�u�W�F�N�g�j
		weak_ptr<ObjectInterface> m_Receiver;
		///	���b�Z�[�W������
		wstring m_MsgStr;
		///	�ǉ��������I�u�W�F�N�g�̃|�C���^
		shared_ptr<void> m_Info;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�R���X�g���N�^
		@param[in]	DispatchTime	�z���܂ł̎���
		@param[in]	Sender	���葤�I�u�W�F�N�g�inullptr�j
		@param[in]	Receiver	�󂯎葤�I�u�W�F�N�g
		@param[in]	MsgStr	���b�Z�[�W������
		@param[in]	Info	�ǉ����������[�U�[�f�[�^
		*/
		//--------------------------------------------------------------------------------------
		Event(float DispatchTime, const shared_ptr<ObjectInterface>& Sender, const shared_ptr<ObjectInterface>& Receiver,
			const wstring& MsgStr, const shared_ptr<void>& Info = shared_ptr<void>()) :
			m_DispatchTime(DispatchTime),
			m_Sender(Sender),
			m_Receiver(Receiver),
			m_MsgStr(MsgStr),
			m_Info(Info)
		{}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�f�X�g���N�^
		*/
		//--------------------------------------------------------------------------------------
		~Event() {}
	};

	//--------------------------------------------------------------------------------------
	///	�C�x���g�z���N���X
	//--------------------------------------------------------------------------------------
	class EventDispatcher {
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�R���X�g���N�^
		@param[in]	SceneBasePtr	�V�[���x�[�X�̃|�C���^
		*/
		//--------------------------------------------------------------------------------------
		explicit EventDispatcher();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�f�X�g���N�^
		*/
		//--------------------------------------------------------------------------------------
		virtual ~EventDispatcher();


		//--------------------------------------------------------------------------------------
		/*!
		@brief	�C�x���g���󂯎��O���[�v�ɒǉ��i�O���[�v���Ȃ���΂��̖��O�ō쐬�j
		@param[in]	GroupKey	�O���[�v��
		@param[in]	Receiver	�󂯎葤�I�u�W�F�N�g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void AddEventReceiverGroup(const wstring& GroupKey, const shared_ptr<ObjectInterface>& Receiver);

		//--------------------------------------------------------------------------------------
		/*!
		@brief	�C�x���g��POST�i�L���[�ɓ����j
		@param[in]	Delay	�z���܂ł̎���
		@param[in]	Sender	���葤�I�u�W�F�N�g�inullptr�j
		@param[in]	Receiver	�󂯎葤�I�u�W�F�N�g
		@param[in]	MsgStr	���b�Z�[�W������
		@param[in]	Info	�ǉ����������[�U�[�f�[�^
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void PostEvent(float Delay, const shared_ptr<ObjectInterface>& Sender, const shared_ptr<ObjectInterface>& Receiver,
			const wstring& MsgStr, const  shared_ptr<void>& Info = shared_ptr<void>());
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�C�x���g��POST�i�L���[�ɓ����j
		@param[in]	DispatchTime	POST���鎞�ԁi0�Ŏ��̃^�[���j
		@param[in]	Sender	�C�x���g���M�ҁinullptr�j
		@param[in]	ReceiverKey	�󂯎葤�I�u�W�F�N�g�𔻕ʂ���L�[
		@param[in]	MsgStr	���b�Z�[�W
		@param[in,out]	Info	�ǉ����
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void PostEvent(float DispatchTime, const shared_ptr<ObjectInterface>& Sender, const wstring& ReceiverKey,
			const wstring& MsgStr, const  shared_ptr<void>& Info = shared_ptr<void>());
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�C�x���g��SEND�i�L���[�ɓ��ꂸ�ɂ��̂܂ܑ���j
		@param[in]	Sender	���葤�I�u�W�F�N�g�inullptr�j
		@param[in]	Receiver	�󂯎葤�I�u�W�F�N�g
		@param[in]	MsgStr	���b�Z�[�W������
		@param[in]	Info	�ǉ����������[�U�[�f�[�^
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SendEvent(const shared_ptr<ObjectInterface>& Sender, const shared_ptr<ObjectInterface>& Receiver,
			const wstring& MsgStr, const  shared_ptr<void>& Info = shared_ptr<void>());
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�C�x���g��SEND�i�L���[�ɓ��ꂸ�ɂ��̂܂ܑ���j
		@param[in]	Sender	�C�x���g���M�ҁinullptr�j
		@param[in]	Receiver	�C�x���g��M�ҁinullptr�s�j
		@param[in]	MsgStr	���b�Z�[�W
		@param[in,out]	Info	�ǉ����
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SendEvent(const shared_ptr<ObjectInterface>& Sender, const wstring& ReceiverKey,
			const wstring& MsgStr, const  shared_ptr<void>& Info = shared_ptr<void>());

		//--------------------------------------------------------------------------------------
		/*!
		@brief	POST�C�x���g�̑��M(���C�����[�v�ŌĂ΂��)
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void DispatchDelayedEvent();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�L���[�ɂ��܂��Ă��郁�b�Z�[�W���폜����
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void ClearEventQ();
	private:
		// pImpl�C�f�B�I��
		struct Impl;
		unique_ptr<Impl> pImpl;
		//�R�s�[�֎~
		EventDispatcher(const EventDispatcher&) = delete;
		EventDispatcher& operator=(const EventDispatcher&) = delete;
		//���[�u�֎~
		EventDispatcher(const EventDispatcher&&) = delete;
		EventDispatcher& operator=(const EventDispatcher&&) = delete;
	};


}
// end namespace basecross 
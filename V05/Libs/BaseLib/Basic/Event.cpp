/*!
@file Event.cpp
@brief �C�x���g
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	struct EventDispatcher::Impl;
	//	�p�r: Impl�\����
	//--------------------------------------------------------------------------------------
	struct EventDispatcher::Impl {
		//�C�x���g�̃L���[
		list< shared_ptr<Event> > m_PriorityQ;
		map<wstring, vector<weak_ptr<ObjectInterface>>> m_EventInterfaceGroupMap;
		//
		//--------------------------------------------------------------------------------------
		//	void Discharge(
		//	const Event& event	//�C�x���g
		//	);
		//�p�r: �C�x���g�̑��M
		//�߂�l: �Ȃ�
		//--------------------------------------------------------------------------------------
		void Discharge(const shared_ptr<Event>& event);
		Impl() {}
		~Impl() {}
	};

	void EventDispatcher::Impl::Discharge(const shared_ptr<Event>& event) {
		auto shptr = event->m_Receiver.lock();
		if (shptr) {
			//�󂯎肪�L��
			shptr->OnEvent(event);
		}
	}



	//--------------------------------------------------------------------------------------
	///	�C�x���g�z���N���X
	//--------------------------------------------------------------------------------------
	//�\�z�Ɣj��
	EventDispatcher::EventDispatcher() :
		pImpl(new Impl())
	{}
	EventDispatcher::~EventDispatcher() {}

	void EventDispatcher::AddEventReceiverGroup(const wstring& GroupKey, const shared_ptr<ObjectInterface>& Receiver) {
		auto it = pImpl->m_EventInterfaceGroupMap.find(GroupKey);
		if (it != pImpl->m_EventInterfaceGroupMap.end()) {
			//�L�[��������
			it->second.push_back(Receiver);
		}
		else {
			//�O���[�v���Ȃ�
			vector<weak_ptr<ObjectInterface>> vec;
			pImpl->m_EventInterfaceGroupMap[GroupKey] = vec;
			pImpl->m_EventInterfaceGroupMap[GroupKey].push_back(Receiver);
		}
	}


	//�C�x���g��POST�i�L���[�ɓ����j
	void EventDispatcher::PostEvent(float Delay, const shared_ptr<ObjectInterface>& Sender, const shared_ptr<ObjectInterface>& Receiver,
		const wstring& MsgStr, const  shared_ptr<void>& Info) {
		//�C�x���g�̍쐬 
		auto Ptr = make_shared<Event>(Delay, Sender, Receiver, MsgStr, Info);
		//�L���[�ɂ��߂�
		pImpl->m_PriorityQ.push_back(Ptr);
	}

	void EventDispatcher::PostEvent(float DispatchTime, const shared_ptr<ObjectInterface>& Sender, const wstring& ReceiverKey,
		const wstring& MsgStr, const  shared_ptr<void>& Info) {
		//ReceiverKey�ɂ�鑊��̓���
		//�d���L�[�̌���
		auto it = pImpl->m_EventInterfaceGroupMap.find(ReceiverKey);
		if (it != pImpl->m_EventInterfaceGroupMap.end()) {
			//�L�[��������
			for (auto v : it->second) {
				auto shptr = v.lock();
				if (shptr) {
					//�C�x���g�̍쐬 
					auto Ptr = make_shared<Event>(0.0f, Sender, shptr, MsgStr, Info);
					//�L���[�ɂ��߂�
					pImpl->m_PriorityQ.push_back(Ptr);
				}
			}
		}
		//�L�[��������Ȃ��Ă��������Ȃ�
	}


	//�C�x���g��SEND�i�L���[�ɓ��ꂸ�ɂ��̂܂ܑ���j
	void EventDispatcher::SendEvent(const shared_ptr<ObjectInterface>& Sender, const shared_ptr<ObjectInterface>& Receiver,
		const wstring& MsgStr, const  shared_ptr<void>& Info) {
		//�C�x���g�̍쐬 
		auto Ptr = make_shared<Event>(0.0f, Sender, Receiver, MsgStr, Info);
		//���M
		pImpl->Discharge(Ptr);
	}

	void EventDispatcher::SendEvent(const shared_ptr<ObjectInterface>& Sender, const wstring& ReceiverKey,
		const wstring& MsgStr, const  shared_ptr<void>& Info) {
		//ReceiverKey�ɂ�鑊��̓���
		//�d���L�[�̌���
		auto it = pImpl->m_EventInterfaceGroupMap.find(ReceiverKey);
		if (it != pImpl->m_EventInterfaceGroupMap.end()) {
			//�L�[��������
			for (auto v : it->second) {
				auto shptr = v.lock();
				if (shptr) {
					//�C�x���g�̍쐬 
					auto Ptr = make_shared<Event>(0.0f, Sender, shptr, MsgStr, Info);
					//�C�x���g�̑��o
					pImpl->Discharge(Ptr);
				}
			}
		}
		//�L�[��������Ȃ��Ă��������Ȃ�
	}


	void EventDispatcher::DispatchDelayedEvent() {
		//�O��̃^�[������̎���
		float ElapsedTime = App::GetElapsedTime();
		auto it = pImpl->m_PriorityQ.begin();
		while (it != pImpl->m_PriorityQ.end()) {
			(*it)->m_DispatchTime -= ElapsedTime;
			if ((*it)->m_DispatchTime <= 0.0f) {
				(*it)->m_DispatchTime = 0.0f;
				//���b�Z�[�W�̑��M
				pImpl->Discharge(*it);
				//�L���[����폜
				it = pImpl->m_PriorityQ.erase(it);
				//�폜��̃C�e���[�^���u�Ō�v��
				//�Ƃ��̓��[�v�𔲂���
				if (it == pImpl->m_PriorityQ.end()) {
					break;
				}
			}
			else {
				it++;
			}
		}
	}

	void EventDispatcher::ClearEventQ() {
		pImpl->m_PriorityQ.clear();
	}



}
// end namespace basecross
/*!
@file Event.cpp
@brief イベント
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
*/

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	struct EventDispatcher::Impl;
	//	用途: Impl構造体
	//--------------------------------------------------------------------------------------
	struct EventDispatcher::Impl {
		//イベントのキュー
		std::list< std::shared_ptr<Event> > m_PriorityQ;
		std::map<std::wstring, std::vector<std::weak_ptr<ObjectInterface>>> m_EventInterfaceGroupMap;
		//
		//--------------------------------------------------------------------------------------
		//	void Discharge(
		//	const Event& event	//イベント
		//	);
		//用途: イベントの送信
		//戻り値: なし
		//--------------------------------------------------------------------------------------
		void Discharge(const std::shared_ptr<Event>& event);
		Impl() {}
		~Impl() {}
	};

	void EventDispatcher::Impl::Discharge(const std::shared_ptr<Event>& event) {
		auto shptr = event->m_Receiver.lock();
		if (shptr) {
			//受け手が有効
			shptr->OnEvent(event);
		}
	}



	//--------------------------------------------------------------------------------------
	///	イベント配送クラス
	//--------------------------------------------------------------------------------------
	//構築と破棄
	EventDispatcher::EventDispatcher() :
		pImpl(new Impl())
	{}
	EventDispatcher::~EventDispatcher() {}

	void EventDispatcher::AddEventReceiverGroup(const std::wstring& GroupKey, const std::shared_ptr<ObjectInterface>& Receiver) {
		auto it = pImpl->m_EventInterfaceGroupMap.find(GroupKey);
		if (it != pImpl->m_EventInterfaceGroupMap.end()) {
			//キーがあった
			it->second.push_back(Receiver);
		}
		else {
			//グループがない
			std::vector<std::weak_ptr<ObjectInterface>> vec;
			pImpl->m_EventInterfaceGroupMap[GroupKey] = vec;
			pImpl->m_EventInterfaceGroupMap[GroupKey].push_back(Receiver);
		}
	}


	//イベントのPOST（キューに入れる）
	void EventDispatcher::PostEvent(float Delay, const std::shared_ptr<ObjectInterface>& Sender, const std::shared_ptr<ObjectInterface>& Receiver,
		const std::wstring& MsgStr, const  std::shared_ptr<void>& Info) {
		//イベントの作成 
		auto Ptr = std::make_shared<Event>(Delay, Sender, Receiver, MsgStr, Info);
		//キューにためる
		pImpl->m_PriorityQ.push_back(Ptr);
	}

	void EventDispatcher::PostEvent(float DispatchTime, const std::shared_ptr<ObjectInterface>& Sender, const std::wstring& ReceiverKey,
		const std::wstring& MsgStr, const  std::shared_ptr<void>& Info) {
		//ReceiverKeyによる相手の特定
		//重複キーの検査
		auto it = pImpl->m_EventInterfaceGroupMap.find(ReceiverKey);
		if (it != pImpl->m_EventInterfaceGroupMap.end()) {
			//キーがあった
			for (auto v : it->second) {
				auto shptr = v.lock();
				if (shptr) {
					//イベントの作成 
					auto Ptr = std::make_shared<Event>(0.0f, Sender, shptr, MsgStr, Info);
					//キューにためる
					pImpl->m_PriorityQ.push_back(Ptr);
				}
			}
		}
		//キーが見つからなくても何もしない
	}


	//イベントのSEND（キューに入れずにそのまま送る）
	void EventDispatcher::SendEvent(const std::shared_ptr<ObjectInterface>& Sender, const std::shared_ptr<ObjectInterface>& Receiver,
		const std::wstring& MsgStr, const  std::shared_ptr<void>& Info) {
		//イベントの作成 
		auto Ptr = std::make_shared<Event>(0.0f, Sender, Receiver, MsgStr, Info);
		//送信
		pImpl->Discharge(Ptr);
	}

	void EventDispatcher::SendEvent(const std::shared_ptr<ObjectInterface>& Sender, const std::wstring& ReceiverKey,
		const std::wstring& MsgStr, const  std::shared_ptr<void>& Info) {
		//ReceiverKeyによる相手の特定
		//重複キーの検査
		auto it = pImpl->m_EventInterfaceGroupMap.find(ReceiverKey);
		if (it != pImpl->m_EventInterfaceGroupMap.end()) {
			//キーがあった
			for (auto v : it->second) {
				auto shptr = v.lock();
				if (shptr) {
					//イベントの作成 
					auto Ptr = std::make_shared<Event>(0.0f, Sender, shptr, MsgStr, Info);
					//イベントの送出
					pImpl->Discharge(Ptr);
				}
			}
		}
		//キーが見つからなくても何もしない
	}


	void EventDispatcher::DispatchDelayedEvent() {
		//前回のターンからの時間
		float ElapsedTime = (float)App::GetElapsedTime();
		auto it = pImpl->m_PriorityQ.begin();
		while (it != pImpl->m_PriorityQ.end()) {
			(*it)->m_DispatchTime -= ElapsedTime;
			if ((*it)->m_DispatchTime <= 0.0f) {
				(*it)->m_DispatchTime = 0.0f;
				//メッセージの送信
				pImpl->Discharge(*it);
				//キューから削除
				it = pImpl->m_PriorityQ.erase(it);
				//削除後のイテレータが「最後」の
				//ときはループを抜ける
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

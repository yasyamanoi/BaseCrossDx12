/*!
@file Event.h
@brief イベント
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	class ObjectFactory;
	class ObjectInterface;

	//--------------------------------------------------------------------------------------
	///	イベント構造体
	//--------------------------------------------------------------------------------------
	struct Event {
		///	遅延時間（SendEventの場合は常に0）
		float m_DispatchTime;
		///	このメッセージを送ったオブジェクト
		weak_ptr<ObjectInterface> m_Sender;
		///	受け取るオブジェクト（nullptrの場合はアクティブステージ内すべてもしくはキーワードで識別するオブジェクト）
		weak_ptr<ObjectInterface> m_Receiver;
		///	メッセージ文字列
		wstring m_MsgStr;
		///	追加情報をもつオブジェクトのポインタ
		shared_ptr<void> m_Info;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンストラクタ
		@param[in]	DispatchTime	配送までの時間
		@param[in]	Sender	送り側オブジェクト（nullptr可）
		@param[in]	Receiver	受け手側オブジェクト
		@param[in]	MsgStr	メッセージ文字列
		@param[in]	Info	追加情報をもつユーザーデータ
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
		@brief	デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		~Event() {}
	};

	//--------------------------------------------------------------------------------------
	///	イベント配送クラス
	//--------------------------------------------------------------------------------------
	class EventDispatcher {
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンストラクタ
		@param[in]	SceneBasePtr	シーンベースのポインタ
		*/
		//--------------------------------------------------------------------------------------
		explicit EventDispatcher();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~EventDispatcher();


		//--------------------------------------------------------------------------------------
		/*!
		@brief	イベントを受け取るグループに追加（グループがなければその名前で作成）
		@param[in]	GroupKey	グループ名
		@param[in]	Receiver	受け手側オブジェクト
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void AddEventReceiverGroup(const wstring& GroupKey, const shared_ptr<ObjectInterface>& Receiver);

		//--------------------------------------------------------------------------------------
		/*!
		@brief	イベントのPOST（キューに入れる）
		@param[in]	Delay	配送までの時間
		@param[in]	Sender	送り側オブジェクト（nullptr可）
		@param[in]	Receiver	受け手側オブジェクト
		@param[in]	MsgStr	メッセージ文字列
		@param[in]	Info	追加情報をもつユーザーデータ
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void PostEvent(float Delay, const shared_ptr<ObjectInterface>& Sender, const shared_ptr<ObjectInterface>& Receiver,
			const wstring& MsgStr, const  shared_ptr<void>& Info = shared_ptr<void>());
		//--------------------------------------------------------------------------------------
		/*!
		@brief	イベントのPOST（キューに入れる）
		@param[in]	DispatchTime	POSTする時間（0で次のターン）
		@param[in]	Sender	イベント送信者（nullptr可）
		@param[in]	ReceiverKey	受け手側オブジェクトを判別するキー
		@param[in]	MsgStr	メッセージ
		@param[in,out]	Info	追加情報
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void PostEvent(float DispatchTime, const shared_ptr<ObjectInterface>& Sender, const wstring& ReceiverKey,
			const wstring& MsgStr, const  shared_ptr<void>& Info = shared_ptr<void>());
		//--------------------------------------------------------------------------------------
		/*!
		@brief	イベントのSEND（キューに入れずにそのまま送る）
		@param[in]	Sender	送り側オブジェクト（nullptr可）
		@param[in]	Receiver	受け手側オブジェクト
		@param[in]	MsgStr	メッセージ文字列
		@param[in]	Info	追加情報をもつユーザーデータ
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SendEvent(const shared_ptr<ObjectInterface>& Sender, const shared_ptr<ObjectInterface>& Receiver,
			const wstring& MsgStr, const  shared_ptr<void>& Info = shared_ptr<void>());
		//--------------------------------------------------------------------------------------
		/*!
		@brief	イベントのSEND（キューに入れずにそのまま送る）
		@param[in]	Sender	イベント送信者（nullptr可）
		@param[in]	Receiver	イベント受信者（nullptr不可）
		@param[in]	MsgStr	メッセージ
		@param[in,out]	Info	追加情報
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SendEvent(const shared_ptr<ObjectInterface>& Sender, const wstring& ReceiverKey,
			const wstring& MsgStr, const  shared_ptr<void>& Info = shared_ptr<void>());

		//--------------------------------------------------------------------------------------
		/*!
		@brief	POSTイベントの送信(メインループで呼ばれる)
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void DispatchDelayedEvent();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	キューにたまっているメッセージを削除する
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void ClearEventQ();
	private:
		// pImplイディオム
		struct Impl;
		unique_ptr<Impl> pImpl;
		//コピー禁止
		EventDispatcher(const EventDispatcher&) = delete;
		EventDispatcher& operator=(const EventDispatcher&) = delete;
		//ムーブ禁止
		EventDispatcher(const EventDispatcher&&) = delete;
		EventDispatcher& operator=(const EventDispatcher&&) = delete;
	};


}
// end namespace basecross 

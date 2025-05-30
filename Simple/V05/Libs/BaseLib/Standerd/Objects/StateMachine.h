/*!
@file Stage.h
@brief 親ステージクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#pragma once

#include "stdafx.h"

namespace basecross {

	template <typename T>
	class StateMachine;


	//--------------------------------------------------------------------------------------
	///	ステート実装テンプレートクラス(抽象クラス)
	/*!
	@tparam T	オーナーの型
	*/
	//--------------------------------------------------------------------------------------
	template <typename T>
	class ObjState {
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンストラクタ
		*/
		//--------------------------------------------------------------------------------------
		ObjState() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~ObjState() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ステートに入ったときに実行される
		@param[in]	Obj	ステートを保持するオブジェクト
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void Enter(const shared_ptr<T>& Obj) = 0;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Updateのときに実行される
		@param[in]	Obj	ステートを保持するオブジェクト
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void Execute(const shared_ptr<T>& Obj) = 0;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ステートを出るときに実行される
		@param[in]	Obj	ステートを保持するオブジェクト
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void Exit(const shared_ptr<T>& Obj) = 0;
		//以下、階層化による追加メソッド（純粋仮想関数にしない）
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Update2のときに実行される
		@param[in]	Obj	ステートを保持するオブジェクト
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void Execute2(const shared_ptr<T>& Obj) {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	上にステートがpushされたときに呼ばれる
		@param[in]	Obj	ステートを保持するオブジェクト
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void Sleep(const shared_ptr<T>& Obj) {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	上のステートがpopされたときに呼ばれる
		@param[in]	Obj	ステートを保持するオブジェクト
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void WakeUp(const shared_ptr<T>& Obj) {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ステート名を得る
		@param[in]	ステート名
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual wstring GetStateName()const { return L""; }
	};

	//--------------------------------------------------------------------------------------
	///	ステートマシン実装テンプレートクラス(抽象クラス)
	/*!
	@tparam T	オーナーの型
	*/
	//--------------------------------------------------------------------------------------
	template <typename T>
	class StateMachine
	{
	private:
		//このステートマシンを持つオーナー
		weak_ptr<T> m_Owner;
		//現在のステート
		weak_ptr< ObjState<T> > m_CurrentState;
		//一つ前のステート
		weak_ptr< ObjState<T> > m_PreviousState;
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンストラクタ
		@param[in]	owner	オーナーのポインタ
		*/
		//--------------------------------------------------------------------------------------
		explicit StateMachine(const shared_ptr<T>& owner) :
			m_Owner(owner)
			//m_CurrentState(nullptr), m_PreviousState(nullptr)
		{}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~StateMachine() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	カレントステートを設定する
		@param[in]	s	設定するステート
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetCurrentState(const shared_ptr< ObjState<T> >& s) { m_CurrentState = s; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	1つ前のステートを設定する
		@param[in]	s	設定するステート
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetPreviousState(const shared_ptr< ObjState<T> >& s) { m_PreviousState = s; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	カレントステートを得る
		@return	カレントステート
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr< ObjState<T> >  GetCurrentState() const {
			auto shptr = m_CurrentState.lock();
			if (shptr) {
				return shptr;
			}
			return nullptr;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	1つ前のステートを得る
		@return	1つ前のステート
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr< ObjState<T> >  GetPreviousState()const {
			auto shptr = m_PreviousState.lock();
			if (shptr) {
				return shptr;
			}
			return nullptr;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ステートを更新する
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void Update() const {
			auto shptr = m_CurrentState.lock();
			auto ow_shptr = m_Owner.lock();
			if (shptr && ow_shptr) {
				shptr->Execute(ow_shptr);
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ステートを更新2する
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void Update2() const {
			auto shptr = m_CurrentState.lock();
			auto ow_shptr = m_Owner.lock();
			if (shptr && ow_shptr) {
				shptr->Execute2(ow_shptr);
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ステートを変更する
		@param[in]	NewState	設定するステート
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void  ChangeState(const shared_ptr< ObjState<T> >& NewState) {
			//元のステートを保存
			m_PreviousState = m_CurrentState;
			auto shptr = m_CurrentState.lock();
			auto ow_shptr = m_Owner.lock();
			if (shptr && ow_shptr) {
				//元のステートに終了を知らせる
				shptr->Exit(ow_shptr);
			}
			//新しいステートをカレントに設定
			m_CurrentState = NewState;
			shptr = m_CurrentState.lock();
			if (shptr && ow_shptr) {
				//元のステートに終了を知らせる
				shptr->Enter(ow_shptr);
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ステートを一つ前のステートに戻す
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void RevertToPreviousState() {
			ChangeState(m_PreviousState);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	カレントステートが指定したステートになってるかどうかをチェック
		@param[in]	st	調べるステート
		@return	指定したステートになってればtrue
		*/
		//--------------------------------------------------------------------------------------
		bool IsInState(const shared_ptr< ObjState<T> >& st)const {
			if (st == GetCurrentState()) {
				return true;
			}
			return false;
		}
	};

	//各シングルトンインスタンス作成マクロ
#define DECLARE_SINGLETON_INSTANCE(InsType)	static shared_ptr<InsType> Instance();
#define IMPLEMENT_SINGLETON_INSTANCE(InsType)	shared_ptr<InsType> InsType::Instance() { \
	static shared_ptr<InsType> instance; \
	if(!instance) { instance = shared_ptr<InsType>(new InsType); }return instance;}


}
//end namespace basecross

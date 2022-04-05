/*!
@file Stage.h
@brief 親ステージクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#pragma once

#include "stdafx.h"

namespace basecross {
	//--------------------------------------------------------------------------------------
	// 親ステージクラス
	//--------------------------------------------------------------------------------------
	class Stage : public ObjectInterface {
		//updateするかどうか
		bool m_UpdateActive;

		void PushBackGameObject(const shared_ptr<GameObject>& ptr);
		void RemoveBackGameObject(const shared_ptr<GameObject>& ptr);
		//追加・削除まちオブジェクトの追加と削除
		void RemoveTargetGameObject(const shared_ptr<GameObject>& targetobj);
		void SetWaitToObjectVec();
		//ゲームオブジェクトの配列
		vector<shared_ptr<GameObject>> m_gameObjectVec;
		//追加待ちのゲームオブジェクト
		vector<shared_ptr<GameObject>> m_waitAddObjectVec;
		//削除待ちのゲームオブジェクト
		vector<shared_ptr<GameObject>> m_waitRemoveObjectVec;
		//シェアオブジェクトポインタのマップ
		map<const wstring, weak_ptr<GameObject> > m_SharedMap;
	protected:
		Stage();
		virtual ~Stage() {}
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ゲームオブジェクトを追加する
		@tparam	T	作成する型
		@tparam	Ts	可変長パラメータ
		@return	作成されたゲームオブジェクト
		*/
		//--------------------------------------------------------------------------------------
		template<typename T, typename... Ts>
		shared_ptr<T> AddGameObject(Ts&&... params) {
			try {
				auto Ptr = ObjectFactory::Create<T>(GetThis<Stage>(), params...);
				PushBackGameObject(Ptr);
				return Ptr;
			}
			catch (...) {
				throw;
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ゲームオブジェクトの配列を得る
		@return	ゲームオブジェクトの配列
		*/
		//--------------------------------------------------------------------------------------
		vector<shared_ptr<GameObject>>& GetGameObjectVec() {
			return m_gameObjectVec;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	更新処理が有効かどうかを得る
		@return	更新処理が有効ならtrue
		*/
		//--------------------------------------------------------------------------------------
		bool IsUpdateActive() const {
			return m_UpdateActive;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	更新処理が有効かどうかを設定する
		@param[in]	b	更新処理が有効ならならtrue
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetUpdateActive(bool b) {
			m_UpdateActive = b;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	共有されてるゲームオブジェクトを取得する
		@tparam	T	取得する型
		@param[in]	key	キー
		@param[in]	exceptionActive	見つからないときに例外を発行するかどうか
		@return	共有されてるゲームオブジェクト
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		shared_ptr<T> GetSharedGameObject(const wstring& key, bool exceptionActive = true)const {
			shared_ptr<T> ptr = dynamic_pointer_cast<T>(GetSharedGameObjectEx(key, exceptionActive));
			if (ptr) {
				return ptr;
			}
			else {
				//キャストできない
				if (exceptionActive) {
					//例外発生
					wstring keyerr = key;
					wstring str = L"オブジェクトを";
					str += Util::GetWSTypeName<T>();
					str += L"型にキャストできません";
					throw BaseException(
						str,
						keyerr,
						L"Stage::GetSharedTypedGameObject<T>()"
					);
				}
			}
			return nullptr;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	共有されてるゲームオブジェクトを取得する（GameObject型）
		@param[in]	key	キー
		@param[in]	exceptionActive	見つからないときに例外を発行するかどうか
		@return	共有されてるゲームオブジェクト
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<GameObject> GetSharedGameObjectEx(const wstring& key, bool exceptionActive = true)const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ゲームオブジェクトを共有する
		@param[in]	key	キー
		@param[in]	ptr	ゲームオブジェクトのポインタ
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetSharedGameObject(const wstring& key, const shared_ptr<GameObject>& ptr);

		virtual shared_ptr<BaseCamera> GetActiveCamera() const = 0;
		virtual shared_ptr<LightSet> GetActiveLightSet() const = 0;

		virtual void OnInitFrame(BaseFrame* pBaseFrame);
		virtual void WriteConstantBuffers(BaseFrame* pBaseFrame);
		virtual void UpdateGameObjects();
		virtual void OnPreInit()override {}
		virtual void OnUpdate()override {}
		virtual void OnRender()override;
		virtual void OnDestroy()override;
		virtual void OnKeyDown(UINT8 key)override {}
		virtual void OnKeyUp(UINT8 key) override {}
	};


}
// end namespace basecross

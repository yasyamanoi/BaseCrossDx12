/*!
@file Stage.h
@brief 親ステージクラス
*/


#pragma once

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	// 親ステージクラス
	//--------------------------------------------------------------------------------------
	class Stage : public ObjectInterface {
		//updateするかどうか
		bool m_updateActive;
		void PushBackGameObject(const std::shared_ptr<GameObject>& ptr);
		void RemoveBackGameObject(const std::shared_ptr<GameObject>& ptr);
		//追加・削除まちオブジェクトの追加と削除
		void RemoveTargetGameObject(const std::shared_ptr<GameObject>& targetobj);
		void SetWaitToObjectVec();
		//ゲームオブジェクトの配列
		std::vector<std::shared_ptr<GameObject>> m_gameObjectVec;
		//追加待ちのゲームオブジェクト
		std::vector<std::shared_ptr<GameObject>> m_waitAddObjectVec;
		//削除待ちのゲームオブジェクト
		std::vector<std::shared_ptr<GameObject>> m_waitRemoveObjectVec;
		//シェアオブジェクトポインタのマップ
		std::map<const std::wstring, std::weak_ptr<GameObject> > m_SharedMap;
		//シェアグループのポインタのマップ
		std::map<const std::wstring, std::shared_ptr<GameObjectGroup> >  m_SharedGroupMap;

		//コリジョン管理者
		std::shared_ptr<CollisionManager> m_collisionManager;
	protected:
		ID3D12Device* m_pDevice;
		//std::shared_ptr<Camera> m_camera;
		//std::shared_ptr<LightSet> m_lightSet;
		Stage(ID3D12Device* pDevice) :
			m_pDevice(pDevice),
			m_updateActive(true)
		{
		}
		virtual ~Stage() {}
	public:
		template<typename T, typename... Ts>
		std::shared_ptr<T> AddGameObject(Ts&&... params) {
			try {
				auto ptr = ObjectFactory::Create<T>(GetThis<Stage>(), params...);
				m_gameObjectVec.push_back(ptr);
				return ptr;
			}
			catch (...) {
				throw;
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	パラメータをOnCreateWithParamに渡すゲームオブジェクトを追加する
		@tparam	T	作成する型
		@tparam	Ts	可変長パラメータの型
		@param[in]	params	可変長パラメータ
		@return	作成されたゲームオブジェクト
		*/
		//--------------------------------------------------------------------------------------
		template<typename T, typename... Ts>
		std::shared_ptr<T> AddGameObjectWithParam(Ts&&... params) {
			try {
				auto Ptr = ObjectFactory::CreateGameObjectWithParam<T>(GetThis<Stage>(), params...);
				PushBackGameObject(Ptr);
				return Ptr;
			}
			catch (...) {
				throw;
			}
		}
		//派生クラスで必ず記述する仮想関数
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ターゲットとなるカメラを得る（派生クラスで定義）
		@return	カメラのポインタ
		*/
		//--------------------------------------------------------------------------------------
		virtual std::shared_ptr<Camera> GetCamera() const = 0;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ターゲットとなるライトを得る（派生クラスで定義）
		@return	ライトのポインタ
		*/
		//--------------------------------------------------------------------------------------
		virtual std::shared_ptr<LightSet> GetLightSet() const = 0;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	カメラとライトを更新する
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void UpdateCameraLight(double elapsedTime) = 0;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	更新処理が有効かどうかを得る
		@return	更新処理が有効ならtrue
		*/
		//--------------------------------------------------------------------------------------
		bool IsUpdateActive() const {
			return m_updateActive;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	更新処理が有効かどうかを設定する
		@param[in]	b	更新処理が有効ならならtrue
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetUpdateActive(bool b) {
			m_updateActive = b;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	衝突判定マネージャを得る(const)
		@return	衝突判定マネージャ
		*/
		//--------------------------------------------------------------------------------------
		std::shared_ptr<CollisionManager> GetCollisionManager() const {
			return m_collisionManager;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ゲームオブジェクトの配列を得る
		@return	ゲームオブジェクトの配列
		*/
		//--------------------------------------------------------------------------------------
		std::vector<std::shared_ptr<GameObject>>& GetGameObjectVec() {
			return m_gameObjectVec;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ゲームオブジェクトの配列を得る(const)
		@return	ゲームオブジェクトの配列
		*/
		//--------------------------------------------------------------------------------------
		const std::vector< std::shared_ptr<GameObject> >& GetGameObjectVec() const {
			return m_gameObjectVec;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ゲームオブジェクトを削除する
		@param[in]	Obj	インスタンス
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void RemoveGameObject(const std::shared_ptr<GameObject>& Obj);
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
		std::shared_ptr<T> GetSharedGameObjectEx(const std::wstring& key, bool exceptionActive = true)const {
			shared_ptr<T> ptr = std::dynamic_pointer_cast<T>(GetSharedGameObject(key, exceptionActive));
			if (ptr) {
				return ptr;
			}
			else {
				//キャストできない
				if (exceptionActive) {
					//例外発生
					std::wstring keyerr = key;
					std::wstring str = L"オブジェクトを";
					str += Util::GetWSTypeName<T>();
					str += L"型にキャストできません";
					throw BaseException(
						str,
						keyerr,
						L"Stage::GetSharedGameObjectEx<T>()"
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

		std::shared_ptr<GameObject> GetSharedGameObject(const std::wstring& key, bool exceptionActive = true)const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ゲームオブジェクトを共有する
		@param[in]	key	キー
		@param[in]	ptr	ゲームオブジェクトのポインタ
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetSharedGameObject(const std::wstring& key, const std::shared_ptr<GameObject>& ptr);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	共有するゲームオブジェクトグループを作成する
		@param[in]	Key	キー
		@return	共有するゲームオブジェクトグループ
		*/
		//--------------------------------------------------------------------------------------
		std::shared_ptr<GameObjectGroup> CreateSharedObjectGroup(const std::wstring& Key);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	共有するゲームオブジェクトグループを得る
		@param[in]	Key	キー
		@param[in]	ExceptionActive	見つからないときに例外を発行するかどうか
		@return	共有するゲームオブジェクトグループ
		*/
		//--------------------------------------------------------------------------------------
		std::shared_ptr<GameObjectGroup> GetSharedObjectGroup(const std::wstring& Key, bool ExceptionActive = true)const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	共有するゲームオブジェクトグループを得る（グループ派生クラスを作った場合用）
		@tparam	T	取得する型
		@param[in]	Key	キー
		@param[in]	ExceptionActive	見つからないときに例外を発行するかどうか
		@return	共有するゲームオブジェクトグループ
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		std::shared_ptr<T> GetSharedObjectGroup(const std::wstring& Key, bool ExceptionActive = true)const {
			auto RetPtr = std::dynamic_pointer_cast<T>(GetSharedObjectGroup(Key, ExceptionActive));
			if (RetPtr) {
				return RetPtr;
			}
			else {
				if (ExceptionActive) {
					//例外発生
					std::wstring keyerr = Key;
					throw BaseException(
						L"指定のキーのグループはT型に変換できません",
						keyerr,
						L"Stage::GetSharedObjectGroup<T>()"
					);
				}
			}
			return nullptr;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	共有するゲームオブジェクトグループをセットする（グループ派生クラスを作った場合用）
		@param[in]	Key	キー
		@param[in]	NewPtr	共有するゲームオブジェクトグループ
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetSharedObjectGroup(const std::wstring& Key, const std::shared_ptr<GameObjectGroup>& NewPtr);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	指定のタグをもつゲームオブジェクトの配列を取得する
		@param[in]	Tag	検索するタグ
		@param[out]	取得するゲームオブジェクトの配列
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void GetUsedTagObjectVec(const std::wstring& Tag, std::vector<std::shared_ptr<GameObject>>& ObjVec) const {
			for (auto& v : GetGameObjectVec()) {
				if (v->FindTag(Tag)) {
					ObjVec.push_back(v);
				}
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ゲームオブジェクトで指定のコンポーネントの親か子のコンポーネントを持つ場合そのコンポーネントの配列を取得する
		@tparam	T	検索するコンポーネント型
		@param[out]	取得するコンポーネントの配列
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		void GetUsedDynamicCompoentVec(std::vector<std::shared_ptr<T>>& CompVec) const {
			for (auto& v : GetGameObjectVec()) {
				auto ptr = v->GetDynamicComponent<T>(false);
				if (ptr) {
					CompVec.push_back(ptr);
				}
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	指定のコンポーネントの親か子のコンポーネントを持つオブジェクトの配列を設定する
		@tparam	T	検索するコンポーネント型
		@param[out]	取得するゲームオブジェクトの配列
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		void GetUsedDynamicCompoentObjectVec(std::vector<std::shared_ptr<GameObject>>& GameObjectVec)const {
			for (auto& v : GetGameObjectVec()) {
				auto ptr = v->GetDynamicComponent<T>(false);
				if (ptr) {
					GameObjectVec.push_back(v);
				}
			}
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	ステージ内の更新（シーンからよばれる）
		@param[in]	elapsedTime	前回のターンからの経過時間
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void UpdateStage(double elapsedTime);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	衝突判定の更新（ステージから呼ぶ）
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void UpdateCollision();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンスタントバッファの更新
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnUpdateConstantBuffers();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンスタントバッファの確定
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCommitConstantBuffers();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	トランスフォームのバックアップ
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void SetToBefore();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	クリエイト前の処理
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnPreCreate()override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	クリエイト処理
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCreate()override {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	更新処理
		@param[in]	elapsedTime	前のターンからの経過時間
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnUpdate(double elapsedTime)override {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	更新処理２
		@param[in]	elapsedTime	前のターンからの経過時間
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnUpdate2(double elapsedTime)override {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	シャドウマップの描画
		@param[in]	pCommandList	コマンドリスト
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnShadowDraw(ID3D12GraphicsCommandList* pCommandList)override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	シーンの描画
		@param[in]	pCommandList	コマンドリスト
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnSceneDraw(ID3D12GraphicsCommandList* pCommandList)override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	破棄時処理
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnDestroy()override;
	};



}
//end namespace basecross


/*!
@file Stage.h
@brief 親ステージクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#pragma once

#include "stdafx.h"

namespace basecross {
	class CollisionManager;
	//--------------------------------------------------------------------------------------
	// 親ステージクラス
	//--------------------------------------------------------------------------------------
	class Stage : public ObjectInterface {
		//updateするかどうか
		bool m_updateActive;
		//シャドウマップを使うかどうか
		bool m_isShadowmapRender;

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
		//シェアグループのポインタのマップ
		map<const wstring, shared_ptr<GameObjectGroup> >  m_SharedGroupMap;

		//コリジョン管理者
		shared_ptr<CollisionManager> m_collisionManager;

		//物理計算を使うかどうか
		bool m_physicsActive;
		//物理計算
		BasePhysics m_basePhysics;
		//物理計算で使う空物理オブジェクトのset
		set<uint16_t> m_vacantPhysicsIndices;

		//衝突判定マネージャのパフォーマンスカウンタ
		PerformanceCounter m_collisionPerformance;
		//Updateのパフォーマンスカウンタ
		PerformanceCounter m_updatePerformance;
		//Renderのパフォーマンスカウンタ
		PerformanceCounter m_renderPerformance;


	protected:
		Stage();
		virtual ~Stage() {}
	public:
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
		@brief	シャドウマップを使うかどうかを得る
		@return	シャドウマップを使うかどうか
		*/
		//--------------------------------------------------------------------------------------
		bool IsShadowmapRender() const {
			return m_isShadowmapRender;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	シャドウマップを使うかどうかを設定する
		@param[in]	b	シャドウマップを使うかどうか
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetShadowmapRender(bool b) {
			m_isShadowmapRender = b;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	物理計算オブジェクトを得る
		@return	物理計算オブジェクト
		*/
		//--------------------------------------------------------------------------------------
		const BasePhysics& GetBasePhysics() const {
			if (!IsPhysicsActive()) {
				throw BaseException(
					L"物理演算が無効になっています。有効にしてから取得してください。",
					L"if (!IsPhysicsActive())",
					L"Stage::GetBasePhysics()"
				);
			}
			return m_basePhysics;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	物理計算が有効かどうかを得る
		@return	物理計算が有効かどうか
		*/
		//--------------------------------------------------------------------------------------
		bool IsPhysicsActive() const {
			return m_physicsActive;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	物理計算が有効かどうかを設定する
		@param[in]	b	物理計算が有効かどうか
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetPhysicsActive(bool b) {
			m_physicsActive = b;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	物理オブジェクトの空いているIDを取得する
		@return	 空いているID(空いてなければUINT16_MAXを返す）
		*/
		//--------------------------------------------------------------------------------------
		uint16_t GetVacantPhysicsIndex();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	無効になった物理オブジェクトIDを追加する
		@param[in]	index	無効にするID
		@return	 なし
		*/
		//--------------------------------------------------------------------------------------
		void SetVacantPhysicsIndex(uint16_t index);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	衝突判定マネージャを得る(const)
		@return	衝突判定マネージャ
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<CollisionManager> GetCollisionManager() const {
			return m_collisionManager;
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
		@brief	ゲームオブジェクトの配列を得る(const)
		@return	ゲームオブジェクトの配列
		*/
		//--------------------------------------------------------------------------------------
		const vector< shared_ptr<GameObject> >& GetGameObjectVec() const {
			return m_gameObjectVec;
		}
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
		@brief	パラメータをOnCreateWithParamに渡すゲームオブジェクトを追加する
		@tparam	T	作成する型
		@tparam	Ts	可変長パラメータの型
		@param[in]	params	可変長パラメータ
		@return	作成されたゲームオブジェクト
		*/
		//--------------------------------------------------------------------------------------
		template<typename T, typename... Ts>
		shared_ptr<T> AddGameObjectWithParam(Ts&&... params) {
			try {
				auto Ptr = ObjectFactory::CreateGameObjectWithParam<T>(GetThis<Stage>(), params...);
				PushBackGameObject(Ptr);
				return Ptr;
			}
			catch (...) {
				throw;
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ゲームオブジェクトを削除する
		@param[in]	Obj	インスタンス
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void RemoveGameObject(const shared_ptr<GameObject>& Obj);
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
		shared_ptr<T> GetSharedGameObjectEx(const wstring& key, bool exceptionActive = true)const {
			shared_ptr<T> ptr = dynamic_pointer_cast<T>(GetSharedGameObject(key, exceptionActive));
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
		shared_ptr<GameObject> GetSharedGameObject(const wstring& key, bool exceptionActive = true)const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ゲームオブジェクトを共有する
		@param[in]	key	キー
		@param[in]	ptr	ゲームオブジェクトのポインタ
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetSharedGameObject(const wstring& key, const shared_ptr<GameObject>& ptr);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	共有するゲームオブジェクトグループを作成する
		@param[in]	Key	キー
		@return	共有するゲームオブジェクトグループ
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<GameObjectGroup> CreateSharedObjectGroup(const wstring& Key);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	共有するゲームオブジェクトグループを得る
		@param[in]	Key	キー
		@param[in]	ExceptionActive	見つからないときに例外を発行するかどうか
		@return	共有するゲームオブジェクトグループ
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<GameObjectGroup> GetSharedObjectGroup(const wstring& Key, bool ExceptionActive = true)const;
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
		shared_ptr<T> GetSharedObjectGroup(const wstring& Key, bool ExceptionActive = true)const {
			auto RetPtr = dynamic_pointer_cast<T>(GetSharedObjectGroup(Key, ExceptionActive));
			if (RetPtr) {
				return RetPtr;
			}
			else {
				if (ExceptionActive) {
					//例外発生
					wstring keyerr = Key;
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
		void SetSharedObjectGroup(const wstring& Key, const shared_ptr<GameObjectGroup>& NewPtr);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	指定のタグをもつゲームオブジェクトの配列を取得する
		@param[in]	Tag	検索するタグ
		@param[out]	取得するゲームオブジェクトの配列
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void GetUsedTagObjectVec(const wstring& Tag, vector<shared_ptr<GameObject>>& ObjVec) const {
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
		void GetUsedDynamicCompoentVec(vector<shared_ptr<T>>& CompVec) const {
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
		void GetUsedDynamicCompoentObjectVec(vector<shared_ptr<GameObject>>& GameObjectVec)const {
			for (auto& v : GetGameObjectVec()) {
				auto ptr = v->GetDynamicComponent<T>(false);
				if (ptr) {
					GameObjectVec.push_back(v);
				}
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	アクティブなカメラを返す（純粋仮想関数）
		@return	アクティブなカメラ
		*/
		//--------------------------------------------------------------------------------------
		virtual shared_ptr<BaseCamera> GetActiveCamera() const = 0;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	アクティブなライトセットを返す（純粋仮想関数）
		@return	アクティブなライトセット
		*/
		//--------------------------------------------------------------------------------------
		virtual shared_ptr<LightSet> GetActiveLightSet() const = 0;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	衝突判定のパフォーマンスを有効無効にする
		@param[in]	b	衝突判定のパフォーマンスを有効にするかどうか
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetCollisionPerformanceActive(bool b) {
			m_collisionPerformance.SetActive(b);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	衝突判定のパフォーマンスを有効無効を得る
		@return	衝突判定のパフォーマンスを有効ならtrue
		*/
		//--------------------------------------------------------------------------------------
		bool IsCollisionPerformanceActive() const {
			return m_collisionPerformance.IsAvtive();
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	衝突判定のパフォーマンスを得る
		@return	衝突判定のパフォーマンス（ミリ秒）
		*/
		//--------------------------------------------------------------------------------------
		float GetCollisionPerformanceTime() const {
			return m_collisionPerformance.GetPerformanceTime();
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Update処理のパフォーマンスを有効無効にする
		@param[in]	b	Update処理のパフォーマンスを有効にするかどうか
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetUpdatePerformanceActive(bool b) {
			m_updatePerformance.SetActive(b);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Update処理のパフォーマンスを有効無効を得る
		@return	Update処理のパフォーマンスを有効ならtrue
		*/
		//--------------------------------------------------------------------------------------
		bool IsUpdatePerformanceActive() const {
			return m_updatePerformance.IsAvtive();
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Update処理のパフォーマンスを得る
		@return	Update処理のパフォーマンス（ミリ秒）
		*/
		//--------------------------------------------------------------------------------------
		float GetUpdatePerformanceTime() const {
			return m_updatePerformance.GetPerformanceTime();
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Render処理のパフォーマンスを有効無効にする
		@param[in]	b	Render処理のパフォーマンスを有効にするかどうか
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetRenderPerformanceActive(bool b) {
			m_renderPerformance.SetActive(b);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Render処理のパフォーマンスを有効無効を得る
		@return	Render処理のパフォーマンスを有効ならtrue
		*/
		//--------------------------------------------------------------------------------------
		bool IsRenderPerformanceActive() const {
			return m_renderPerformance.IsAvtive();
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Render処理のパフォーマンスを得る
		@return	Render処理のパフォーマンス（ミリ秒）
		*/
		//--------------------------------------------------------------------------------------
		float GetRenderPerformanceTime() const {
			return m_renderPerformance.GetPerformanceTime();

		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	フレームを初期化する
		@param[in]	pBaseFrame	ターゲットとなるフレーム
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnInitFrame(BaseFrame* pBaseFrame);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンスタントバッファを書き込む
		@param[in]	pBaseFrame	ターゲットとなるフレーム
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void WriteConstantBuffers(BaseFrame* pBaseFrame);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ステージ内の更新（シーンからよばれる）
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void UpdateStage();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	衝突判定の更新（ステージから呼ぶ）
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void UpdateCollision();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	更新準備（構築時によばれる）
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnPreInit()override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	更新
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnUpdate()override {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	後更新
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnUpdate2() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	カメラとライト
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnUpdateLightCamera() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	描画
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnRender()override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	シャドウマップ描画
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnShadowmapRender();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	破棄時に呼ばれる
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnDestroy()override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	キーの押下
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnKeyDown(UINT8 key)override {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	キーを離す
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnKeyUp(UINT8 key) override {}

		//BaseCrossDx12で実装停止
		// ステージの親子関係は停止
		//vector< shared_ptr<Stage> >& GetChileStageVec();
		//vector< shared_ptr<Stage> >& GetChileStageVec() const;
		//template<typename T>
		//shared_ptr<Stage> AddChileStage();
		//shared_ptr<Stage> GetParentStage() const;
		//template<typename T>
		//bool FindGameObject(const shared_ptr<GameObject>& Obj);

	};


}
// end namespace basecross

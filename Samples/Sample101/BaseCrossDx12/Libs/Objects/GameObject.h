/*!
@file GameObject.h
@brief ゲームオブジェクトクラス
*/


#pragma once
#include "stdafx.h"


namespace basecross {

	class Stage;
	class Behavior;
	class Collision;
	struct CollisionPair;

	//--------------------------------------------------------------------------------------
	// 配置されるオブジェクトの親
	//--------------------------------------------------------------------------------------
	class GameObject : public ObjectInterface {
		std::weak_ptr<Stage> m_stage;

		//updateするかどうか
		bool m_updateActive;
		//drawするかどうか
		bool m_drawActive;
		//透明かどうか
		bool m_alphaActive;


		//行動のマップ
		std::map<std::type_index, std::shared_ptr<Behavior>> m_BehaviorMap;
		//コンポーネントのマップ
		std::map<std::type_index, std::shared_ptr<Component> > m_compMap;
		//コンポーネント実行順番
		std::list<std::type_index> m_compOrder;
		//指定のコンポーネントを探す（派生クラスも検索）
		template<typename T>
		std::shared_ptr<T> SearchDynamicComponent()const {
			auto it = m_compMap.begin();
			while (it != m_compMap.end()) {
				auto ptr = std::dynamic_pointer_cast<T>(it->second);
				if (ptr) {
					return ptr;
				}
				it++;
			}
			return nullptr;
		}


		int m_drawLayer = 0;	//描画レイヤー
		std::set<std::wstring> m_tagSet;	//タグのセット
		std::set<int> m_numTagSet;	//数字タグのセット


		const std::map<std::type_index, std::shared_ptr<Behavior> >& GetBehaviorMap() const {
			return m_BehaviorMap;
		}
		std::map<std::type_index, std::shared_ptr<Behavior> >& GetBehaviorMap() {
			return m_BehaviorMap;
		}
		std::shared_ptr<Behavior> SearchBehavior(std::type_index TypeIndex)const {
			auto it = m_BehaviorMap.find(TypeIndex);
			if (it != m_BehaviorMap.end()) {
				return it->second;
			}
			return nullptr;

		}
		void AddMakedBehavior(std::type_index TypeIndex, const std::shared_ptr<Behavior>& Ptr) {
			//mapに追加もしくは更新
			m_BehaviorMap[TypeIndex] = Ptr;
		}
	protected:
		TransParam m_tempParam;
		GameObject(const std::shared_ptr<Stage>& stage):
			m_stage(stage),
			m_tempParam(),
			m_updateActive(true),
			m_drawActive(true),
			m_alphaActive(false)
		{
		}
		virtual ~GameObject() {}


	public:
		std::shared_ptr<Stage> GetStage(bool exceptionActive = true) const;
		bool IsUpdateActive() const {
			return m_updateActive;
		}
		void SetUpdateActive(bool b) {
			m_updateActive = b;
		}
		bool IsDrawActive() const {
			return m_drawActive;
		}
		void SetDrawActive(bool b) {
			m_drawActive = b;
		}
		bool IsAlphaActive() const {
			return m_alphaActive;
		}
		void SetAlphaActive(bool b) {
			m_alphaActive = b;
		}

		virtual std::shared_ptr<Camera> GetCamera()const;
		virtual std::shared_ptr<LightSet> GetLightSet() const;


		template<typename T, typename... Ts>
		std::shared_ptr<T> AddComponent(Ts&&... params) {
			std::type_index t_index = std::type_index(typeid(T));
			auto ptr = SearchDynamicComponent<T>();
			//指定の型のコンポーネントが見つかった
			if (ptr) {
				auto retPtr = std::dynamic_pointer_cast<T>(ptr);
				if (retPtr) {
					return retPtr;
				}
				else {
					throw BaseException(
						L"すでにコンポーネントがありましたが、型キャストできません",
						Util::GetWSTypeName<T>(),
						L"GameObject::AddComponent<T>()"
					);
				}
			}
			else {
				//見つからない。新たに作成する
				std::shared_ptr<T> newPtr = ObjectFactory::Create<T>(GetThis<GameObject>(), params...);
				//そのコンポーネントがまだなければ新規登録
				m_compOrder.push_back(t_index);
				//mapに追加もしくは更新
				m_compMap[t_index] = newPtr;
				newPtr->AttachGameObject(GetThis<GameObject>());
				return newPtr;
			}
		}


		template <typename T>
		std::shared_ptr<T> GetComponent(bool exceptionActive = true)const {
			auto Ptr = SearchDynamicComponent<T>();
			if (Ptr) {
				//指定の型のコンポーネントが見つかった
				return Ptr;
			}
			else {
				if (exceptionActive) {
					throw BaseException(
						L"コンポーネントが見つかりません",
						Util::GetWSTypeName<T>(),
						L"GameObject::GetComponent<T>()"
					);
				}
			}
			return nullptr;
		}

		template <typename T>
		void RemoveComponent() {
			auto typeIndex = type_index(typeid(T));
			//順番リストを検証して削除
			auto it = m_compOrder.begin();
			while (it != m_compOrder.end()) {
				if (*it == typeIndex) {
					auto it2 = m_compMap.find(*it);
					if (it2 != m_compMap.end()) {
						//指定の型のコンポーネントが見つかった
						//mapデータを削除
						m_compMap.erase(it2);
					}
					m_compOrder.erase(it);
					return;
				}
				it++;
			}
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	行動の取得。存在しなければ作成する
		@tparam	T	取得する型
		@return	コンポーネント
		*/
		//--------------------------------------------------------------------------------------
		template <typename T>
		std::shared_ptr<T> GetBehavior() {
			auto Ptr = SearchBehavior(std::type_index(typeid(T)));
			if (Ptr) {
				//指定の型の行動が見つかった
				auto RetPtr = std::dynamic_pointer_cast<T>(Ptr);
				if (RetPtr) {
					return RetPtr;
				}
				else {
					throw BaseException(
						L"行動がありましたが、型キャストできません",
						Util::GetWSTypeName<T>(),
						L"GameObject::GetBehavior<T>()"
					);
				}
			}
			else {
				//無ければ新たに制作する
				std::shared_ptr<T> newPtr = ObjectFactory::Create<T>(GetThis<GameObject>());
				AddMakedBehavior(std::type_index(typeid(T)), newPtr);
				return newPtr;
			}
			return nullptr;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	行動の検索。
		@tparam	T	取得する型
		@return	存在すればtrue
		*/
		//--------------------------------------------------------------------------------------
		template <typename T>
		bool FindBehavior() {
			auto Ptr = SearchBehavior(type_index(typeid(T)));
			if (Ptr) {
				//指定の型の行動が見つかった
				auto RetPtr = dynamic_pointer_cast<T>(Ptr);
				if (RetPtr) {
					return true;
				}
				else {
					return false;
				}
			}
			return false;
		}


		//--------------------------------------------------------------------------------------
		/*!
		@brief	タグのセットを得る(const)
		@return	タグのセット
		*/
		//--------------------------------------------------------------------------------------
		const std::set<std::wstring>& GetTagSet() const {
			return m_tagSet;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	タグのセットを得る
		@return	タグのセット
		*/
		//--------------------------------------------------------------------------------------
		std::set<std::wstring>& GetTagSet() {
			return m_tagSet;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	指定するタグが存在するかどうかを得る
		@param[in]	tagstr	検証するタグ
		@return	存在すればtrue
		*/
		//--------------------------------------------------------------------------------------
		bool FindTag(const std::wstring& tagstr) const {
			if (m_tagSet.find(tagstr) == m_tagSet.end()) {
				return false;
			}
			return true;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	指定するタグを追加する
		@param[in]	tagstr	追加するタグ
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void  AddTag(const std::wstring& tagstr) {
			if (tagstr == L"") {
				//空白なら例外
				throw BaseException(
					L"設定するタグが空です",
					L"if (tagstr == L"")",
					L"GameObject::AddTag()"
				);
			}
			m_tagSet.insert(tagstr);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	指定するタグが存在したら削除する（存在しない場合は何もしない）
		@param[in]	tagstr	削除するタグ
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void  RemoveTag(const std::wstring& tagstr) {
			m_tagSet.erase(tagstr);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	数字タグのセットを得る(const)
		@return	数字タグのセット
		*/
		//--------------------------------------------------------------------------------------
		const std::set<int>& GetNumTagSet() const {
			return m_numTagSet;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	数字タグのセットを得る
		@return	数字タグのセット
		*/
		//--------------------------------------------------------------------------------------
		std::set<int>& GetNumTagSet() {
			return m_numTagSet;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	指定する数字タグが存在するかどうかを得る
		@param[in]	numtag	検証する数字タグ
		@return	存在すればtrue
		*/
		//--------------------------------------------------------------------------------------
		bool FindNumTag(int numtag) const {
			if (m_numTagSet.find(numtag) == m_numTagSet.end()) {
				return false;
			}
			return true;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	指定する数字タグを追加する
		@param[in]	numtag	追加する数字タグ
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void  AddNumTag(int numtag) {
			m_numTagSet.insert(numtag);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	指定する数字タグが存在したら削除する（存在しない場合は何もしない）
		@param[in]	numtag	削除する数字タグ
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void  RemoveNumTag(int numtag) {
			m_numTagSet.erase(numtag);
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	衝突発生時時のイベント（デフォルトは何もしない）。複数あった場合は複数回呼ばれる
		@param[in]	Other	新しく衝突した相手
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCollisionEnter(std::shared_ptr<GameObject>& Other) {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	衝突発生時時のイベント（デフォルトは何もしない）。複数あった場合は複数回呼ばれる
		@param[in]	Pair ペア
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCollisionEnter(const CollisionPair& Pair) {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	衝突し続ける相手があった場合のイベント（デフォルトは何もしない）。複数あった場合は複数回呼ばれる
		@param[in]	OtherVec	衝突し続けた相手
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCollisionExcute(std::shared_ptr<GameObject>& Other) {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	衝突し続ける相手があった場合のイベント（デフォルトは何もしない）。複数あった場合は複数回呼ばれる
		@param[in]	Pair ペア
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCollisionExcute(const CollisionPair& Pair) {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	衝突を抜けた相手があった場合のイベント（デフォルトは何もしない）。複数あった場合は複数回呼ばれる
		@param[in]	OtherVec	衝突を抜けた相手の配列
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCollisionExit(std::shared_ptr<GameObject>& Other) {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	衝突を抜けた相手があった場合のイベント（デフォルトは何もしない）。複数あった場合は複数回呼ばれる
		@param[in]	Pair ペア
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCollisionExit(const CollisionPair& Pair) {}

		void ComponentUpdate();
		void TransformInit();
		void ComponentShadowmapRender();
		void ComponentRender();
		void ComponentDestroy();

		virtual void OnUpdateConstantBuffers();
		virtual void OnCommitConstantBuffers();
		virtual void OnPreCreate()override;
		virtual void OnCreate()override {}
		virtual void OnShadowDraw()override;
		virtual void OnSceneDraw()override;
		virtual void OnDestroy()override {}
		virtual void SetToBefore();

	};


	//--------------------------------------------------------------------------------------
	///	ゲームオブジェクトのweak_ptrをグループ化したもの
	//--------------------------------------------------------------------------------------
	class GameObjectGroup : public ObjectInterface {
		std::vector< std::weak_ptr<GameObject> > m_Group;
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンストラクタ
		*/
		//--------------------------------------------------------------------------------------
		GameObjectGroup();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~GameObjectGroup();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	グループ内のゲームオブジェクトのweak_ptrの配列を得る
		@return	グループ内のゲームオブジェクトのweak_ptrの配列
		*/
		//--------------------------------------------------------------------------------------
		const std::vector< std::weak_ptr<GameObject> >& GetGroupVector() const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	グループ内のゲームオブジェクトを得る
		@param[in]	index	グループ内オブジェクトのインデックス
		@return	グループ内のゲームオブジェクト
		*/
		//--------------------------------------------------------------------------------------
		std::shared_ptr<GameObject> at(size_t index);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	グループ内のゲームオブジェクトの数を得る
		@return	グループ内のゲームオブジェクトの数
		*/
		//--------------------------------------------------------------------------------------
		size_t size() const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	グループにゲームオブジェクトを追加する
		@param[in]	Obj	追加するゲームオブジェクト
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void IntoGroup(const std::shared_ptr<GameObject>& Obj);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	グループをクリアする
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void AllClear();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	初期化
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCreate()override {}
		virtual void OnUpdate(double elapsedTime) override{}
		virtual void OnShadowDraw()override{}
		virtual void OnSceneDraw()override{}
		virtual void OnDestroy() override {}
	};
}
// end namespace basecross

/*!
@file GameObject.h
@brief ゲームオブジェクトクラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#pragma once
#include "stdafx.h"


namespace basecross {

	
	

	class Stage;

	//--------------------------------------------------------------------------------------
	// 配置されるオブジェクトの親
	//--------------------------------------------------------------------------------------
	class GameObject : public ObjectInterface {
		std::weak_ptr<Stage> m_stage;
		TransParam m_tempParam;
	protected:
		GameObject(const std::shared_ptr<Stage>& stage,const TransParam& param):
			m_stage(stage),
			m_tempParam(param)
		{
		}
		virtual ~GameObject() {}

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

	public:
		std::shared_ptr<Stage> GetStage(bool exceptionActive = true) const;
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
		virtual void OnUpdateConstantBuffers() = 0;
		virtual void OnCommitConstantBuffers() = 0;
//		virtual void OnUpdateConstantBuffers(Scene* scene, shared_ptr<Stage>& stage) = 0;
//		virtual void OnCommitConstantBuffers(Scene* scene, shared_ptr<Stage>& stage) = 0;
		virtual void OnPreCreate()override;
		virtual void OnCreate()override {}
		virtual void OnShadowDraw()override {}
		virtual void OnSceneDraw()override {}
		virtual void OnDestroy()override {}
		virtual void SetToBefore();

	};
/*
	//--------------------------------------------------------------------------------------
	///	ゲームオブジェクトのweak_ptrをグループ化したもの
	//--------------------------------------------------------------------------------------
	class GameObjectGroup : public ObjectInterface {
		vector< weak_ptr<GameObject> > m_Group;
	public:
		GameObjectGroup();
		virtual ~GameObjectGroup();
		const vector< weak_ptr<GameObject> >& GetGroupVector() const;
		shared_ptr<GameObject> at(size_t index);
		size_t size() const;
		void IntoGroup(const shared_ptr<GameObject>& Obj);
		void AllClear();
		virtual void OnCreate()override {}
		virtual void OnUpdate()override {}
		virtual void OnDraw() override {}
		virtual void OnDestroy() override {}
	};
*/
}
// end namespace basecross

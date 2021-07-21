/*!
@file GameObject.h
@brief ゲームオブジェクトクラス
@copyright Copyright (c) 2021 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	class BaseScene;
	class BaseStage;

	//--------------------------------------------------------------------------------------
	// 配置されるオブジェクト
	//--------------------------------------------------------------------------------------
	class GameObject : public ObjectInterface {
		bool m_UpdateActive;	//updateするかどうか
		bool m_renderActive;		//Renderするかどうか
		bool m_AlphaActive;		//透明かどうか

		map<type_index, shared_ptr<Component> > m_compMap;
		list<type_index> m_compOrder;	//コンポーネント実行順番

		template<typename T>
		shared_ptr<T> SearchDynamicComponent()const {
			auto it = m_compMap.begin();
			while (it != m_compMap.end()) {
				auto ptr = dynamic_pointer_cast<T>(it->second);
				if (ptr) {
					return ptr;
				}
				it++;
			}
			return nullptr;
		}

	protected:
		explicit GameObject(const shared_ptr<BaseStage>& stage);
		virtual ~GameObject();
		//フレーム内のCBV関連パラメータのインデックス
		size_t m_paramIndex;
		//ステージ親のポインタ
		weak_ptr<BaseStage> m_stage;
		//コンスタントバッファの登録
		void SetConstants(BasicConstants& constants, const shared_ptr<Transform>& transform);
	public:
		bool IsUpdateActive() const {
			return m_UpdateActive;
		}
		void SetUpdateActive(bool b) {
			m_UpdateActive = b;
		}
		bool IsRenderActive() const {
			return m_renderActive;
		}
		void SetRenderActive(bool b) {
			m_renderActive = b;
		}
		bool IsAlphaActive() const {
			return m_AlphaActive;
		}
		void SetAlphaActive(bool b) {
			m_AlphaActive = b;
		}
		size_t GetParamIndex() const {
			return m_paramIndex;
		}
		shared_ptr<BaseStage> GetBaseStage(bool exceptionActive = true) const;
		template<typename T, typename... Ts>
		shared_ptr<T> AddComponent(Ts&&... params) {
			type_index t_index = type_index(typeid(T));
			auto ptr = SearchDynamicComponent<T>();
			//指定の型のコンポーネントが見つかった
			if (ptr) {
				auto retPtr = dynamic_pointer_cast<T>(ptr);
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
				shared_ptr<T> newPtr = ObjectFactory::Create<T>(GetThis<GameObject>(), params...);
				//そのコンポーネントがまだなければ新規登録
				m_compOrder.push_back(t_index);
				//mapに追加もしくは更新
				m_compMap[t_index] = newPtr;
				newPtr->AttachGameObject(GetThis<GameObject>());
				return newPtr;
			}
		}


		template <typename T>
		shared_ptr<T> GetComponent(bool exceptionActive = true)const {
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


		void TransformInit();
		void ComponentUpdate();
		void ComponentRender();
		void ComponentDestroy();

		virtual void OnPreInit() override;
		virtual void OnRender()override;
		virtual void OnInitFrame(FrameResource* pFrameResource);
		virtual void WriteConstantBuffers(FrameResource* pFrameResource);
	};

}
//end basecross

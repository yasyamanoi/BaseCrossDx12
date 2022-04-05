/*!
@file GameObject.h
@brief ゲームオブジェクトクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	class BaseScene;
	class Stage;
	class Component;
	class Transform;

	//--------------------------------------------------------------------------------------
	// 配置されるオブジェクト
	//--------------------------------------------------------------------------------------
	class GameObject : public ObjectInterface {
		//ステージ親のポインタ
		weak_ptr<Stage> m_stage;
		//updateするかどうか
		bool m_updateActive;
		//Renderするかどうか
		bool m_renderActive;
		//透明かどうか
		bool m_alphaActive;
		//コンポーネントのマップ
		map<type_index, shared_ptr<Component> > m_compMap;
		//コンポーネント実行順番
		list<type_index> m_compOrder;
		//指定のコンポーネントを探す（派生クラスも検索）
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
		explicit GameObject(const shared_ptr<Stage>& stage);
		virtual ~GameObject();
	public:
		bool IsUpdateActive() const {
			return m_updateActive;
		}
		void SetUpdateActive(bool b) {
			m_updateActive = b;
		}
		bool IsRenderActive() const {
			return m_renderActive;
		}
		void SetRenderActive(bool b) {
			m_renderActive = b;
		}
		bool IsAlphaActive() const {
			return m_alphaActive;
		}
		void SetAlphaActive(bool b) {
			m_alphaActive = b;
		}
		shared_ptr<Stage> GetStage(bool exceptionActive = true) const;
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

		//--------------------------------------------------------------------------------------
		/*!
		@brief	このオブジェクトを2D描画するための射影行列を得る（デフォルトはステージのビューから取得）
		@return	ProjMatrix	射影行列
		*/
		//--------------------------------------------------------------------------------------
		virtual Mat4x4 Get2DDrawProjMatrix() const;

		//--------------------------------------------------------------------------------------
		/*!
		@brief	このオブジェクトを描画するためのカメラを得る（デフォルトはステージから取得）
		@return	カメラのshared_ptr
		*/
		//--------------------------------------------------------------------------------------
		virtual shared_ptr<BaseCamera> GetActiveCamera() const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	このオブジェクトを照らすライトセットを得る（デフォルトはステージから取得）
		@return	ライトセットのshared_ptr
		*/
		//--------------------------------------------------------------------------------------
		virtual shared_ptr<LightSet> GetActiveLightSet() const;


		virtual void OnPreInit() override;
		virtual void OnUpdate()override {}
		virtual void OnRender()override;
		virtual void OnInitFrame(BaseFrame* pBaseFrame);
		virtual void WriteConstantBuffers(BaseFrame* pBaseFrame);
		virtual void OnDestroy()override {}
	};


	//--------------------------------------------------------------------------------------
	///	ゲームオブジェクトのweak_ptrをグループ化したもの
	//--------------------------------------------------------------------------------------
	class GameObjectGroup : public ObjectInterface {
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
		const vector< weak_ptr<GameObject> >& GetGroupVector() const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	グループ内のゲームオブジェクトを得る
		@param[in]	index	グループ内オブジェクトのインデックス
		@return	グループ内のゲームオブジェクト
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<GameObject> at(size_t index);
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
		void IntoGroup(const shared_ptr<GameObject>& Obj);
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
		virtual void OnInit()override {}

	private:
		// pImplイディオム
		struct Impl;
		unique_ptr<Impl> pImpl;
	};



}
// end namespace basecross

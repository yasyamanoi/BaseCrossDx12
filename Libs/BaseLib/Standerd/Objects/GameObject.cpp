/*!
@file GameObject.cpp
@brief ゲームオブジェクトクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"

namespace basecross {

	GameObject::GameObject(const shared_ptr<Stage>& stage) :
		m_stage(stage),
		m_updateActive(true),
		m_renderActive(true),
		m_alphaActive(false)
	{}

	GameObject::~GameObject() {}



	shared_ptr<Stage> GameObject::GetStage(bool exceptionActive) const {
		auto shptr = m_stage.lock();
		if (shptr) {
			return shptr;
		}
		else {
			if (exceptionActive) {
				throw BaseException(
					L"所属ステージがnullです。自分自身がステージではありませんか？",
					L"if (!shptr)",
					L"GameObject::GetStage()const"
				);
			}
			else {
				return nullptr;
			}
		}
		// 所属ステージがnullだった
		// 自分自身がステージの可能性がある
		return nullptr;
	}

	void GameObject::OnShadowmapDraw() {
		ComponentShadowmapRender();
	}


	void GameObject::OnDraw() {
		ComponentRender();
	}

	void GameObject::TransformInit() {
		auto Transptr = GetComponent<Transform>();
		Transptr->SetToBefore();
	}

	void GameObject::OnPreCreate() {
		//Transform必須
		AddComponent<Transform>();
	}


	void GameObject::ComponentUpdate() {
		auto transPtr = GetComponent<Transform>();
		//		auto RightPtr = GetComponent<Rigidbody>(false);
				//マップを検証してUpdate
		list<type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			map<type_index, shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			if (it2 != m_compMap.end()) {
				//指定の型のコンポーネントが見つかった
				if (it2->second->IsUpdateActive()
					&& (it2->second != transPtr)
					//	&& (it2->second != RightPtr)
					) {
					it2->second->OnUpdate();
				}
			}
			it++;
		}
		//		if (RightPtr && RightPtr->IsUpdateActive()) {
		//			RightPtr->OnUpdate();
		//		}
		//TransformのUpdate
		if (transPtr->IsUpdateActive()) {
			transPtr->OnUpdate();
		}
	}

	void GameObject::ComponentShadowmapRender() {
		//Transformがなければ例外
		auto transptr = GetComponent<Transform>();
		auto shadowmapPtr = GetComponent<Shadowmap>(false);
		if (shadowmapPtr) {
			if (shadowmapPtr->IsDrawActive()) {
				shadowmapPtr->OnDraw();
			}
		}
	}


	void GameObject::ComponentRender() {

		//Transformがなければ例外
		auto Transptr = GetComponent<Transform>();
//		auto RightPtr = GetComponent<Rigidbody>(false);
		auto CollisionPtr = GetComponent<Collision>(false);
//		auto StringSpritePtr = GetComponent<StringSprite>(false);

		//マップを検証してDraw
		list<type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			map<type_index, shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			//指定の型のコンポーネントが見つかった
			if (it2 != m_compMap.end() && !dynamic_pointer_cast<Shadowmap>(it2->second)) {
				//シャドウマップ以外なら実行
				//そのコンポーネントの子コンポーネントの描画
				if (it2->second->IsDrawActive()
					&& (it2->second != Transptr)
//					&& (it2->second != RightPtr)
					&& (it2->second != CollisionPtr)
//					&& (it2->second != StringSpritePtr)
				) {
					it2->second->OnDraw();
				}
			}
			it++;
		}
		//if (RightPtr && RightPtr->IsDrawActive()) {
		//	//RigidbodyがあればDraw()
		//	RightPtr->OnDraw();
		//}
		if (CollisionPtr && CollisionPtr->IsDrawActive()) {
			//CollisionがあればDraw()
			CollisionPtr->OnDraw();
		}
		if (Transptr->IsDrawActive()) {
			Transptr->OnDraw();
		}
		//if (StringSpritePtr && StringSpritePtr->IsDrawActive()) {
		//	//StringSpriteのDraw()
		//	StringSpritePtr->OnDraw();
		//}

/*
		//Transformがなければ例外
		auto transptr = GetComponent<Transform>();
		//マップを検証してRender
		list<type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			map<type_index, shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			//指定の型のコンポーネントが見つかった
			if (it2 != m_compMap.end()) {
				if (it2->second->IsDrawActive()) {
					it2->second->OnDraw();
				}
			}
			it++;
		}
*/
	}

	void GameObject::WriteConstantBuffers(BaseFrame* pBaseFrame) {
		//Transformがなければ例外
		auto transptr = GetComponent<Transform>();
		//マップを検証してRender
		list<type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			map<type_index, shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			//指定の型のコンポーネントが見つかった
			if (it2 != m_compMap.end()) {
				it2->second->WriteConstantBuffers(pBaseFrame);
			}
			it++;
		}
	}

	void GameObject::OnInitFrame(BaseFrame* pBaseFrame) {
		//Transformがなければ例外
		auto transptr = GetComponent<Transform>();
		//マップを検証してRender
		list<type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			map<type_index, shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			//指定の型のコンポーネントが見つかった
			if (it2 != m_compMap.end()) {
				it2->second->OnInitFrame(pBaseFrame);
			}
			it++;
		}
	}



	void GameObject::ComponentDestroy() {
		auto transptr = GetComponent<Transform>();
		//マップを検証してUpdate
		list<type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			map<type_index, shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			if (it2 != m_compMap.end()) {
				//指定の型のコンポーネントが見つかった
				if (it2->second != transptr)
				{
					it2->second->OnDestroy();
				}
			}
			it++;
		}
		//TransformのOnDestroy
		transptr->OnDestroy();
	}


	//shared_ptr<BaseCamera> GameObject::GetActiveCamera()const {
	//	return GetStage()->GetActiveCamera();
	//}

	shared_ptr<Camera> GameObject::GetActiveCamera()const {
		return GetStage()->GetActiveCamera();
	}


	shared_ptr<LightSet> GameObject::GetActiveLightSet() const {
		return GetStage()->GetActiveLightSet();
	}

	//--------------------------------------------------------------------------------------
	//	class GameObjectGroup;
	//--------------------------------------------------------------------------------------
	GameObjectGroup::GameObjectGroup() :
		ObjectInterface()
	{}
	GameObjectGroup::~GameObjectGroup() {}
	//アクセサ
	const vector< weak_ptr<GameObject> >& GameObjectGroup::GetGroupVector() const {
		return m_Group;
	}
	shared_ptr<GameObject> GameObjectGroup::at(size_t index) {
		if (index >= m_Group.size()) {
			wstring msg = Util::SizeTToWStr(index);
			msg += L" >= ";
			msg += Util::SizeTToWStr(m_Group.size());
			throw BaseException(
				L"インデックスが範囲外です",
				msg,
				L"GameObjectGroup::at()"
			);
		}
		if (m_Group.at(index).expired()) {
			wstring msg = Util::SizeTToWStr(index);
			throw BaseException(
				L"そのインデックスのオブジェクトは無効です。",
				msg,
				L"GameObjectGroup::at()"
			);
		}
		return m_Group.at(index).lock();
	}
	size_t GameObjectGroup::size() const {
		return m_Group.size();
	}
	//操作
	void GameObjectGroup::IntoGroup(const shared_ptr<GameObject>& Obj) {
		m_Group.push_back(Obj);
	}

	void GameObjectGroup::AllClear() {
		m_Group.clear();
	}



}
//end basecross

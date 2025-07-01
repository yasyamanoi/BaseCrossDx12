/*!
@file GameObject.cpp
@brief ゲームオブジェクトクラス　実体
*/

#include "stdafx.h"

namespace basecross {

	std::shared_ptr<Stage> GameObject::GetStage(bool exceptionActive) const {
		auto shptr = m_stage.lock();
		if (shptr) {
			return shptr;
		}
		else {
			if (exceptionActive) {
				throw BaseException(
					L"所属ステージがnullです。",
					L"if (!shptr)",
					L"GameObject::GetStage()const"
				);
			}
			else {
				return nullptr;
			}
		}
		return nullptr;
	}

	std::shared_ptr<Camera> GameObject::GetCamera()const {
		return GetStage()->GetCamera();
	}
	std::shared_ptr<LightSet> GameObject::GetLightSet() const {
		return GetStage()->GetLightSet();
	}


	void GameObject::OnPreCreate() {
		//Transform必須
		AddComponent<Transform>(m_tempParam);
	}

	void GameObject::SetToBefore() {
		auto transPtr = GetComponent<Transform>();
		transPtr->SetToBefore();
	}

	void GameObject::ComponentUpdate() {
		auto transPtr = GetComponent<Transform>();

		//マップを検証してUpdate
		std::list<std::type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			std::map<std::type_index, std::shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			if (it2 != m_compMap.end()) {
				//指定の型のコンポーネントが見つかった
				if (it2->second->IsUpdateActive()
					&& (it2->second != transPtr)
					) {
					it2->second->OnUpdate(Scene::GetElapsedTime());
				}
			}
			it++;
		}
		//TransformのUpdate
		if (transPtr->IsUpdateActive()) {
			transPtr->OnUpdate(Scene::GetElapsedTime());
		}
	}

	void GameObject::TransformInit() {
		auto Transptr = GetComponent<Transform>();
		Transptr->SetToBefore();
	}

	void GameObject::ComponentDestroy() {
		auto transptr = GetComponent<Transform>();
		//マップを検証してUpdate
		std::list<std::type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			std::map<std::type_index, std::shared_ptr<Component> >::const_iterator it2;
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

	void GameObject::OnUpdateConstantBuffers() {
		//マップを検証してOnUpdateConstantBuffers
		std::list<std::type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			std::map<std::type_index, std::shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			if (it2 != m_compMap.end()) {
				//指定の型のコンポーネントが見つかった
				it2->second->OnUpdateConstantBuffers();
			}
			it++;
		}
	}

	void GameObject::OnCommitConstantBuffers() {
		//マップを検証してOnCommitConstantBuffers
		std::list<std::type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			std::map<std::type_index, std::shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			if (it2 != m_compMap.end()) {
				//指定の型のコンポーネントが見つかった
				it2->second->OnCommitConstantBuffers();
			}
			it++;
		}
	}

	void GameObject::OnShadowDraw() {
		//マップを検証してOnUpdateConstantBuffers
		std::list<std::type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			std::map<std::type_index, std::shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			if (it2 != m_compMap.end()) {
				//指定の型のコンポーネントが見つかった
				it2->second->OnShadowDraw();
			}
			it++;
		}
	}

	void GameObject::OnSceneDraw() {
		//マップを検証してOnUpdateConstantBuffers
		std::list<std::type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			std::map<std::type_index, std::shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			if (it2 != m_compMap.end()) {
				//指定の型のコンポーネントが見つかった
				it2->second->OnSceneDraw();
			}
			it++;
		}
	}


	//--------------------------------------------------------------------------------------
	//	class GameObjectGroup;
	//--------------------------------------------------------------------------------------
	GameObjectGroup::GameObjectGroup() :
		ObjectInterface()
	{
	}
	GameObjectGroup::~GameObjectGroup() {}
	//アクセサ
	const std::vector< std::weak_ptr<GameObject> >& GameObjectGroup::GetGroupVector() const {
		return m_Group;
	}
	std::shared_ptr<GameObject> GameObjectGroup::at(size_t index) {
		if (index >= m_Group.size()) {
			std::wstring msg = Util::SizeTToWStr(index);
			msg += L" >= ";
			msg += Util::SizeTToWStr(m_Group.size());
			throw BaseException(
				L"インデックスが範囲外です",
				msg,
				L"GameObjectGroup::at()"
			);
		}
		if (m_Group.at(index).expired()) {
			std::wstring msg = Util::SizeTToWStr(index);
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
	void GameObjectGroup::IntoGroup(const std::shared_ptr<GameObject>& Obj) {
		m_Group.push_back(Obj);
	}

	void GameObjectGroup::AllClear() {
		m_Group.clear();
	}



}
// end namespace basecross

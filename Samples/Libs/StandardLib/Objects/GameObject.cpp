/*!
@file GameObject.cpp
@brief �Q�[���I�u�W�F�N�g�N���X�@����
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
					L"�����X�e�[�W��null�ł��B",
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
		//Transform�K�{
		AddComponent<Transform>(m_tempParam);
	}

	void GameObject::SetToBefore() {
		auto transPtr = GetComponent<Transform>();
		transPtr->SetToBefore();
	}

	void GameObject::ComponentUpdate() {
		auto transPtr = GetComponent<Transform>();

		//�}�b�v�����؂���Update
		std::list<std::type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			std::map<std::type_index, std::shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			if (it2 != m_compMap.end()) {
				//�w��̌^�̃R���|�[�l���g����������
				if (it2->second->IsUpdateActive()
					&& (it2->second != transPtr)
					) {
					it2->second->OnUpdate(Scene::GetElapsedTime());
				}
			}
			it++;
		}
		//Transform��Update
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
		//�}�b�v�����؂���Update
		std::list<std::type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			std::map<std::type_index, std::shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			if (it2 != m_compMap.end()) {
				//�w��̌^�̃R���|�[�l���g����������
				if (it2->second != transptr)
				{
					it2->second->OnDestroy();
				}
			}
			it++;
		}
		//Transform��OnDestroy
		transptr->OnDestroy();
	}

	void GameObject::OnUpdateConstantBuffers() {
		//�}�b�v�����؂���OnUpdateConstantBuffers
		std::list<std::type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			std::map<std::type_index, std::shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			if (it2 != m_compMap.end()) {
				//�w��̌^�̃R���|�[�l���g����������
				it2->second->OnUpdateConstantBuffers();
			}
			it++;
		}
	}

	void GameObject::OnCommitConstantBuffers() {
		//�}�b�v�����؂���OnCommitConstantBuffers
		std::list<std::type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			std::map<std::type_index, std::shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			if (it2 != m_compMap.end()) {
				//�w��̌^�̃R���|�[�l���g����������
				it2->second->OnCommitConstantBuffers();
			}
			it++;
		}
	}

	void GameObject::OnShadowDraw() {
		//�}�b�v�����؂���OnUpdateConstantBuffers
		std::list<std::type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			std::map<std::type_index, std::shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			if (it2 != m_compMap.end()) {
				//�w��̌^�̃R���|�[�l���g����������
				it2->second->OnShadowDraw();
			}
			it++;
		}
	}

	void GameObject::OnSceneDraw() {
		//�}�b�v�����؂���OnUpdateConstantBuffers
		std::list<std::type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			std::map<std::type_index, std::shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			if (it2 != m_compMap.end()) {
				//�w��̌^�̃R���|�[�l���g����������
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
	//�A�N�Z�T
	const std::vector< std::weak_ptr<GameObject> >& GameObjectGroup::GetGroupVector() const {
		return m_Group;
	}
	std::shared_ptr<GameObject> GameObjectGroup::at(size_t index) {
		if (index >= m_Group.size()) {
			std::wstring msg = Util::SizeTToWStr(index);
			msg += L" >= ";
			msg += Util::SizeTToWStr(m_Group.size());
			throw BaseException(
				L"�C���f�b�N�X���͈͊O�ł�",
				msg,
				L"GameObjectGroup::at()"
			);
		}
		if (m_Group.at(index).expired()) {
			std::wstring msg = Util::SizeTToWStr(index);
			throw BaseException(
				L"���̃C���f�b�N�X�̃I�u�W�F�N�g�͖����ł��B",
				msg,
				L"GameObjectGroup::at()"
			);
		}
		return m_Group.at(index).lock();
	}
	size_t GameObjectGroup::size() const {
		return m_Group.size();
	}
	//����
	void GameObjectGroup::IntoGroup(const std::shared_ptr<GameObject>& Obj) {
		m_Group.push_back(Obj);
	}

	void GameObjectGroup::AllClear() {
		m_Group.clear();
	}



}
// end namespace basecross

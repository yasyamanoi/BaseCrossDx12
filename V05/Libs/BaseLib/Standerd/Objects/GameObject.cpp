/*!
@file GameObject.cpp
@brief �Q�[���I�u�W�F�N�g�N���X
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"

namespace basecross {

	GameObject::GameObject(const shared_ptr<Stage>& stage) :
		m_stage(stage),
		m_updateActive(true),
		m_drawActive(true),
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
					L"�����X�e�[�W��null�ł��B�������g���X�e�[�W�ł͂���܂��񂩁H",
					L"if (!shptr)",
					L"GameObject::GetStage()const"
				);
			}
			else {
				return nullptr;
			}
		}
		// �����X�e�[�W��null������
		// �������g���X�e�[�W�̉\��������
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
		//Transform�K�{
		AddComponent<Transform>();
	}


	void GameObject::ComponentUpdate(bool physicsFlg) {
		auto transPtr = GetComponent<Transform>();
		auto rigidPtr = GetComponent<Rigidbody>(false);
				//�}�b�v�����؂���Update
		list<type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			map<type_index, shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			if (it2 != m_compMap.end()) {
				//�w��̌^�̃R���|�[�l���g����������
				if (it2->second->IsUpdateActive()
					&& (it2->second != transPtr)
						&& (it2->second != rigidPtr)
					) {
					it2->second->OnUpdate();
				}
			}
			it++;
		}
		if (rigidPtr && physicsFlg && rigidPtr->IsUpdateActive()) {
			rigidPtr->OnUpdate();
		}
		//Transform��Update
		if (transPtr->IsUpdateActive()) {
			transPtr->OnUpdate();
		}
	}

	void GameObject::ComponentShadowmapRender() {
		//Transform���Ȃ���Η�O
		auto transptr = GetComponent<Transform>();
		auto shadowmapPtr = GetComponent<Shadowmap>(false);
		if (shadowmapPtr) {
			if (shadowmapPtr->IsDrawActive()) {
				shadowmapPtr->OnDraw();
			}
		}
	}


	void GameObject::ComponentRender() {

		//Transform���Ȃ���Η�O
		auto Transptr = GetComponent<Transform>();
//		auto RightPtr = GetComponent<Rigidbody>(false);
		auto CollisionPtr = GetComponent<Collision>(false);
//		auto StringSpritePtr = GetComponent<StringSprite>(false);

		//�}�b�v�����؂���Draw
		list<type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			map<type_index, shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			//�w��̌^�̃R���|�[�l���g����������
			if (it2 != m_compMap.end() && !dynamic_pointer_cast<Shadowmap>(it2->second)) {
				//�V���h�E�}�b�v�ȊO�Ȃ���s
				//���̃R���|�[�l���g�̎q�R���|�[�l���g�̕`��
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
		//	//Rigidbody�������Draw()
		//	RightPtr->OnDraw();
		//}
		if (CollisionPtr && CollisionPtr->IsDrawActive()) {
			//Collision�������Draw()
			CollisionPtr->OnDraw();
		}
		if (Transptr->IsDrawActive()) {
			Transptr->OnDraw();
		}
		//if (StringSpritePtr && StringSpritePtr->IsDrawActive()) {
		//	//StringSprite��Draw()
		//	StringSpritePtr->OnDraw();
		//}

/*
		//Transform���Ȃ���Η�O
		auto transptr = GetComponent<Transform>();
		//�}�b�v�����؂���Render
		list<type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			map<type_index, shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			//�w��̌^�̃R���|�[�l���g����������
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
		//Transform���Ȃ���Η�O
		auto transptr = GetComponent<Transform>();
		//�}�b�v�����؂���Render
		list<type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			map<type_index, shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			//�w��̌^�̃R���|�[�l���g����������
			if (it2 != m_compMap.end()) {
				it2->second->WriteConstantBuffers(pBaseFrame);
			}
			it++;
		}
	}



	void GameObject::OnInitFrame(BaseFrame* pBaseFrame) {
		//Transform���Ȃ���Η�O
		auto transptr = GetComponent<Transform>();
		//�}�b�v�����؂���Render
		list<type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			map<type_index, shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			//�w��̌^�̃R���|�[�l���g����������
			if (it2 != m_compMap.end()) {
				it2->second->OnInitFrame(pBaseFrame);
			}
			it++;
		}
	}

	void GameObject::ComponentDestroy() {
		auto transptr = GetComponent<Transform>();
		//�}�b�v�����؂���Update
		list<type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			map<type_index, shared_ptr<Component> >::const_iterator it2;
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
	//�A�N�Z�T
	const vector< weak_ptr<GameObject> >& GameObjectGroup::GetGroupVector() const {
		return m_Group;
	}
	shared_ptr<GameObject> GameObjectGroup::at(size_t index) {
		if (index >= m_Group.size()) {
			wstring msg = Util::SizeTToWStr(index);
			msg += L" >= ";
			msg += Util::SizeTToWStr(m_Group.size());
			throw BaseException(
				L"�C���f�b�N�X���͈͊O�ł�",
				msg,
				L"GameObjectGroup::at()"
			);
		}
		if (m_Group.at(index).expired()) {
			wstring msg = Util::SizeTToWStr(index);
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
	void GameObjectGroup::IntoGroup(const shared_ptr<GameObject>& Obj) {
		m_Group.push_back(Obj);
	}

	void GameObjectGroup::AllClear() {
		m_Group.clear();
	}



}
//end basecross
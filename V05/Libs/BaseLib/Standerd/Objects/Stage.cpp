/*!
@file Stage.cpp
@brief �e�X�e�[�W�N���X
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#include "stdafx.h"

namespace basecross {
	//--------------------------------------------------------------------------------------
	///	�e�X�e�[�W
	//--------------------------------------------------------------------------------------
	Stage::Stage():
		m_updateActive(true),
		m_physicsActive(false)
	{}


	void Stage::PushBackGameObject(const shared_ptr<GameObject>& Ptr) {
		//���̃X�e�[�W�̓N���G�C�g��ł���
		if (IsCreated()) {
			m_waitAddObjectVec.push_back(Ptr);
		}
		else {
			//�N���G�C�g�O
			m_gameObjectVec.push_back(Ptr);
		}
		Ptr->TransformInit();
	}

	//�폜�I�u�W�F�N�g�̐ݒ�
	void Stage::RemoveBackGameObject(const shared_ptr<GameObject>& Ptr) {
		m_waitRemoveObjectVec.push_back(Ptr);
	}

	void Stage::RemoveTargetGameObject(const shared_ptr<GameObject>& targetobj) {
		auto it = m_gameObjectVec.begin();
		while (it != m_gameObjectVec.end()) {
			if (*it == targetobj) {
				//�폜����邱�Ƃ��I�u�W�F�N�g�ɓ`����
				targetobj->ComponentDestroy();
				targetobj->OnDestroy();
				m_gameObjectVec.erase(it);
				return;
			}
			it++;
		}
	}


	void Stage::SetWaitToObjectVec() {
		if (!m_waitRemoveObjectVec.empty()) {
			for (auto Ptr : m_waitRemoveObjectVec) {
				RemoveTargetGameObject(Ptr);
			}
		}
		m_waitRemoveObjectVec.clear();
		if (!m_waitAddObjectVec.empty()) {
			auto baseDevice = App::GetBaseDevice();
			auto frames = baseDevice->GetBaseFrames();
			for (auto Ptr : m_waitAddObjectVec) {
				m_gameObjectVec.push_back(Ptr);
				for (UINT i = 0; i < baseDevice->m_frameCount; i++) {
					Ptr->OnInitFrame(frames[i]);
					Ptr->WriteConstantBuffers(frames[i]);
				}
			}
		}
		m_waitAddObjectVec.clear();
	}

	shared_ptr<GameObject> Stage::GetSharedGameObject(const wstring& key, bool exceptionActive)const {
		map<const wstring, weak_ptr<GameObject> >::const_iterator it;
		//�d���L�[�̌���
		it = m_SharedMap.find(key);
		if (it != m_SharedMap.end()) {
			auto shptr = it->second.lock();
			if (shptr) {
				return shptr;
			}
			else {
				//���łɖ���
				if (exceptionActive) {
					//��O����
					wstring keyerr = key;
					throw BaseException(
						L"�I�u�W�F�N�g�������ł�",
						keyerr,
						L"Stage::GetSharedGameObject()"
					);
				}
			}
		}
		else {
			//�w��̖��O��������Ȃ�����
			if (exceptionActive) {
				//��O����
				wstring keyerr = key;
				throw BaseException(
					L"�I�u�W�F�N�g��������܂���",
					keyerr,
					L"Stage::GetSharedGameObject()"
				);
			}
		}
		return nullptr;
	}

	void Stage::SetSharedGameObject(const wstring& key, const shared_ptr<GameObject>& ptr) {
		map<const wstring, weak_ptr<GameObject> >::iterator it;
		//�d���L�[�̌���
		it = m_SharedMap.find(key);
		if (it != m_SharedMap.end()) {
			//���ɑ��݂���
			//��O����
			wstring keyerr = key;
			throw BaseException(
				L"�����̃V�F�A�I�u�W�F�N�g������܂�",
				keyerr,
				L"Stage::SetSharedGameObject()"
			);
		}
		else {
			m_SharedMap[key] = ptr;
		}
	}


	shared_ptr<GameObjectGroup> Stage::CreateSharedObjectGroup(const wstring& Key) {
		try {
			map<const wstring, shared_ptr<GameObjectGroup> >::iterator it;
			//�d���L�[�̌���
			it = m_SharedGroupMap.find(Key);
			if (it != m_SharedGroupMap.end()) {
				//���ɑ��݂���
				//��O����
				wstring keyerr = Key;
				throw BaseException(
					L"�����̃V�F�A�I�u�W�F�N�g�z�񂪂���܂�",
					keyerr,
					L"Stage::CreateSharedObjectGroup()"
				);
			}
			else {
				auto Ptr = ObjectFactory::Create<GameObjectGroup>();
				m_SharedGroupMap[Key] = Ptr;
				return Ptr;
			}
		}
		catch (...) {
			throw;
		}
	}

	shared_ptr<GameObjectGroup> Stage::GetSharedObjectGroup(const wstring& Key, bool ExceptionActive)const {
		//�d���L�[�̌���
		auto it = m_SharedGroupMap.find(Key);
		if (it != m_SharedGroupMap.end()) {
			//�y�A��second��Ԃ�
			return it->second;
		}
		else {
			//�w��̖��O��������Ȃ�����
			if (ExceptionActive) {
				//��O����
				wstring keyerr = Key;
				throw BaseException(
					L"�w��̃L�[��������܂���",
					keyerr,
					L"Stage::GetSharedObjectGroup() const"
				);
			}
		}
		return nullptr;
	}

	void Stage::SetSharedObjectGroup(const wstring& Key, const shared_ptr<GameObjectGroup>& NewPtr) {
		//�d���L�[�̌���
		auto it = m_SharedGroupMap.find(Key);
		if (it != m_SharedGroupMap.end()) {
			//��O����
			wstring keyerr = Key;
			throw BaseException(
				L"�����̃V�F�A�I�u�W�F�N�g�z�񂪂���܂�",
				keyerr,
				L"Stage::SetSharedObjectGroup()"
			);
		}
		else {
			//�w��̖��O��������Ȃ�����
			//�o�^�ł���
			m_SharedGroupMap[Key] = NewPtr;
		}
	}

	uint16_t Stage::GetVacantPhysicsIndex() {
		uint16_t ret = UINT16_MAX;
		if (m_vacantPhysicsIndices.size() > 0) {
			//�󂢂Ă���ID������B�擪���擾
			auto it = m_vacantPhysicsIndices.begin();
			ret = *it;
			//ID���g���̂ł���ID���폜
			m_vacantPhysicsIndices.erase(ret);
		}
		return ret;
	}

	void Stage::SetVacantPhysicsIndex(uint16_t index) {
		m_vacantPhysicsIndices.insert(index);
	}

	void Stage::RemoveGameObject(const shared_ptr<GameObject>& Obj) {
		for (auto ptr : GetGameObjectVec()) {
			if (Obj == ptr) {
				RemoveBackGameObject(Obj);
				return;
			}
		}
		//���݂��Ȃ��Ă��������Ȃ�
	}

	void Stage::OnInitFrame(BaseFrame* pBaseFrame) {
		for (auto& v : m_gameObjectVec) {
			v->OnInitFrame(pBaseFrame);
		}
	}


	void Stage::WriteConstantBuffers(BaseFrame* pBaseFrame) {
		for (auto& v : m_gameObjectVec) {
			v->WriteConstantBuffers(pBaseFrame);
		}
	}


	//�X�e�[�W���̍X�V�i�V�[�������΂��j
	void Stage::UpdateStage() {
		bool physicsFlg = false;
		static float timeStep = 0.0f;
		timeStep += App::GetElapsedTime();
		
		if (timeStep >= m_basePhysics.GetTimeStep() * 0.5f) {
			physicsFlg = true;
			timeStep = 0.0f;
		}

		if (IsUpdatePerformanceActive()) {
			m_updatePerformance.Start();
		}
		//�ǉ��E�폜�܂��I�u�W�F�N�g�̒ǉ��ƍ폜
		SetWaitToObjectVec();
		//Transform�R���|�[�l���g�̒l���o�b�N�A�b�v�ɃR�s�[
		for (auto& ptr : GetGameObjectVec()) {
			if (ptr->IsUpdateActive()) {
				auto ptr2 = ptr->GetComponent<Transform>();
				ptr2->SetToBefore();
			}
		}
		//�����I�u�W�F�N�g�̃t�H�[�X�̏�����
		if (IsPhysicsActive()) {
			if (physicsFlg) {
				m_basePhysics.InitForce();
			}
		}

		//�z�u�I�u�W�F�N�g�̍X�V����
		for (auto& ptr : GetGameObjectVec()) {
			if (ptr->IsUpdateActive()) {
				ptr->OnUpdate();
			}
		}

		//���g�̍X�V����
		if (IsUpdateActive()) {
			OnUpdate();
		}
		//�����I�u�W�F�N�g�̍X�V
		if (IsPhysicsActive()) {
			if (physicsFlg) {
				m_basePhysics.Update(false);
			}
		}
		//�z�u�I�u�W�F�N�g�̃R���|�[�l���g�X�V
		for (auto& ptr : GetGameObjectVec()) {
			if (ptr->IsUpdateActive()) {
				ptr->ComponentUpdate(physicsFlg);
			}
		}

		////�Փ˔���
		UpdateCollision();
		//�z�u�I�u�W�F�N�g�̍X�V�㏈��
		for (auto& ptr : GetGameObjectVec()) {
			if (ptr->IsUpdateActive()) {
				ptr->OnUpdate2();
			}
		}
		//���g�̍X�V�㏈��
		if (IsUpdateActive()) {
			OnUpdate2();
		}
		//�J�����ƃ��C�g
		OnUpdateLightCamera();
		if (IsUpdatePerformanceActive()) {
			m_updatePerformance.End();
		}
	}

	//�Փ˔���̍X�V�i�X�e�[�W����Ăԁj
	//�Փ˔�����J�X�^�}�C�Y���邽�߂ɂ�
	//���̊֐��𑽏d��`����
	void Stage::UpdateCollision() {
		if (m_collisionPerformance.IsAvtive()) {
			m_collisionPerformance.Start();
			//�Փ˔���Ǘ��҂�Update(����)
			m_collisionManager->OnUpdate();
			m_collisionPerformance.End();
		}
		else {
			//�Փ˔���Ǘ��҂�Update(����)
			m_collisionManager->OnUpdate();
		}
	}



	void Stage::OnPreCreate() {
		//�R���W�����Ǘ��҂̍쐬
		m_collisionManager = ObjectFactory::Create<CollisionManager>(GetThis<Stage>());
		//�����v�Z���Z�b�g
		m_basePhysics.Reset();
	}


	void Stage::OnDraw() {
		for (auto& v : m_gameObjectVec) {
			if (v->IsDrawActive()) {
				v->OnDraw();
			}
		}
	}

	void Stage::OnShadowmapDraw() {
		for (auto& v : m_gameObjectVec) {
			if (v->IsDrawActive()) {
				v->OnShadowmapDraw();
			}
		}
	}



	void Stage::OnDestroy() {
		for (auto& v : m_gameObjectVec) {
			v->OnDestroy();
		}
	}
}
//end namespace basecross
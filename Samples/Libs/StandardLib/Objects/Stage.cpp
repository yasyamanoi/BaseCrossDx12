/*!
@file Stage.cpp
@brief �e�X�e�[�W�N���X�@����
*/

#include "stdafx.h"

namespace basecross {

	void Stage::OnUpdateConstantBuffers()
	{
		for (auto& v : m_gameObjectVec) {
			v->OnUpdateConstantBuffers();
		}
	}

	void Stage::OnCommitConstantBuffers()
	{
		for (auto& v : m_gameObjectVec) {
			v->OnCommitConstantBuffers();
		}
	}

	//�X�e�[�W���̍X�V�i�V�[�������΂��j
	void Stage::UpdateStage() {
		//�ǉ��E�폜�܂��I�u�W�F�N�g�̒ǉ��ƍ폜
		SetWaitToObjectVec();
		//Transform�R���|�[�l���g�̒l���o�b�N�A�b�v�ɃR�s�[
		for (auto& ptr : GetGameObjectVec()) {
			if (ptr->IsUpdateActive()) {
				auto ptr2 = ptr->GetComponent<Transform>();
				ptr2->SetToBefore();
			}
		}
		//�z�u�I�u�W�F�N�g�̍X�V����
		for (auto& ptr : GetGameObjectVec()) {
			if (ptr->IsUpdateActive()) {
				ptr->OnUpdate(Scene::GetElapsedTime());
			}
		}
		//���g�̍X�V����
		if (IsUpdateActive()) {
			OnUpdate(Scene::GetElapsedTime());
		}
		//�z�u�I�u�W�F�N�g�̃R���|�[�l���g�X�V
		for (auto& ptr : GetGameObjectVec()) {
			if (ptr->IsUpdateActive()) {
				ptr->ComponentUpdate();
			}
		}
		////�Փ˔���
		UpdateCollision();
		//�z�u�I�u�W�F�N�g�̍X�V�㏈��
		for (auto& ptr : GetGameObjectVec()) {
			if (ptr->IsUpdateActive()) {
				ptr->OnUpdate2(Scene::GetElapsedTime());
			}
		}
		//���g�̍X�V�㏈��
		if (IsUpdateActive()) {
			OnUpdate2(Scene::GetElapsedTime());
		}
		//�J�����ƃ��C�g
		m_camera->OnUpdate(Scene::GetElapsedTime());
		m_lightSet->OnUpdate(Scene::GetElapsedTime());
	}

	//�Փ˔���̍X�V�i�X�e�[�W����Ăԁj
	//�Փ˔�����J�X�^�}�C�Y���邽�߂ɂ�
	//���̊֐��𑽏d��`����
	void Stage::UpdateCollision() {
		//�Փ˔���Ǘ��҂�Update(����)
		m_collisionManager->OnUpdate(Scene::GetElapsedTime());
	}


	void Stage::OnPreCreate() {
		//�R���W�����Ǘ��҂̍쐬
		m_collisionManager = ObjectFactory::Create<CollisionManager>(GetThis<Stage>());
	}

	void Stage::OnShadowDraw() {
		for (auto& v : m_gameObjectVec) {
			v->OnShadowDraw();
		}
	}
	void Stage::OnSceneDraw() {
		std::vector<std::shared_ptr<GameObject>> alphaVec;
		std::vector<std::shared_ptr<GameObject>> normalVec;
		for (auto& v : m_gameObjectVec) {
			if (v->IsAlphaActive()) {
				alphaVec.push_back(v);
			}
			else {
				normalVec.push_back(v);
			}
		}
		//�J�����̈ʒu
		Vec3 CameraEye = GetCamera()->GetEye();
		//������3D�I�u�W�F�N�g���J��������̋����Ń\�[�g
		//�ȉ��́A�I�u�W�F�N�g�������Ɏ��bool��Ԃ������_��
		//--------------------------------------------------------
		auto func = [&](std::shared_ptr<GameObject>& Left, std::shared_ptr<GameObject>& Right)->bool {
			auto PtrLeftTrans = Left->GetComponent<Transform>();
			auto PtrRightTrans = Right->GetComponent<Transform>();

			auto LeftPos = PtrLeftTrans->GetWorldMatrix().transInMatrix();
			auto RightPos = PtrRightTrans->GetWorldMatrix().transInMatrix();

			auto LeftLen = bsmUtil::length(LeftPos - CameraEye);
			auto RightLen = bsmUtil::length(RightPos - CameraEye);

			return (LeftLen > RightLen);
			};
		std::sort(alphaVec.begin(), alphaVec.end(), func);
		for (auto& v : normalVec) {
			v->OnSceneDraw();
		}
		for (auto& v : alphaVec) {
			v->OnSceneDraw();
		}
	}

	void Stage::OnDestroy() {
		for (auto& v : m_gameObjectVec) {
			v->OnDestroy();
		}
	}

	void Stage::SetToBefore() {
		for (auto& v : m_gameObjectVec) {
			v->SetToBefore();
		}
	}


	void Stage::PushBackGameObject(const std::shared_ptr<GameObject>& Ptr) {
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
	void Stage::RemoveBackGameObject(const std::shared_ptr<GameObject>& Ptr) {
		m_waitRemoveObjectVec.push_back(Ptr);
	}

	void Stage::RemoveTargetGameObject(const std::shared_ptr<GameObject>& targetobj) {
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
			for (auto Ptr : m_waitAddObjectVec) {
				m_gameObjectVec.push_back(Ptr);
			}
		}
		m_waitAddObjectVec.clear();
	}
	std::shared_ptr<GameObject> Stage::GetSharedGameObject(const std::wstring & key, bool exceptionActive)const {
		std::map<const std::wstring, std::weak_ptr<GameObject> >::const_iterator it;
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
						std::wstring keyerr = key;
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
					std::wstring keyerr = key;
					throw BaseException(
						L"�I�u�W�F�N�g��������܂���",
						keyerr,
						L"Stage::GetSharedGameObject()"
					);
				}
			}
			return nullptr;
		}

	void Stage::SetSharedGameObject(const std::wstring& key, const std::shared_ptr<GameObject>& ptr) {
		std::map<const std::wstring, std::weak_ptr<GameObject> >::iterator it;
		//�d���L�[�̌���
		it = m_SharedMap.find(key);
		if (it != m_SharedMap.end()) {
			//���ɑ��݂���
			//��O����
			std::wstring keyerr = key;
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


	std::shared_ptr<GameObjectGroup> Stage::CreateSharedObjectGroup(const std::wstring& Key) {
		try {
			std::map<const std::wstring, std::shared_ptr<GameObjectGroup> >::iterator it;
			//�d���L�[�̌���
			it = m_SharedGroupMap.find(Key);
			if (it != m_SharedGroupMap.end()) {
				//���ɑ��݂���
				//��O����
				std::wstring keyerr = Key;
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

	std::shared_ptr<GameObjectGroup> Stage::GetSharedObjectGroup(const std::wstring& Key, bool ExceptionActive)const {
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
				std::wstring keyerr = Key;
				throw BaseException(
					L"�w��̃L�[��������܂���",
					keyerr,
					L"Stage::GetSharedObjectGroup() const"
				);
			}
		}
		return nullptr;
	}

	void Stage::SetSharedObjectGroup(const std::wstring& Key, const std::shared_ptr<GameObjectGroup>& NewPtr) {
		//�d���L�[�̌���
		auto it = m_SharedGroupMap.find(Key);
		if (it != m_SharedGroupMap.end()) {
			//��O����
			std::wstring keyerr = Key;
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


	void Stage::RemoveGameObject(const std::shared_ptr<GameObject>& Obj) {
		for (auto ptr : GetGameObjectVec()) {
			if (Obj == ptr) {
				RemoveBackGameObject(Obj);
				return;
			}
		}
		//���݂��Ȃ��Ă��������Ȃ�
	}


	



}
// end namespace basecross

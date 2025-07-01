/*!
@file Stage.h
@brief �e�X�e�[�W�N���X
*/


#pragma once

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	// �e�X�e�[�W�N���X
	//--------------------------------------------------------------------------------------
	class Stage : public ObjectInterface {
		//update���邩�ǂ���
		bool m_updateActive;
		void PushBackGameObject(const std::shared_ptr<GameObject>& ptr);
		void RemoveBackGameObject(const std::shared_ptr<GameObject>& ptr);
		//�ǉ��E�폜�܂��I�u�W�F�N�g�̒ǉ��ƍ폜
		void RemoveTargetGameObject(const std::shared_ptr<GameObject>& targetobj);
		void SetWaitToObjectVec();
		//�Q�[���I�u�W�F�N�g�̔z��
		std::vector<std::shared_ptr<GameObject>> m_gameObjectVec;
		//�ǉ��҂��̃Q�[���I�u�W�F�N�g
		std::vector<std::shared_ptr<GameObject>> m_waitAddObjectVec;
		//�폜�҂��̃Q�[���I�u�W�F�N�g
		std::vector<std::shared_ptr<GameObject>> m_waitRemoveObjectVec;
		//�V�F�A�I�u�W�F�N�g�|�C���^�̃}�b�v
		std::map<const std::wstring, std::weak_ptr<GameObject> > m_SharedMap;
		//�V�F�A�O���[�v�̃|�C���^�̃}�b�v
		std::map<const std::wstring, std::shared_ptr<GameObjectGroup> >  m_SharedGroupMap;

		//�R���W�����Ǘ���
		std::shared_ptr<CollisionManager> m_collisionManager;
	protected:
		ID3D12Device* m_pDevice;
		std::shared_ptr<Camera> m_camera;
		std::shared_ptr<LightSet> m_lightSet;
		Stage(ID3D12Device* pDevice) :
			m_pDevice(pDevice),
			m_updateActive(true)
		{}
		virtual ~Stage() {}
	public:
		template<typename T, typename... Ts>
		std::shared_ptr<T> AddGameObject(Ts&&... params) {
			try {
				auto ptr = ObjectFactory::Create<T>(GetThis<Stage>(), params...);
				m_gameObjectVec.push_back(ptr);
				return ptr;
			}
			catch (...) {
				throw;
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�p�����[�^��OnCreateWithParam�ɓn���Q�[���I�u�W�F�N�g��ǉ�����
		@tparam	T	�쐬����^
		@tparam	Ts	�ϒ��p�����[�^�̌^
		@param[in]	params	�ϒ��p�����[�^
		@return	�쐬���ꂽ�Q�[���I�u�W�F�N�g
		*/
		//--------------------------------------------------------------------------------------
		template<typename T, typename... Ts>
		std::shared_ptr<T> AddGameObjectWithParam(Ts&&... params) {
			try {
				auto Ptr = ObjectFactory::CreateGameObjectWithParam<T>(GetThis<Stage>(), params...);
				PushBackGameObject(Ptr);
				return Ptr;
			}
			catch (...) {
				throw;
			}
		}


		//�A�N�Z�T
		std::shared_ptr<Camera> GetCamera() const {
			return m_camera;
		}
		std::shared_ptr<LightSet> GetLightSet() const{
			return m_lightSet;
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	�X�V�������L�����ǂ����𓾂�
		@return	�X�V�������L���Ȃ�true
		*/
		//--------------------------------------------------------------------------------------
		bool IsUpdateActive() const {
			return m_updateActive;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�X�V�������L�����ǂ�����ݒ肷��
		@param[in]	b	�X�V�������L���Ȃ�Ȃ�true
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetUpdateActive(bool b) {
			m_updateActive = b;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Փ˔���}�l�[�W���𓾂�(const)
		@return	�Փ˔���}�l�[�W��
		*/
		//--------------------------------------------------------------------------------------
		std::shared_ptr<CollisionManager> GetCollisionManager() const {
			return m_collisionManager;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Q�[���I�u�W�F�N�g�̔z��𓾂�
		@return	�Q�[���I�u�W�F�N�g�̔z��
		*/
		//--------------------------------------------------------------------------------------
		std::vector<std::shared_ptr<GameObject>>& GetGameObjectVec() {
			return m_gameObjectVec;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Q�[���I�u�W�F�N�g�̔z��𓾂�(const)
		@return	�Q�[���I�u�W�F�N�g�̔z��
		*/
		//--------------------------------------------------------------------------------------
		const std::vector< std::shared_ptr<GameObject> >& GetGameObjectVec() const {
			return m_gameObjectVec;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Q�[���I�u�W�F�N�g���폜����
		@param[in]	Obj	�C���X�^���X
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void RemoveGameObject(const std::shared_ptr<GameObject>& Obj);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���L����Ă�Q�[���I�u�W�F�N�g���擾����
		@tparam	T	�擾����^
		@param[in]	key	�L�[
		@param[in]	exceptionActive	������Ȃ��Ƃ��ɗ�O�𔭍s���邩�ǂ���
		@return	���L����Ă�Q�[���I�u�W�F�N�g
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		std::shared_ptr<T> GetSharedGameObjectEx(const std::wstring& key, bool exceptionActive = true)const {
			shared_ptr<T> ptr = std::dynamic_pointer_cast<T>(GetSharedGameObject(key, exceptionActive));
			if (ptr) {
				return ptr;
			}
			else {
				//�L���X�g�ł��Ȃ�
				if (exceptionActive) {
					//��O����
					std::wstring keyerr = key;
					std::wstring str = L"�I�u�W�F�N�g��";
					str += Util::GetWSTypeName<T>();
					str += L"�^�ɃL���X�g�ł��܂���";
					throw BaseException(
						str,
						keyerr,
						L"Stage::GetSharedGameObjectEx<T>()"
					);
				}
			}
			return nullptr;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���L����Ă�Q�[���I�u�W�F�N�g���擾����iGameObject�^�j
		@param[in]	key	�L�[
		@param[in]	exceptionActive	������Ȃ��Ƃ��ɗ�O�𔭍s���邩�ǂ���
		@return	���L����Ă�Q�[���I�u�W�F�N�g
		*/
		//--------------------------------------------------------------------------------------

		std::shared_ptr<GameObject> GetSharedGameObject(const std::wstring& key, bool exceptionActive = true)const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Q�[���I�u�W�F�N�g�����L����
		@param[in]	key	�L�[
		@param[in]	ptr	�Q�[���I�u�W�F�N�g�̃|�C���^
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetSharedGameObject(const std::wstring& key, const std::shared_ptr<GameObject>& ptr);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���L����Q�[���I�u�W�F�N�g�O���[�v���쐬����
		@param[in]	Key	�L�[
		@return	���L����Q�[���I�u�W�F�N�g�O���[�v
		*/
		//--------------------------------------------------------------------------------------
		std::shared_ptr<GameObjectGroup> CreateSharedObjectGroup(const std::wstring& Key);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���L����Q�[���I�u�W�F�N�g�O���[�v�𓾂�
		@param[in]	Key	�L�[
		@param[in]	ExceptionActive	������Ȃ��Ƃ��ɗ�O�𔭍s���邩�ǂ���
		@return	���L����Q�[���I�u�W�F�N�g�O���[�v
		*/
		//--------------------------------------------------------------------------------------
		std::shared_ptr<GameObjectGroup> GetSharedObjectGroup(const std::wstring& Key, bool ExceptionActive = true)const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���L����Q�[���I�u�W�F�N�g�O���[�v�𓾂�i�O���[�v�h���N���X��������ꍇ�p�j
		@tparam	T	�擾����^
		@param[in]	Key	�L�[
		@param[in]	ExceptionActive	������Ȃ��Ƃ��ɗ�O�𔭍s���邩�ǂ���
		@return	���L����Q�[���I�u�W�F�N�g�O���[�v
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		std::shared_ptr<T> GetSharedObjectGroup(const std::wstring& Key, bool ExceptionActive = true)const {
			auto RetPtr = std::dynamic_pointer_cast<T>(GetSharedObjectGroup(Key, ExceptionActive));
			if (RetPtr) {
				return RetPtr;
			}
			else {
				if (ExceptionActive) {
					//��O����
					std::wstring keyerr = Key;
					throw BaseException(
						L"�w��̃L�[�̃O���[�v��T�^�ɕϊ��ł��܂���",
						keyerr,
						L"Stage::GetSharedObjectGroup<T>()"
					);
				}
			}
			return nullptr;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���L����Q�[���I�u�W�F�N�g�O���[�v���Z�b�g����i�O���[�v�h���N���X��������ꍇ�p�j
		@param[in]	Key	�L�[
		@param[in]	NewPtr	���L����Q�[���I�u�W�F�N�g�O���[�v
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetSharedObjectGroup(const std::wstring& Key, const std::shared_ptr<GameObjectGroup>& NewPtr);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�w��̃^�O�����Q�[���I�u�W�F�N�g�̔z����擾����
		@param[in]	Tag	��������^�O
		@param[out]	�擾����Q�[���I�u�W�F�N�g�̔z��
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void GetUsedTagObjectVec(const std::wstring& Tag, std::vector<std::shared_ptr<GameObject>>& ObjVec) const {
			for (auto& v : GetGameObjectVec()) {
				if (v->FindTag(Tag)) {
					ObjVec.push_back(v);
				}
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Q�[���I�u�W�F�N�g�Ŏw��̃R���|�[�l���g�̐e���q�̃R���|�[�l���g�����ꍇ���̃R���|�[�l���g�̔z����擾����
		@tparam	T	��������R���|�[�l���g�^
		@param[out]	�擾����R���|�[�l���g�̔z��
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		void GetUsedDynamicCompoentVec(std::vector<std::shared_ptr<T>>& CompVec) const {
			for (auto& v : GetGameObjectVec()) {
				auto ptr = v->GetDynamicComponent<T>(false);
				if (ptr) {
					CompVec.push_back(ptr);
				}
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�w��̃R���|�[�l���g�̐e���q�̃R���|�[�l���g�����I�u�W�F�N�g�̔z���ݒ肷��
		@tparam	T	��������R���|�[�l���g�^
		@param[out]	�擾����Q�[���I�u�W�F�N�g�̔z��
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		void GetUsedDynamicCompoentObjectVec(std::vector<std::shared_ptr<GameObject>>& GameObjectVec)const {
			for (auto& v : GetGameObjectVec()) {
				auto ptr = v->GetDynamicComponent<T>(false);
				if (ptr) {
					GameObjectVec.push_back(v);
				}
			}
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	�X�e�[�W���̍X�V�i�V�[�������΂��j
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void UpdateStage();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Փ˔���̍X�V�i�X�e�[�W����Ăԁj
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void UpdateCollision();


		virtual void OnUpdateConstantBuffers();
		virtual void OnCommitConstantBuffers();
		virtual void SetToBefore();

		virtual void OnPreCreate()override;
		virtual void OnCreate()override {}
		virtual void OnUpdate(double elapsedTime)override{}
		virtual void OnUpdate2(double elapsedTime)override{}
		virtual void OnShadowDraw()override;
		virtual void OnSceneDraw()override;
		virtual void OnDestroy()override;


	};



}
//end namespace basecross


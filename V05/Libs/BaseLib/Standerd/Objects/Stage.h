/*!
@file Stage.h
@brief �e�X�e�[�W�N���X
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#pragma once

#include "stdafx.h"

namespace basecross {

	class CollisionManager;

	//--------------------------------------------------------------------------------------
	// �e�X�e�[�W�N���X
	//--------------------------------------------------------------------------------------
	class Stage : public ObjectInterface {
		//update���邩�ǂ���
		bool m_updateActive;
		//�V���h�E�}�b�v���g�����ǂ���
		bool m_isShadowmapRender;

		void PushBackGameObject(const shared_ptr<GameObject>& ptr);
		void RemoveBackGameObject(const shared_ptr<GameObject>& ptr);
		//�ǉ��E�폜�܂��I�u�W�F�N�g�̒ǉ��ƍ폜
		void RemoveTargetGameObject(const shared_ptr<GameObject>& targetobj);
		void SetWaitToObjectVec();
		//�Q�[���I�u�W�F�N�g�̔z��
		vector<shared_ptr<GameObject>> m_gameObjectVec;
		//�ǉ��҂��̃Q�[���I�u�W�F�N�g
		vector<shared_ptr<GameObject>> m_waitAddObjectVec;
		//�폜�҂��̃Q�[���I�u�W�F�N�g
		vector<shared_ptr<GameObject>> m_waitRemoveObjectVec;
		//�V�F�A�I�u�W�F�N�g�|�C���^�̃}�b�v
		map<const wstring, weak_ptr<GameObject> > m_SharedMap;
		//�V�F�A�O���[�v�̃|�C���^�̃}�b�v
		map<const wstring, shared_ptr<GameObjectGroup> >  m_SharedGroupMap;

		//�R���W�����Ǘ���
		shared_ptr<CollisionManager> m_collisionManager;

		//�����v�Z���g�����ǂ���
		bool m_physicsActive;
		//�����v�Z
		BasePhysics m_basePhysics;
		//�����v�Z�Ŏg���󕨗��I�u�W�F�N�g��set
		set<uint16_t> m_vacantPhysicsIndices;

		//�Փ˔���}�l�[�W���̃p�t�H�[�}���X�J�E���^
		PerformanceCounter m_collisionPerformance;
		//Update�̃p�t�H�[�}���X�J�E���^
		PerformanceCounter m_updatePerformance;
		//Render�̃p�t�H�[�}���X�J�E���^
		PerformanceCounter m_renderPerformance;


	protected:
		Stage();
		virtual ~Stage() {}
	public:
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
		@brief	�V���h�E�}�b�v���g�����ǂ����𓾂�
		@return	�V���h�E�}�b�v���g�����ǂ���
		*/
		//--------------------------------------------------------------------------------------
		bool IsShadowmapRender() const {
			return m_isShadowmapRender;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�V���h�E�}�b�v���g�����ǂ�����ݒ肷��
		@param[in]	b	�V���h�E�}�b�v���g�����ǂ���
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetShadowmapRender(bool b) {
			m_isShadowmapRender = b;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�����v�Z�I�u�W�F�N�g�𓾂�
		@return	�����v�Z�I�u�W�F�N�g
		*/
		//--------------------------------------------------------------------------------------
		BasePhysics& GetBasePhysics(){
			if (!IsPhysicsActive()) {
				throw BaseException(
					L"�������Z�������ɂȂ��Ă��܂��B�L���ɂ��Ă���擾���Ă��������B",
					L"if (!IsPhysicsActive())",
					L"Stage::GetBasePhysics()"
				);
			}
			return m_basePhysics;
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	�����v�Z���L�����ǂ����𓾂�
		@return	�����v�Z���L�����ǂ���
		*/
		//--------------------------------------------------------------------------------------
		bool IsPhysicsActive() const {
			return m_physicsActive;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�����v�Z���L�����ǂ�����ݒ肷��
		@param[in]	b	�����v�Z���L�����ǂ���
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetPhysicsActive(bool b) {
			m_physicsActive = b;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�����I�u�W�F�N�g�̋󂢂Ă���ID���擾����
		@return	 �󂢂Ă���ID(�󂢂ĂȂ����UINT16_MAX��Ԃ��j
		*/
		//--------------------------------------------------------------------------------------
		uint16_t GetVacantPhysicsIndex();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�����ɂȂ��������I�u�W�F�N�gID��ǉ�����
		@param[in]	index	�����ɂ���ID
		@return	 �Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetVacantPhysicsIndex(uint16_t index);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Փ˔���}�l�[�W���𓾂�(const)
		@return	�Փ˔���}�l�[�W��
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<CollisionManager> GetCollisionManager() const {
			return m_collisionManager;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Q�[���I�u�W�F�N�g�̔z��𓾂�
		@return	�Q�[���I�u�W�F�N�g�̔z��
		*/
		//--------------------------------------------------------------------------------------
		vector<shared_ptr<GameObject>>& GetGameObjectVec() {
			return m_gameObjectVec;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Q�[���I�u�W�F�N�g�̔z��𓾂�(const)
		@return	�Q�[���I�u�W�F�N�g�̔z��
		*/
		//--------------------------------------------------------------------------------------
		const vector< shared_ptr<GameObject> >& GetGameObjectVec() const {
			return m_gameObjectVec;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Q�[���I�u�W�F�N�g��ǉ�����
		@tparam	T	�쐬����^
		@tparam	Ts	�ϒ��p�����[�^
		@return	�쐬���ꂽ�Q�[���I�u�W�F�N�g
		*/
		//--------------------------------------------------------------------------------------
		template<typename T, typename... Ts>
		shared_ptr<T> AddGameObject(Ts&&... params) {
			try {
				auto Ptr = ObjectFactory::Create<T>(GetThis<Stage>(), params...);
				PushBackGameObject(Ptr);
				return Ptr;
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
		shared_ptr<T> AddGameObjectWithParam(Ts&&... params) {
			try {
				auto Ptr = ObjectFactory::CreateGameObjectWithParam<T>(GetThis<Stage>(), params...);
				PushBackGameObject(Ptr);
				return Ptr;
			}
			catch (...) {
				throw;
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Q�[���I�u�W�F�N�g���폜����
		@param[in]	Obj	�C���X�^���X
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void RemoveGameObject(const shared_ptr<GameObject>& Obj);
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
		shared_ptr<T> GetSharedGameObjectEx(const wstring& key, bool exceptionActive = true)const {
			shared_ptr<T> ptr = dynamic_pointer_cast<T>(GetSharedGameObject(key, exceptionActive));
			if (ptr) {
				return ptr;
			}
			else {
				//�L���X�g�ł��Ȃ�
				if (exceptionActive) {
					//��O����
					wstring keyerr = key;
					wstring str = L"�I�u�W�F�N�g��";
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
		shared_ptr<GameObject> GetSharedGameObject(const wstring& key, bool exceptionActive = true)const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Q�[���I�u�W�F�N�g�����L����
		@param[in]	key	�L�[
		@param[in]	ptr	�Q�[���I�u�W�F�N�g�̃|�C���^
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetSharedGameObject(const wstring& key, const shared_ptr<GameObject>& ptr);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���L����Q�[���I�u�W�F�N�g�O���[�v���쐬����
		@param[in]	Key	�L�[
		@return	���L����Q�[���I�u�W�F�N�g�O���[�v
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<GameObjectGroup> CreateSharedObjectGroup(const wstring& Key);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���L����Q�[���I�u�W�F�N�g�O���[�v�𓾂�
		@param[in]	Key	�L�[
		@param[in]	ExceptionActive	������Ȃ��Ƃ��ɗ�O�𔭍s���邩�ǂ���
		@return	���L����Q�[���I�u�W�F�N�g�O���[�v
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<GameObjectGroup> GetSharedObjectGroup(const wstring& Key, bool ExceptionActive = true)const;
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
		shared_ptr<T> GetSharedObjectGroup(const wstring& Key, bool ExceptionActive = true)const {
			auto RetPtr = dynamic_pointer_cast<T>(GetSharedObjectGroup(Key, ExceptionActive));
			if (RetPtr) {
				return RetPtr;
			}
			else {
				if (ExceptionActive) {
					//��O����
					wstring keyerr = Key;
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
		void SetSharedObjectGroup(const wstring& Key, const shared_ptr<GameObjectGroup>& NewPtr);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�w��̃^�O�����Q�[���I�u�W�F�N�g�̔z����擾����
		@param[in]	Tag	��������^�O
		@param[out]	�擾����Q�[���I�u�W�F�N�g�̔z��
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void GetUsedTagObjectVec(const wstring& Tag, vector<shared_ptr<GameObject>>& ObjVec) const {
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
		void GetUsedDynamicCompoentVec(vector<shared_ptr<T>>& CompVec) const {
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
		void GetUsedDynamicCompoentObjectVec(vector<shared_ptr<GameObject>>& GameObjectVec)const {
			for (auto& v : GetGameObjectVec()) {
				auto ptr = v->GetDynamicComponent<T>(false);
				if (ptr) {
					GameObjectVec.push_back(v);
				}
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�A�N�e�B�u�ȃJ������Ԃ��i�������z�֐��j
		@return	�A�N�e�B�u�ȃJ����
		*/
		//--------------------------------------------------------------------------------------
		virtual shared_ptr<Camera> GetActiveCamera() const = 0;

		//--------------------------------------------------------------------------------------
		/*!
		@brief	�A�N�e�B�u�ȃ��C�g�Z�b�g��Ԃ��i�������z�֐��j
		@return	�A�N�e�B�u�ȃ��C�g�Z�b�g
		*/
		//--------------------------------------------------------------------------------------
		virtual shared_ptr<LightSet> GetActiveLightSet() const = 0;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Փ˔���̃p�t�H�[�}���X��L�������ɂ���
		@param[in]	b	�Փ˔���̃p�t�H�[�}���X��L���ɂ��邩�ǂ���
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetCollisionPerformanceActive(bool b) {
			m_collisionPerformance.SetActive(b);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Փ˔���̃p�t�H�[�}���X��L�������𓾂�
		@return	�Փ˔���̃p�t�H�[�}���X��L���Ȃ�true
		*/
		//--------------------------------------------------------------------------------------
		bool IsCollisionPerformanceActive() const {
			return m_collisionPerformance.IsAvtive();
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Փ˔���̃p�t�H�[�}���X�𓾂�
		@return	�Փ˔���̃p�t�H�[�}���X�i�~���b�j
		*/
		//--------------------------------------------------------------------------------------
		float GetCollisionPerformanceTime() const {
			return m_collisionPerformance.GetPerformanceTime();
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Update�����̃p�t�H�[�}���X��L�������ɂ���
		@param[in]	b	Update�����̃p�t�H�[�}���X��L���ɂ��邩�ǂ���
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetUpdatePerformanceActive(bool b) {
			m_updatePerformance.SetActive(b);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Update�����̃p�t�H�[�}���X��L�������𓾂�
		@return	Update�����̃p�t�H�[�}���X��L���Ȃ�true
		*/
		//--------------------------------------------------------------------------------------
		bool IsUpdatePerformanceActive() const {
			return m_updatePerformance.IsAvtive();
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Update�����̃p�t�H�[�}���X�𓾂�
		@return	Update�����̃p�t�H�[�}���X�i�~���b�j
		*/
		//--------------------------------------------------------------------------------------
		float GetUpdatePerformanceTime() const {
			return m_updatePerformance.GetPerformanceTime();
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Render�����̃p�t�H�[�}���X��L�������ɂ���
		@param[in]	b	Render�����̃p�t�H�[�}���X��L���ɂ��邩�ǂ���
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetRenderPerformanceActive(bool b) {
			m_renderPerformance.SetActive(b);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Render�����̃p�t�H�[�}���X��L�������𓾂�
		@return	Render�����̃p�t�H�[�}���X��L���Ȃ�true
		*/
		//--------------------------------------------------------------------------------------
		bool IsRenderPerformanceActive() const {
			return m_renderPerformance.IsAvtive();
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Render�����̃p�t�H�[�}���X�𓾂�
		@return	Render�����̃p�t�H�[�}���X�i�~���b�j
		*/
		//--------------------------------------------------------------------------------------
		float GetRenderPerformanceTime() const {
			return m_renderPerformance.GetPerformanceTime();
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�t���[��������������
		@param[in]	pBaseFrame	�^�[�Q�b�g�ƂȂ�t���[��
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnInitFrame(BaseFrame* pBaseFrame);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�R���X�^���g�o�b�t�@����������
		@param[in]	pBaseFrame	�^�[�Q�b�g�ƂȂ�t���[��
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void WriteConstantBuffers(BaseFrame* pBaseFrame);
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
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�X�V�����i�\�z���ɂ�΂��j
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnPreCreate()override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�X�V
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnUpdate()override {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	��X�V
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnUpdate2() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�J�����ƃ��C�g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnUpdateLightCamera() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�`��
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnDraw()override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�V���h�E�}�b�v�`��
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnShadowmapDraw();

		//--------------------------------------------------------------------------------------
		/*!
		@brief	�j�����ɌĂ΂��
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnDestroy()override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�L�[�̉���
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnKeyDown(UINT8 key)override {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�L�[�𗣂�
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnKeyUp(UINT8 key) override {}

		//BaseCrossDx12�Ŏ�����~
		// �X�e�[�W�̐e�q�֌W�͒�~
		//vector< shared_ptr<Stage> >& GetChileStageVec();
		//vector< shared_ptr<Stage> >& GetChileStageVec() const;
		//template<typename T>
		//shared_ptr<Stage> AddChileStage();
		//shared_ptr<Stage> GetParentStage() const;
		//template<typename T>
		//bool FindGameObject(const shared_ptr<GameObject>& Obj);

	};


}
// end namespace basecross
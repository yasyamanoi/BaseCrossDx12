/*!
@file GameObject.h
@brief �Q�[���I�u�W�F�N�g�N���X
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	class BaseScene;
	class Stage;
	class Component;
	class Transform;
	struct CollisionPair;

	//--------------------------------------------------------------------------------------
	// �z�u�����I�u�W�F�N�g
	//--------------------------------------------------------------------------------------
	class GameObject : public ObjectInterface {
		//�X�e�[�W�e�̃|�C���^
		weak_ptr<Stage> m_stage;
		//update���邩�ǂ���
		bool m_updateActive;
		//Render���邩�ǂ���
		bool m_renderActive;
		//�������ǂ���
		bool m_alphaActive;

		int m_drawLayer = 0;	//�`�惌�C���[
		set<wstring> m_tagSet;	//�^�O�̃Z�b�g
		set<int> m_numTagSet;	//�����^�O�̃Z�b�g

		//�R���|�[�l���g�̃}�b�v
		map<type_index, shared_ptr<Component> > m_compMap;
		//�R���|�[�l���g���s����
		list<type_index> m_compOrder;
		//�w��̃R���|�[�l���g��T���i�h���N���X�������j
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
		//�s���̃}�b�v
		map<type_index, shared_ptr<Behavior>> m_BehaviorMap;

		const map<type_index, shared_ptr<Behavior> >& GetBehaviorMap() const {
			return m_BehaviorMap;
		}
		map<type_index, shared_ptr<Behavior> >& GetBehaviorMap() {
			return m_BehaviorMap;
		}
		shared_ptr<Behavior> SearchBehavior(type_index TypeIndex)const {
			auto it = m_BehaviorMap.find(TypeIndex);
			if (it != m_BehaviorMap.end()) {
				return it->second;
			}
			return nullptr;

		}
		void AddMakedBehavior(type_index TypeIndex, const shared_ptr<Behavior>& Ptr) {
			//map�ɒǉ��������͍X�V
			m_BehaviorMap[TypeIndex] = Ptr;
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
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�^�O�̃Z�b�g�𓾂�(const)
		@return	�^�O�̃Z�b�g
		*/
		//--------------------------------------------------------------------------------------
		const set<wstring>& GetTagSet() const {
			return m_tagSet;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�^�O�̃Z�b�g�𓾂�
		@return	�^�O�̃Z�b�g
		*/
		//--------------------------------------------------------------------------------------
		set<wstring>& GetTagSet() {
			return m_tagSet;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�w�肷��^�O�����݂��邩�ǂ����𓾂�
		@param[in]	tagstr	���؂���^�O
		@return	���݂����true
		*/
		//--------------------------------------------------------------------------------------
		bool FindTag(const wstring& tagstr) const {
			if (m_tagSet.find(tagstr) == m_tagSet.end()) {
				return false;
			}
			return true;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�w�肷��^�O��ǉ�����
		@param[in]	tagstr	�ǉ�����^�O
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void  AddTag(const wstring& tagstr) {
			if (tagstr == L"") {
				//�󔒂Ȃ��O
				throw BaseException(
					L"�ݒ肷��^�O����ł�",
					L"if (tagstr == L"")",
					L"GameObject::AddTag()"
				);
			}
			m_tagSet.insert(tagstr);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�w�肷��^�O�����݂�����폜����i���݂��Ȃ��ꍇ�͉������Ȃ��j
		@param[in]	tagstr	�폜����^�O
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void  RemoveTag(const wstring& tagstr) {
			m_tagSet.erase(tagstr);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�����^�O�̃Z�b�g�𓾂�(const)
		@return	�����^�O�̃Z�b�g
		*/
		//--------------------------------------------------------------------------------------
		const set<int>& GetNumTagSet() const {
			return m_numTagSet;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�����^�O�̃Z�b�g�𓾂�
		@return	�����^�O�̃Z�b�g
		*/
		//--------------------------------------------------------------------------------------
		set<int>& GetNumTagSet() {
			return m_numTagSet;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�w�肷�鐔���^�O�����݂��邩�ǂ����𓾂�
		@param[in]	numtag	���؂��鐔���^�O
		@return	���݂����true
		*/
		//--------------------------------------------------------------------------------------
		bool FindNumTag(int numtag) const {
			if (m_numTagSet.find(numtag) == m_numTagSet.end()) {
				return false;
			}
			return true;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�w�肷�鐔���^�O��ǉ�����
		@param[in]	numtag	�ǉ����鐔���^�O
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void  AddNumTag(int numtag) {
			m_numTagSet.insert(numtag);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�w�肷�鐔���^�O�����݂�����폜����i���݂��Ȃ��ꍇ�͉������Ȃ��j
		@param[in]	numtag	�폜���鐔���^�O
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void  RemoveNumTag(int numtag) {
			m_numTagSet.erase(numtag);
		}



		shared_ptr<Stage> GetStage(bool exceptionActive = true) const;
		template<typename T, typename... Ts>
		shared_ptr<T> AddComponent(Ts&&... params) {
			type_index t_index = type_index(typeid(T));
			auto ptr = SearchDynamicComponent<T>();
			//�w��̌^�̃R���|�[�l���g����������
			if (ptr) {
				auto retPtr = dynamic_pointer_cast<T>(ptr);
				if (retPtr) {
					return retPtr;
				}
				else {
					throw BaseException(
						L"���łɃR���|�[�l���g������܂������A�^�L���X�g�ł��܂���",
						Util::GetWSTypeName<T>(),
						L"GameObject::AddComponent<T>()"
					);
				}
			}
			else {
				//������Ȃ��B�V���ɍ쐬����
				shared_ptr<T> newPtr = ObjectFactory::Create<T>(GetThis<GameObject>(), params...);
				//���̃R���|�[�l���g���܂��Ȃ���ΐV�K�o�^
				m_compOrder.push_back(t_index);
				//map�ɒǉ��������͍X�V
				m_compMap[t_index] = newPtr;
				newPtr->AttachGameObject(GetThis<GameObject>());
				return newPtr;
			}
		}


		template <typename T>
		shared_ptr<T> GetComponent(bool exceptionActive = true)const {
			auto Ptr = SearchDynamicComponent<T>();
			if (Ptr) {
				//�w��̌^�̃R���|�[�l���g����������
				return Ptr;
			}
			else {
				if (exceptionActive) {
					throw BaseException(
						L"�R���|�[�l���g��������܂���",
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
			//���ԃ��X�g�����؂��č폜
			auto it = m_compOrder.begin();
			while (it != m_compOrder.end()) {
				if (*it == typeIndex) {
					auto it2 = m_compMap.find(*it);
					if (it2 != m_compMap.end()) {
						//�w��̌^�̃R���|�[�l���g����������
						//map�f�[�^���폜
						m_compMap.erase(it2);
					}
					m_compOrder.erase(it);
					return;
				}
				it++;
			}
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	�s���̎擾�B���݂��Ȃ���΍쐬����
		@tparam	T	�擾����^
		@return	�R���|�[�l���g
		*/
		//--------------------------------------------------------------------------------------
		template <typename T>
		shared_ptr<T> GetBehavior() {
			auto Ptr = SearchBehavior(type_index(typeid(T)));
			if (Ptr) {
				//�w��̌^�̍s������������
				auto RetPtr = dynamic_pointer_cast<T>(Ptr);
				if (RetPtr) {
					return RetPtr;
				}
				else {
					throw BaseException(
						L"�s��������܂������A�^�L���X�g�ł��܂���",
						Util::GetWSTypeName<T>(),
						L"GameObject::GetBehavior<T>()"
					);
				}
			}
			else {
				//������ΐV���ɐ��삷��
				shared_ptr<T> newPtr = ObjectFactory::Create<T>(GetThis<GameObject>());
				AddMakedBehavior(type_index(typeid(T)), newPtr);
				return newPtr;
			}
			return nullptr;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�s���̌����B
		@tparam	T	�擾����^
		@return	���݂����true
		*/
		//--------------------------------------------------------------------------------------
		template <typename T>
		bool FindBehavior() {
			auto Ptr = SearchBehavior(type_index(typeid(T)));
			if (Ptr) {
				//�w��̌^�̍s������������
				auto RetPtr = dynamic_pointer_cast<T>(Ptr);
				if (RetPtr) {
					return true;
				}
				else {
					return false;
				}
			}
			return false;
		}



		void TransformInit();
		void ComponentUpdate();
		void ComponentShadowmapRender();
		void ComponentRender();
		void ComponentDestroy();

		//--------------------------------------------------------------------------------------
		/*!
		@brief	���̃I�u�W�F�N�g��2D�`�悷�邽�߂̎ˉe�s��𓾂�i�f�t�H���g�̓X�e�[�W�̃r���[����擾�j
		@return	ProjMatrix	�ˉe�s��
		*/
		//--------------------------------------------------------------------------------------
		virtual Mat4x4 Get2DDrawProjMatrix() const;

		//--------------------------------------------------------------------------------------
		/*!
		@brief	���̃I�u�W�F�N�g��`�悷�邽�߂̃J�����𓾂�i�f�t�H���g�̓X�e�[�W����擾�j
		@return	�J������shared_ptr
		*/
		//--------------------------------------------------------------------------------------
		virtual shared_ptr<BaseCamera> GetActiveCamera() const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���̃I�u�W�F�N�g���Ƃ炷���C�g�Z�b�g�𓾂�i�f�t�H���g�̓X�e�[�W����擾�j
		@return	���C�g�Z�b�g��shared_ptr
		*/
		//--------------------------------------------------------------------------------------
		virtual shared_ptr<LightSet> GetActiveLightSet() const;

		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Փ˔��������̃C�x���g�i�f�t�H���g�͉������Ȃ��j�B�����������ꍇ�͕�����Ă΂��
		@param[in]	Other	�V�����Փ˂�������
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCollisionEnter(shared_ptr<GameObject>& Other) {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Փ˔��������̃C�x���g�i�f�t�H���g�͉������Ȃ��j�B�����������ꍇ�͕�����Ă΂��
		@param[in]	Pair �y�A
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCollisionEnter(const CollisionPair& Pair) {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Փ˂������鑊�肪�������ꍇ�̃C�x���g�i�f�t�H���g�͉������Ȃ��j�B�����������ꍇ�͕�����Ă΂��
		@param[in]	OtherVec	�Փ˂�����������
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCollisionExcute(shared_ptr<GameObject>& Other) {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Փ˂������鑊�肪�������ꍇ�̃C�x���g�i�f�t�H���g�͉������Ȃ��j�B�����������ꍇ�͕�����Ă΂��
		@param[in]	Pair �y�A
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCollisionExcute(const CollisionPair& Pair) {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Փ˂𔲂������肪�������ꍇ�̃C�x���g�i�f�t�H���g�͉������Ȃ��j�B�����������ꍇ�͕�����Ă΂��
		@param[in]	OtherVec	�Փ˂𔲂�������̔z��
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCollisionExit(shared_ptr<GameObject>& Other) {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Փ˂𔲂������肪�������ꍇ�̃C�x���g�i�f�t�H���g�͉������Ȃ��j�B�����������ꍇ�͕�����Ă΂��
		@param[in]	Pair �y�A
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCollisionExit(const CollisionPair& Pair) {}

		virtual void OnPreInit() override;
		virtual void OnUpdate()override {}
		virtual void OnUpdate2()override {}
		virtual void OnShadowmapRender();
		virtual void OnRender()override;
		virtual void OnInitFrame(BaseFrame* pBaseFrame);
		virtual void WriteConstantBuffers(BaseFrame* pBaseFrame);
		virtual void OnDestroy()override {}


	};


	//--------------------------------------------------------------------------------------
	///	�Q�[���I�u�W�F�N�g��weak_ptr���O���[�v����������
	//--------------------------------------------------------------------------------------
	class GameObjectGroup : public ObjectInterface {
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�R���X�g���N�^
		*/
		//--------------------------------------------------------------------------------------
		GameObjectGroup();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�f�X�g���N�^
		*/
		//--------------------------------------------------------------------------------------
		virtual ~GameObjectGroup();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�O���[�v���̃Q�[���I�u�W�F�N�g��weak_ptr�̔z��𓾂�
		@return	�O���[�v���̃Q�[���I�u�W�F�N�g��weak_ptr�̔z��
		*/
		//--------------------------------------------------------------------------------------
		const vector< weak_ptr<GameObject> >& GetGroupVector() const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�O���[�v���̃Q�[���I�u�W�F�N�g�𓾂�
		@param[in]	index	�O���[�v���I�u�W�F�N�g�̃C���f�b�N�X
		@return	�O���[�v���̃Q�[���I�u�W�F�N�g
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<GameObject> at(size_t index);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�O���[�v���̃Q�[���I�u�W�F�N�g�̐��𓾂�
		@return	�O���[�v���̃Q�[���I�u�W�F�N�g�̐�
		*/
		//--------------------------------------------------------------------------------------
		size_t size() const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�O���[�v�ɃQ�[���I�u�W�F�N�g��ǉ�����
		@param[in]	Obj	�ǉ�����Q�[���I�u�W�F�N�g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void IntoGroup(const shared_ptr<GameObject>& Obj);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�O���[�v���N���A����
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void AllClear();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	������
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnInit()override {}

		virtual void OnUpdate()override {}
		virtual void OnRender() override {}
		virtual void OnDestroy() override {}


	private:
		// pImpl�C�f�B�I��
		struct Impl;
		unique_ptr<Impl> pImpl;
	};



}
// end namespace basecross
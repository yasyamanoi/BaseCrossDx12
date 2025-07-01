/*!
@file GameObject.h
@brief �Q�[���I�u�W�F�N�g�N���X
*/


#pragma once
#include "stdafx.h"


namespace basecross {

	class Stage;
	class Behavior;
	class Collision;
	struct CollisionPair;

	//--------------------------------------------------------------------------------------
	// �z�u�����I�u�W�F�N�g�̐e
	//--------------------------------------------------------------------------------------
	class GameObject : public ObjectInterface {
		std::weak_ptr<Stage> m_stage;

		//update���邩�ǂ���
		bool m_updateActive;
		//draw���邩�ǂ���
		bool m_drawActive;
		//�������ǂ���
		bool m_alphaActive;


		//�s���̃}�b�v
		std::map<std::type_index, std::shared_ptr<Behavior>> m_BehaviorMap;
		//�R���|�[�l���g�̃}�b�v
		std::map<std::type_index, std::shared_ptr<Component> > m_compMap;
		//�R���|�[�l���g���s����
		std::list<std::type_index> m_compOrder;
		//�w��̃R���|�[�l���g��T���i�h���N���X�������j
		template<typename T>
		std::shared_ptr<T> SearchDynamicComponent()const {
			auto it = m_compMap.begin();
			while (it != m_compMap.end()) {
				auto ptr = std::dynamic_pointer_cast<T>(it->second);
				if (ptr) {
					return ptr;
				}
				it++;
			}
			return nullptr;
		}


		int m_drawLayer = 0;	//�`�惌�C���[
		std::set<std::wstring> m_tagSet;	//�^�O�̃Z�b�g
		std::set<int> m_numTagSet;	//�����^�O�̃Z�b�g


		const std::map<std::type_index, std::shared_ptr<Behavior> >& GetBehaviorMap() const {
			return m_BehaviorMap;
		}
		std::map<std::type_index, std::shared_ptr<Behavior> >& GetBehaviorMap() {
			return m_BehaviorMap;
		}
		std::shared_ptr<Behavior> SearchBehavior(std::type_index TypeIndex)const {
			auto it = m_BehaviorMap.find(TypeIndex);
			if (it != m_BehaviorMap.end()) {
				return it->second;
			}
			return nullptr;

		}
		void AddMakedBehavior(std::type_index TypeIndex, const std::shared_ptr<Behavior>& Ptr) {
			//map�ɒǉ��������͍X�V
			m_BehaviorMap[TypeIndex] = Ptr;
		}
	protected:
		TransParam m_tempParam;
		GameObject(const std::shared_ptr<Stage>& stage):
			m_stage(stage),
			m_tempParam(),
			m_updateActive(true),
			m_drawActive(true),
			m_alphaActive(false)
		{
		}
		virtual ~GameObject() {}


	public:
		std::shared_ptr<Stage> GetStage(bool exceptionActive = true) const;
		bool IsUpdateActive() const {
			return m_updateActive;
		}
		void SetUpdateActive(bool b) {
			m_updateActive = b;
		}
		bool IsDrawActive() const {
			return m_drawActive;
		}
		void SetDrawActive(bool b) {
			m_drawActive = b;
		}
		bool IsAlphaActive() const {
			return m_alphaActive;
		}
		void SetAlphaActive(bool b) {
			m_alphaActive = b;
		}

		virtual std::shared_ptr<Camera> GetCamera()const;
		virtual std::shared_ptr<LightSet> GetLightSet() const;


		template<typename T, typename... Ts>
		std::shared_ptr<T> AddComponent(Ts&&... params) {
			std::type_index t_index = std::type_index(typeid(T));
			auto ptr = SearchDynamicComponent<T>();
			//�w��̌^�̃R���|�[�l���g����������
			if (ptr) {
				auto retPtr = std::dynamic_pointer_cast<T>(ptr);
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
				std::shared_ptr<T> newPtr = ObjectFactory::Create<T>(GetThis<GameObject>(), params...);
				//���̃R���|�[�l���g���܂��Ȃ���ΐV�K�o�^
				m_compOrder.push_back(t_index);
				//map�ɒǉ��������͍X�V
				m_compMap[t_index] = newPtr;
				newPtr->AttachGameObject(GetThis<GameObject>());
				return newPtr;
			}
		}


		template <typename T>
		std::shared_ptr<T> GetComponent(bool exceptionActive = true)const {
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
		std::shared_ptr<T> GetBehavior() {
			auto Ptr = SearchBehavior(std::type_index(typeid(T)));
			if (Ptr) {
				//�w��̌^�̍s������������
				auto RetPtr = std::dynamic_pointer_cast<T>(Ptr);
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
				std::shared_ptr<T> newPtr = ObjectFactory::Create<T>(GetThis<GameObject>());
				AddMakedBehavior(std::type_index(typeid(T)), newPtr);
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


		//--------------------------------------------------------------------------------------
		/*!
		@brief	�^�O�̃Z�b�g�𓾂�(const)
		@return	�^�O�̃Z�b�g
		*/
		//--------------------------------------------------------------------------------------
		const std::set<std::wstring>& GetTagSet() const {
			return m_tagSet;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�^�O�̃Z�b�g�𓾂�
		@return	�^�O�̃Z�b�g
		*/
		//--------------------------------------------------------------------------------------
		std::set<std::wstring>& GetTagSet() {
			return m_tagSet;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�w�肷��^�O�����݂��邩�ǂ����𓾂�
		@param[in]	tagstr	���؂���^�O
		@return	���݂����true
		*/
		//--------------------------------------------------------------------------------------
		bool FindTag(const std::wstring& tagstr) const {
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
		void  AddTag(const std::wstring& tagstr) {
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
		void  RemoveTag(const std::wstring& tagstr) {
			m_tagSet.erase(tagstr);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�����^�O�̃Z�b�g�𓾂�(const)
		@return	�����^�O�̃Z�b�g
		*/
		//--------------------------------------------------------------------------------------
		const std::set<int>& GetNumTagSet() const {
			return m_numTagSet;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�����^�O�̃Z�b�g�𓾂�
		@return	�����^�O�̃Z�b�g
		*/
		//--------------------------------------------------------------------------------------
		std::set<int>& GetNumTagSet() {
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

		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Փ˔��������̃C�x���g�i�f�t�H���g�͉������Ȃ��j�B�����������ꍇ�͕�����Ă΂��
		@param[in]	Other	�V�����Փ˂�������
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCollisionEnter(std::shared_ptr<GameObject>& Other) {}
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
		virtual void OnCollisionExcute(std::shared_ptr<GameObject>& Other) {}
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
		virtual void OnCollisionExit(std::shared_ptr<GameObject>& Other) {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Փ˂𔲂������肪�������ꍇ�̃C�x���g�i�f�t�H���g�͉������Ȃ��j�B�����������ꍇ�͕�����Ă΂��
		@param[in]	Pair �y�A
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCollisionExit(const CollisionPair& Pair) {}

		void ComponentUpdate();
		void TransformInit();
		void ComponentShadowmapRender();
		void ComponentRender();
		void ComponentDestroy();

		virtual void OnUpdateConstantBuffers();
		virtual void OnCommitConstantBuffers();
		virtual void OnPreCreate()override;
		virtual void OnCreate()override {}
		virtual void OnShadowDraw()override;
		virtual void OnSceneDraw()override;
		virtual void OnDestroy()override {}
		virtual void SetToBefore();

	};


	//--------------------------------------------------------------------------------------
	///	�Q�[���I�u�W�F�N�g��weak_ptr���O���[�v����������
	//--------------------------------------------------------------------------------------
	class GameObjectGroup : public ObjectInterface {
		std::vector< std::weak_ptr<GameObject> > m_Group;
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
		const std::vector< std::weak_ptr<GameObject> >& GetGroupVector() const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�O���[�v���̃Q�[���I�u�W�F�N�g�𓾂�
		@param[in]	index	�O���[�v���I�u�W�F�N�g�̃C���f�b�N�X
		@return	�O���[�v���̃Q�[���I�u�W�F�N�g
		*/
		//--------------------------------------------------------------------------------------
		std::shared_ptr<GameObject> at(size_t index);
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
		void IntoGroup(const std::shared_ptr<GameObject>& Obj);
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
		virtual void OnCreate()override {}
		virtual void OnUpdate(double elapsedTime) override{}
		virtual void OnShadowDraw()override{}
		virtual void OnSceneDraw()override{}
		virtual void OnDestroy() override {}
	};
}
// end namespace basecross

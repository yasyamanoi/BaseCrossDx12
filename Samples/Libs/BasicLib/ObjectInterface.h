/*!
@file ObjectInterface.h
@brief GameObject���̐e�N���X
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#pragma once

#include "stdafx.h"

namespace basecross {


	class ObjectFactory;
	class ObjectInterface;



	//--------------------------------------------------------------------------------------
	///	Object�C���^�[�t�F�C�X
	//--------------------------------------------------------------------------------------
	class ObjectInterface : public std::enable_shared_from_this<ObjectInterface> {
		friend class ObjectFactory;
	protected:
		ObjectInterface() {}
		virtual ~ObjectInterface() {}
	public:
		//�N���G�C�g�ς݂��ǂ���
		//Create�֐����Ăяo�����true�ɂȂ�
		bool m_created{ false };
		void SetCreated(bool b) {
			m_created = b;
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief this�|�C���^�ishared_ptr�j�̎擾
		@tparam T	�|�C���^�̌^
		@return�@�I�u�W�F�N�g��shared_ptr
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		std::shared_ptr<T> GetThis() {
			auto ptr = std::dynamic_pointer_cast<T>(shared_from_this());
			if (ptr) {
				return ptr;
			}
			else {
				std::wstring str(L"this��");
				str += Util::GetWSTypeName<T>();
				str += L"�^�ɃL���X�g�ł��܂���";
				throw BaseException(
					str,
					L"if( ! dynamic_pointer_cast<T>(shared_from_this()) )",
					L"ObjectInterface::GetThis()"
				);
			}
			return nullptr;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief �N���G�C�g�ς݂����擾
		@return�@�N���G�C�g�ς݂Ȃ�true
		*/
		//--------------------------------------------------------------------------------------
		bool IsCreated()const {
			return m_created;
		}
		//���z�֐��Q
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�������O����
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnPreCreate() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	����������
		@param[in]	pCommandList �R�}���h���X�g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCreate() = 0;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�X�V����
		@param[in]	elapsedTime �O�̃^�[������̎���
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnUpdate(double elapsedTime) = 0;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�X�V�����Q
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnUpdate2(double elapsedTime) {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�`�揈��
		@param[in]	pCommandList �R�}���h���X�g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnShadowDraw(){}
		virtual void OnSceneDraw() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�j��������
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnDestroy(){}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�L�[�{�[�h�����ꂽ����
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnKeyDown(UINT8 /*key*/) {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�L�[�{�[�h�����ꂽ����
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnKeyUp(UINT8 /*key*/) {}
	private:
		//�R�s�[�֎~
		ObjectInterface(const ObjectInterface&) = delete;
		ObjectInterface& operator=(const ObjectInterface&) = delete;
		//���[�u�֎~
		ObjectInterface(const ObjectInterface&&) = delete;
		ObjectInterface& operator=(const ObjectInterface&&) = delete;
	};


	//--------------------------------------------------------------------------------------
	///	Object���\�z����
	//--------------------------------------------------------------------------------------
	class ObjectFactory {
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief �I�u�W�F�N�g�쐬�istatic�֐��j
		@tparam T	�쐬����^
		@tparam Ts...	�ϒ��p�����[�^�^
		@param[in]	params	�ϒ��p�����[�^
		@return�@�쐬�����I�u�W�F�N�g��shared_ptr
		*/
		//--------------------------------------------------------------------------------------
		template<typename T, typename... Ts>
		static std::shared_ptr<T> Create(Ts&&... params) {
			std::shared_ptr<T> ptr = std::shared_ptr<T>(new T(params...));
			//�������֐��Ăяo��
			ptr->OnPreCreate();
			ptr->OnCreate();
			ptr->SetCreated(true);
			return ptr;
		}


		//--------------------------------------------------------------------------------------
		/*!
		@brief �I�u�W�F�N�g�쐬�istatic�֐��j�B�p�����[�^��OnInit�ɓn�����
		@tparam T	�쐬����^
		@tparam Ts...	�ϒ��p�����[�^�^
		@param[in]	params	�ϒ��p�����[�^
		@return�@�쐬�����I�u�W�F�N�g��shared_ptr
		*/
		//--------------------------------------------------------------------------------------
		template<typename T, typename... Ts>
		static std::shared_ptr<T> CreateInitParam(Ts&&... params) {
			std::shared_ptr<T> ptr = std::shared_ptr<T>(new T());
			//�������֐��Ăяo��
			ptr->OnCreate(params...);
			ptr->SetCreated(true);
			return ptr;
		}

	};


}
// end namespace basecross

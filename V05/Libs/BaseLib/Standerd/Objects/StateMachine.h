/*!
@file Stage.h
@brief �e�X�e�[�W�N���X
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#pragma once

#include "stdafx.h"

namespace basecross {

	template <typename T>
	class StateMachine;


	//--------------------------------------------------------------------------------------
	///	�X�e�[�g�����e���v���[�g�N���X(���ۃN���X)
	/*!
	@tparam T	�I�[�i�[�̌^
	*/
	//--------------------------------------------------------------------------------------
	template <typename T>
	class ObjState {
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�R���X�g���N�^
		*/
		//--------------------------------------------------------------------------------------
		ObjState() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�f�X�g���N�^
		*/
		//--------------------------------------------------------------------------------------
		virtual ~ObjState() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�X�e�[�g�ɓ������Ƃ��Ɏ��s�����
		@param[in]	Obj	�X�e�[�g��ێ�����I�u�W�F�N�g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void Enter(const shared_ptr<T>& Obj) = 0;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Update�̂Ƃ��Ɏ��s�����
		@param[in]	Obj	�X�e�[�g��ێ�����I�u�W�F�N�g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void Execute(const shared_ptr<T>& Obj) = 0;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�X�e�[�g���o��Ƃ��Ɏ��s�����
		@param[in]	Obj	�X�e�[�g��ێ�����I�u�W�F�N�g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void Exit(const shared_ptr<T>& Obj) = 0;
		//�ȉ��A�K�w���ɂ��ǉ����\�b�h�i�������z�֐��ɂ��Ȃ��j
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Update2�̂Ƃ��Ɏ��s�����
		@param[in]	Obj	�X�e�[�g��ێ�����I�u�W�F�N�g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void Execute2(const shared_ptr<T>& Obj) {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	��ɃX�e�[�g��push���ꂽ�Ƃ��ɌĂ΂��
		@param[in]	Obj	�X�e�[�g��ێ�����I�u�W�F�N�g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void Sleep(const shared_ptr<T>& Obj) {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	��̃X�e�[�g��pop���ꂽ�Ƃ��ɌĂ΂��
		@param[in]	Obj	�X�e�[�g��ێ�����I�u�W�F�N�g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void WakeUp(const shared_ptr<T>& Obj) {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�X�e�[�g���𓾂�
		@param[in]	�X�e�[�g��
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual wstring GetStateName()const { return L""; }
	};

	//--------------------------------------------------------------------------------------
	///	�X�e�[�g�}�V�������e���v���[�g�N���X(���ۃN���X)
	/*!
	@tparam T	�I�[�i�[�̌^
	*/
	//--------------------------------------------------------------------------------------
	template <typename T>
	class StateMachine
	{
	private:
		//���̃X�e�[�g�}�V�������I�[�i�[
		weak_ptr<T> m_Owner;
		//���݂̃X�e�[�g
		weak_ptr< ObjState<T> > m_CurrentState;
		//��O�̃X�e�[�g
		weak_ptr< ObjState<T> > m_PreviousState;
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�R���X�g���N�^
		@param[in]	owner	�I�[�i�[�̃|�C���^
		*/
		//--------------------------------------------------------------------------------------
		explicit StateMachine(const shared_ptr<T>& owner) :
			m_Owner(owner)
			//m_CurrentState(nullptr), m_PreviousState(nullptr)
		{}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�f�X�g���N�^
		*/
		//--------------------------------------------------------------------------------------
		virtual ~StateMachine() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�J�����g�X�e�[�g��ݒ肷��
		@param[in]	s	�ݒ肷��X�e�[�g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetCurrentState(const shared_ptr< ObjState<T> >& s) { m_CurrentState = s; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	1�O�̃X�e�[�g��ݒ肷��
		@param[in]	s	�ݒ肷��X�e�[�g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetPreviousState(const shared_ptr< ObjState<T> >& s) { m_PreviousState = s; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�J�����g�X�e�[�g�𓾂�
		@return	�J�����g�X�e�[�g
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr< ObjState<T> >  GetCurrentState() const {
			auto shptr = m_CurrentState.lock();
			if (shptr) {
				return shptr;
			}
			return nullptr;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	1�O�̃X�e�[�g�𓾂�
		@return	1�O�̃X�e�[�g
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr< ObjState<T> >  GetPreviousState()const {
			auto shptr = m_PreviousState.lock();
			if (shptr) {
				return shptr;
			}
			return nullptr;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�X�e�[�g���X�V����
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void Update() const {
			auto shptr = m_CurrentState.lock();
			auto ow_shptr = m_Owner.lock();
			if (shptr && ow_shptr) {
				shptr->Execute(ow_shptr);
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�X�e�[�g���X�V2����
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void Update2() const {
			auto shptr = m_CurrentState.lock();
			auto ow_shptr = m_Owner.lock();
			if (shptr && ow_shptr) {
				shptr->Execute2(ow_shptr);
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�X�e�[�g��ύX����
		@param[in]	NewState	�ݒ肷��X�e�[�g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void  ChangeState(const shared_ptr< ObjState<T> >& NewState) {
			//���̃X�e�[�g��ۑ�
			m_PreviousState = m_CurrentState;
			auto shptr = m_CurrentState.lock();
			auto ow_shptr = m_Owner.lock();
			if (shptr && ow_shptr) {
				//���̃X�e�[�g�ɏI����m�点��
				shptr->Exit(ow_shptr);
			}
			//�V�����X�e�[�g���J�����g�ɐݒ�
			m_CurrentState = NewState;
			shptr = m_CurrentState.lock();
			if (shptr && ow_shptr) {
				//���̃X�e�[�g�ɏI����m�点��
				shptr->Enter(ow_shptr);
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�X�e�[�g����O�̃X�e�[�g�ɖ߂�
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void RevertToPreviousState() {
			ChangeState(m_PreviousState);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�J�����g�X�e�[�g���w�肵���X�e�[�g�ɂȂ��Ă邩�ǂ������`�F�b�N
		@param[in]	st	���ׂ�X�e�[�g
		@return	�w�肵���X�e�[�g�ɂȂ��Ă��true
		*/
		//--------------------------------------------------------------------------------------
		bool IsInState(const shared_ptr< ObjState<T> >& st)const {
			if (st == GetCurrentState()) {
				return true;
			}
			return false;
		}
	};

	//�e�V���O���g���C���X�^���X�쐬�}�N��
#define DECLARE_SINGLETON_INSTANCE(InsType)	static shared_ptr<InsType> Instance();
#define IMPLEMENT_SINGLETON_INSTANCE(InsType)	shared_ptr<InsType> InsType::Instance() { \
	static shared_ptr<InsType> instance; \
	if(!instance) { instance = shared_ptr<InsType>(new InsType); }return instance;}


}
//end namespace basecross
/*!
@file Player.h
@brief �v���C���[�Ȃ�
*/

#pragma once
#include "stdafx.h"

namespace basecross {
#ifdef test
	//--------------------------------------------------------------------------------------
	///	�v���C���[
	//--------------------------------------------------------------------------------------
	class Player : public GameObject {
		//�v���C���[���g�p����R���g���[���ƃL�[�{�[�h�̓���
		Vec2 GetInputState() const;
		// �R���g���[����������x�N�g���𓾂�
		Vec3 GetMoveVector() const;
		//�v���C���[�̈ړ�
		void MovePlayer();
		//�X�s�[�h
		float m_Speed;
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�R���X�g���N�^
		@param[in]	stagePtr	�X�e�[�W
		*/
		//--------------------------------------------------------------------------------------
		Player(const shared_ptr<Stage>& stagePtr);
		virtual ~Player() {}
		virtual void OnInit() override;
		//�X�V
		virtual void OnUpdate() override;
	};

#endif

	//--------------------------------------------------------------------------------------
	///	�v���C���[
	//--------------------------------------------------------------------------------------
	class Player : public GameObject {
		//�v���C���[���g�p����R���g���[���ƃL�[�{�[�h�̓���
		Vec2 GetInputState() const;
		// �R���g���[����������x�N�g���𓾂�
		Vec3 GetMoveVector() const;
		//�v���C���[�̈ړ�
		void MovePlayer();
		//������̕\��
//		void DrawStrings();
		//���̓n���h���[
		InputHandler<Player> m_InputHandler;
		//�X�s�[�h
		float m_Speed;
	public:
		//�\�z�Ɣj��
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�R���X�g���N�^
		@param[in]	StagePtr	�X�e�[�W
		*/
		//--------------------------------------------------------------------------------------
		Player(const shared_ptr<Stage>& StagePtr);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�f�X�g���N�^
		*/
		//--------------------------------------------------------------------------------------
		virtual ~Player() {}
		//�A�N�Z�T
		//������
		virtual void OnInit() override;
		//�X�V
		virtual void OnUpdate() override;
		virtual void OnUpdate2() override;
		virtual void OnCollisionEnter(shared_ptr<GameObject>& Other) override;
		virtual void OnCollisionExit(shared_ptr<GameObject>& Other) override;
		//A�{�^��
		void OnPushA();
	};



}
//end namespace basecross
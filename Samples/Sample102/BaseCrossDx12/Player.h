/*!
@file Player.h
@brief �v���C���[
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	�v���C���[
	//--------------------------------------------------------------------------------------
	class Player : public GameObject {
		//�v���C���[���g�p����R���g���[���ƃL�[�{�[�h�̓���
		Vec2 GetInputState() const;
		// �R���g���[����������x�N�g���𓾂�
		Vec3 GetMoveVector();
		//�v���C���[�̈ړ�
		void MovePlayer();
		//���̓n���h���[
		InputHandler<Player> m_InputHandler;
		//�X�s�[�h
		float m_Speed;
		Vec3 m_angle;
	public:
		Player(const std::shared_ptr<Stage>& StagePtr, const TransParam& param);
		virtual ~Player() {}
		virtual void OnCreate()override;
		virtual void OnUpdate(double elapsedTime);
		//A�{�^��
		void OnPushA();
		//B�{�^��
		void OnPushB() {}
	};

}
//end namespace basecross

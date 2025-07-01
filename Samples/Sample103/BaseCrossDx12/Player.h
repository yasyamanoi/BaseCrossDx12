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
		//		std::shared_ptr<BaseMesh> m_mesh;
		//		std::shared_ptr < BaseTexture> m_texture;
				//�v���C���[���g�p����R���g���[���ƃL�[�{�[�h�̓���
		Vec2 GetInputState() const;
		// �R���g���[����������x�N�g���𓾂�
		Vec3 GetMoveVector() const;
		//�v���C���[�̈ړ�
		void MovePlayer();
		//���̓n���h���[
		InputHandler<Player> m_InputHandler;
		//�X�s�[�h
		float m_Speed;
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

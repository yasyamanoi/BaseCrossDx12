/*!
@file GameStage.h
@brief �Q�[���X�e�[�W�N���X
*/
#pragma once
#include "stdafx.h"

namespace basecross {

	class GameStage : public Stage {
		//�J����
		shared_ptr<MyCamera> m_camera;
		//���C�g
		shared_ptr<LightSet> m_lightSet;
		//�Œ�̃{�b�N�X�̍쐬
		void CreateFixedBox();
		//�Œ�̋��̂̍쐬
		void CreateFixedSphere();
		//�Œ�̃J�v�Z���̍쐬
		void CreateFixedCapsule();
		//�s�����藈���肷��{�b�N�X�̍쐬
		void CreateMoveFixedBox();
		//�ړ�����{�b�N�X�̍쐬
		void CreateMoveBox();
		//�ǂ�������I�u�W�F�N�g�̍쐬
		void CreateSeekObject();
		//�v���C���[�̍쐬
		void CreatePlayer();
	public:
		GameStage() :Stage() {}
		virtual ~GameStage() {}

		virtual shared_ptr<Camera> GetActiveCamera() const override {
			return m_camera;
		}
		virtual shared_ptr<LightSet> GetActiveLightSet() const override {
			return m_lightSet;
		}
		virtual void OnCreate()override;
		virtual void OnUpdateLightCamera() override {
			m_camera->OnUpdate();
			m_lightSet->OnUpdate();
		}

	};

}
//end namespace basecross
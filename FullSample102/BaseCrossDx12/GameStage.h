/*!
@file GameStage.h
@brief �Q�[���X�e�[�W�N���X
*/
#pragma once
#include "stdafx.h"

namespace basecross {

	class GameStage : public Stage {
		//�J����
		shared_ptr<BaseCamera> m_camera;
		//���C�g
		shared_ptr<LightSet> m_lightSet;
		//�����v�Z�I�u�W�F�N�g�̍쐬
		void CreatePhysicsObjects();
		//�v���C���[�̍쐬
		void CreatePlayer();

	public:
		GameStage() :Stage() {}
		virtual ~GameStage() {}
		virtual shared_ptr<BaseCamera> GetActiveCamera() const override {
			return m_camera;
		}
		virtual shared_ptr<LightSet> GetActiveLightSet() const override {
			return m_lightSet;
		}
		virtual void OnCreate()override;
		virtual void OnUpdateLightCamera() override {
			m_camera->OnUpdate();
		}

		//�X�V
		virtual void OnUpdate()override;


	};



}
//end namespace basecross
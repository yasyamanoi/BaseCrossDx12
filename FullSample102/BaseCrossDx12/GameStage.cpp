/*!
@file GameStage.cpp
@brief �Q�[���X�e�[�W�N���X
*/


#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//�����v�Z�I�u�W�F�N�g�̍쐬
	void GameStage::CreatePhysicsObjects() {
		Quat qt1, qt2, qt3, qt4;
		qt1.rotationZ(2.0f);
		qt2.rotationX(0.7f);
		qt3.rotationZ(-2.0f);
		qt4.rotationX(-0.7f);
		//���̑�
		AddGameObject<FixedPsBox>(Vec3(30.0f, 1.0f, 30.0f), Quat(), Vec3(0.0f, -0.5f, 0.0f));
		//�����Ȃ���
		AddGameObject<FixedPsBox>(Vec3(3.0f, 1.0f, 3.0f), Quat(), Vec3(0.0f, 0.5f, 3.0f));
		//�ォ��~���Ă���{�b�N�X
		AddGameObject<ActivePsBox>(Vec3(2.0f, 1.0f, 1.0f), Quat(), Vec3(2.0f, 1.0f, 4.0f));
		AddGameObject<ActivePsBox>(Vec3(2.0f, 1.0f, 1.0f), qt2, Vec3(1.0f, 3.0f, 3.0f));
		AddGameObject<ActivePsBox>(Vec3(1.0f, 1.0f, 1.0f), qt4, Vec3(1.0f, 11.0f, 8.0f));
		//�ォ��~���Ă��鋅��
		AddGameObject<ActivePsSphere>(1.0f, Quat(), Vec3(0.0f, 6.0f, 6.0f));

	}



	//�v���C���[�̍쐬
	void GameStage::CreatePlayer() {
		//�v���[���[�̍쐬
		auto ptrPlayer = AddGameObject<Player>();
		//�V�F�A�z��Ƀv���C���[��ǉ�
		SetSharedGameObject(L"Player", ptrPlayer);
		ptrPlayer->AddTag(L"Player");
	}



	void GameStage::OnCreate() {
		//�����v�Z�L��
		SetPhysicsActive(true);

		//�J�����ƃ��C�g�̐ݒ�
		m_camera = ObjectFactory::Create<MyCamera>();
		m_camera->SetEye(Vec3(0, 3.43f, -6.37f));
		m_camera->SetAt(Vec3(0, 0.125f, 0));

		m_lightSet = LightSet::CreateDefaultLights();
		//�����v�Z�I�u�W�F�N�g�̍쐬
		CreatePhysicsObjects();
		//�v���[���[�̍쐬
		CreatePlayer();
	}

	void GameStage::OnUpdate() {
		const float fullMax = 20.0f;
		const float halfMax = fullMax * 0.5f;
		if (Util::DivProbability(20)) {
			float x = Util::RandZeroToOne() * fullMax - halfMax;
			float z = Util::RandZeroToOne() * fullMax - halfMax;
			Vec3 velo(x * 0.5f, 0, z * 0.5f);
			AddGameObject<FallingBall>(Vec3(x, halfMax, z), velo);
		}
	}


}
//end namespace basecross

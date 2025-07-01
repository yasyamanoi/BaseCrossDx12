/*!
@file GameStage.cpp
@brief �Q�[���X�e�[�W�N���X�@����
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	// �Q�[���X�e�[�W
	//--------------------------------------------------------------------------------------



	void GameStage::OnCreate() {
		//�J�����ƃ��C�g�̐ݒ�
		m_camera = ObjectFactory::Create<MyCamera>();
		m_camera->SetEye(Vec3(0, 3.43f, -6.37f));
		m_camera->SetAt(Vec3(0, 0.125f, 0));
		m_lightSet = ObjectFactory::Create<LightSet>();
		TransParam param;
		auto quat = XMQuaternionIdentity();
		param.quaternion = Quat(quat);
		param.scale = Vec3(50.0f, 1.0f, 50.0f);
		param.position = Vec3(0.0f, -0.5, 0.0f);
		AddGameObject<FixedBox>(param);


		param.scale = Vec3(1.0f, 1.0f, 1.0f);
		param.rotOrigin = Vec3(0.0f, 0.0f, 0.0f);
		quat = Quat(Vec3(1, 1, 0), 1.0f);
		param.quaternion = quat;
		param.position = Vec3(-4.0f, 5.0f, 5.0f);
		AddGameObject<WallBox>(param);

		quat = Quat(Vec3(0, 1, 1), 2.0);
		param.quaternion = quat;
		param.position = Vec3(-2.0f, 5.0f, 5.0f);
		AddGameObject<WallBox>(param);

		quat = Quat(Vec3(1, 0, 1), 1.5);
		param.quaternion = quat;
		param.position = Vec3(0.0f, 5.0f, 5.0f);
		AddGameObject<WallBox>(param);

		quat = Quat(Vec3(1, 1, 1), 1.0);
		param.quaternion = quat;
		param.position = Vec3(2.0f, 5.0f, 5.0f);
		AddGameObject<WallBox>(param);

		quat = Quat(Vec3(1, 0, 1), 0.5);
		param.quaternion = quat;
		param.position = Vec3(4.0f, 5.0f, 5.0f);
		AddGameObject<WallBox>(param);


		param.scale = Vec3(0.25f, 0.25f, 0.25f);
		param.position = Vec3(0.0f, 0.125f, 0.0f);
		AddGameObject<Player>(param);
	}

	void GameStage::OnUpdate2(double elapsedTime) {
		auto pBaseScene = BaseScene::Get();
		// �V�~�����[�V�������x���w�肷��
		pBaseScene->GetPxScene()->simulate((float)elapsedTime);
		// PhysX�̏������I���܂ő҂�
		pBaseScene->GetPxScene()->fetchResults(true);

	}

}
// end namespace basecross

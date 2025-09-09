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
		param.scale = Vec3(1.0f, 1.0f, 1.0f);

		param.scale = Vec3(50.0f, 1.0f, 50.0f);
		param.position = Vec3(0.0f, -0.5, 0.0f);
		AddGameObject<FixedBox>(param);

		param.scale = Vec3(0.25f, 0.25f, 0.25f);
		param.position = Vec3(0.0f, 0.125f, 0.0f);
		AddGameObject<Player>(param);

		param = TransParam();
		param.scale = Vec3(0.05f);
//		param.quaternion = Quat();
		param.quaternion = Quat(Vec3(0, 1, 0), -XM_PI);
		//		param.quaternion = Quat(Vec3(1, 0, 0), -XM_PIDIV2);
		param.position = Vec3(0.0f, 2.0f, 0.0f);
		AddGameObject<FixedModel>(param);

	}

}
// end namespace basecross

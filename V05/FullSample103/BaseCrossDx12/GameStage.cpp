/*!
@file GameStage.cpp
@brief �Q�[���X�e�[�W�N���X
*/


#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//�Œ�̃{�b�N�X�̍쐬
	void GameStage::CreateFixedBox() {
		vector< vector<Vec3> > vec = {
			{
				Vec3(50.0f, 1.0f, 50.0f),
				Vec3(0.0f, 0.0f, 0.0f),
				Vec3(0.0f, -0.5f, 0.0f)
			},
			{
				Vec3(5.0f, 0.5f, 5.0f),
				Vec3(0.0f, 0.0f, 0.0f),
				Vec3(10.0f, 0.25f, 10.0f)
			},
			{
				Vec3(5.0f, 0.5f, 5.0f),
				Vec3(0.0f, 0.0f, 0.0f),
				Vec3(15.0f, 0.25f, 10.0f)
			},

			{
				Vec3(2.0f, 1.0f, 2.0f),
				Vec3(0, 0, 0),
				Vec3(10.0f, 0.5f, 10.0f)
			},

		};
		//�I�u�W�F�N�g�̍쐬
		for (auto v : vec) {
			AddGameObject<FixedBox>(v[0], v[1], v[2]);
		}
	}

	//�G�̍쐬
	void GameStage::CreateEnemy() {
		//�I�u�W�F�N�g�̃O���[�v���쐬����
		auto Group = CreateSharedObjectGroup(L"EnemyGroup");
		//�z��̏�����
		vector<Vec3> Vec = {
			{ 0, 0.25f, 10.0f },
			{ 10.0f, 0.25f, 0.0f },
			{ -10.0f, 0.25f, 0.0f },
			{ 0, 0.25f, -10.0f },
		};
		//�z�u�I�u�W�F�N�g�̍쐬
		for (auto v : Vec) {
			AddGameObject<Enemy1>(v);
		}

		//�z��̏�����
		vector<Vec3> Vec2 = {
			{ 15.0f, 0.25f, 0.0f },
			{ -15.0f, 0.25f, 0.0f },
			{ 0.0f, 0.25f, 15.0f },
			{ 0.0f, 0.25f, -15.0f },
		};
		//�z�u�I�u�W�F�N�g�̍쐬
		for (auto v : Vec2) {
			AddGameObject<Enemy2>(v);
		}

		//�z��̏�����
		vector<Vec3> Vec3 = {
			{ 10.0f, 0.25f, -10.0f },
		};
		//�z�u�I�u�W�F�N�g�̍쐬
		for (auto v : Vec3) {
			AddGameObject<Enemy3>(v);
		}



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
		//�J�����ƃ��C�g�̐ݒ�
		m_camera = ObjectFactory::Create<MyCamera>();
		m_camera->SetEye(Vec3(0, 3.43f, -6.37f));
		m_camera->SetAt(Vec3(0, 0.125f, 0));
		m_lightSet = ObjectFactory::Create<LightSet>();
		//�Œ�̃{�b�N�X�̍쐬
		CreateFixedBox();
		//�v���[���[�̍쐬
		CreatePlayer();
		//�G�̍쐬
		CreateEnemy();
	}

}
//end namespace basecross

/*!
@file GameStage.cpp
@brief �Q�[���X�e�[�W�N���X
*/


#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//�Œ�̃{�b�N�X�̍쐬
	void GameStage::CreateFixedBox() {
		//�z��̏�����
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
			{
				Vec3(4.0f, 1.0f, 4.0f),
				Vec3(0, 0, 0),
				Vec3(-10.0f, 0.5f, 10.0f)
			},
			{
				Vec3(10.0f, 0.5f, 10.0f),
				Vec3(-0.5f, 0.0f, -0.5f),
				Vec3(-10.0f, 0.25f, 10.0f)
			},
		};
		//�I�u�W�F�N�g�̍쐬
		for (auto v : vec) {
			AddGameObject<FixedBox>(v[0], v[1], v[2]);
		}
	}

	void GameStage::CreateFixedSphere() {
		struct SpStr {
			float Scale;
			Vec3 Rot;
			Vec3 Pos;
		};
		//�z��̏�����
		vector< SpStr > data = {
			{
				2.0f,
				Vec3(0.0f, 0.0f, 0.0f),
				Vec3(5.0f, 1.0f, 15.0f)
			},
		};
		//�I�u�W�F�N�g�̍쐬
		for (auto v : data) {
			AddGameObject<FixedSphere>(v.Scale, v.Rot, v.Pos);
		}

	}


	void GameStage::CreateFixedCapsule() {
		//�z��̏�����
		vector< vector<Vec3> > vec = {
			{
				Vec3(1.0f, 1.0f, 1.0f),
				Vec3(0.0f, 0.0f, 0.0f),
				Vec3(0.0f, 1.0f, 15.0f)
			},
		};
		//�I�u�W�F�N�g�̍쐬
		for (auto v : vec) {
			AddGameObject<FixedCapsule>(v[0], v[1], v[2]);
		}

	}

	//�s�����藈���肷��{�b�N�X�̍쐬
	void GameStage::CreateMoveFixedBox() {
		CreateSharedObjectGroup(L"MoveFixedBox");
		AddGameObject<MoveFixedBox>(
			Vec3(5.0f, 0.5f, 5.0f),
			Vec3(0.0f, 0.0f, 0.0f),
			Vec3(0.0f, -0.5f, 20.0f)
			);

	}



	//�ړ�����{�b�N�X�̍쐬
	void GameStage::CreateMoveBox() {
		//�z��̏�����
		vector< vector<Vec3> > vec = {
			{
				Vec3(1.0f, 1.0f, 1.0f),
				Vec3(0.0f, 0.0f, 0.0f),
				Vec3(5.0f, 1.0f, 5.0f)
			},
		};
		//�I�u�W�F�N�g�̍쐬
		for (auto v : vec) {
			AddGameObject<MoveBox>(v[0], v[1], v[2]);
		}

	}

	//�ǂ�������I�u�W�F�N�g�̍쐬
	void GameStage::CreateSeekObject() {
		//�I�u�W�F�N�g�̃O���[�v���쐬����
		auto group = CreateSharedObjectGroup(L"SeekGroup");
		//�z��̏�����
		vector<Vec3> vec = {
			{ 0, 0.125f, 10.0f },
			{ 10.0f, 0.125f, 0.0f },
			{ -10.0f, 0.125f, 0.0f },
			{ 0, 0.125f, -10.0f },
		};

		//�z�u�I�u�W�F�N�g�̍쐬
		for (size_t count = 0; count < vec.size(); count++) {
			AddGameObject<SeekObject>(vec[count]);
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



	void GameStage::OnInit() {
		//�J�����ƃ��C�g�̐ݒ�
		m_camera = ObjectFactory::Create<MyCamera>();
		m_camera->SetEye(Vec3(0, 3.43f, -6.37f));
		m_camera->SetAt(Vec3(0, 0.125f, 0));

		m_lightSet = LightSet::CreateDefaultLights();
		//�Œ�̃{�b�N�X�̍쐬
		CreateFixedBox();
		//�s�����藈���肷��{�b�N�X�̍쐬
		CreateMoveFixedBox();
		//�ړ�����{�b�N�X�̍쐬
		CreateMoveBox();
		//�Œ�̋��̂̍쐬
		CreateFixedSphere();
		//�Œ�̃J�v�Z���̍쐬
		CreateFixedCapsule();
		//�ǂ�������I�u�W�F�N�g�̍쐬
		CreateSeekObject();
		//�v���[���[�̍쐬
		CreatePlayer();
	}

}
//end namespace basecross

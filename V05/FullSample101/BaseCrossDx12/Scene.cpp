/*!
@file Scene.cpp
@brief �V�[���N���X
*/


#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void Scene::OnCreate() {
		//�e�N�X�`��
		auto texFile = App::GetRelativeAssetsDir() + L"wall.jpg";
		auto texture = BaseTexture::CreateTextureFlomFile(texFile);
		RegisterTexture(L"WALL_TX", texture);

		texFile = App::GetRelativeAssetsDir() + L"wall_normal.png";
		texture = BaseTexture::CreateTextureFlomFile(texFile);
		RegisterTexture(L"WALL_NORMAL_TX", texture);

		texFile = App::GetRelativeAssetsDir() + L"sky.jpg";
		texture = BaseTexture::CreateTextureFlomFile(texFile);
		RegisterTexture(L"SKY_TX", texture);

		texFile = App::GetRelativeAssetsDir() + L"wall_normal.png";
		texture = BaseTexture::CreateTextureFlomFile(texFile);
		RegisterTexture(L"SKY_NORMAL_TX", texture);


		texFile = App::GetRelativeAssetsDir() + L"trace.png";
		texture = BaseTexture::CreateTextureFlomFile(texFile);
		RegisterTexture(L"TRACE_TX", texture);

		texFile = App::GetRelativeAssetsDir() + L"number.png";
		texture = BaseTexture::CreateTextureFlomFile(texFile);
		RegisterTexture(L"NUMBER_TX", texture);
		//�ŏ��̃A�N�e�B�u�ȃX�e�[�W�̐ݒ�
		ResetActiveStage<GameStage>();

	}

	void Scene::OnEvent(const shared_ptr<SceneEvent>& event) {
		//�X�e�[�W�̐؂�ւ��Ȃǂ̎��ɋL�q����
	}


}
// end namespace basecross
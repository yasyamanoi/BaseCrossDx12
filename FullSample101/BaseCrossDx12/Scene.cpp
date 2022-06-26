/*!
@file Scene.cpp
@brief シーンクラス
*/


#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void Scene::OnCreate() {
		//テクスチャ
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
		//最初のアクティブなステージの設定
		ResetActiveStage<GameStage>();

	}

	void Scene::OnEvent(const shared_ptr<SceneEvent>& event) {
		//ステージの切り替えなどの時に記述する
	}


}
// end namespace basecross

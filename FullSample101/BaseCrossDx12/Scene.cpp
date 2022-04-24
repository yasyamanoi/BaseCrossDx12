/*!
@file Scene.cpp
@brief シーンクラス
*/


#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void Scene::OnInit() {
		//テクスチャ
		auto texFile = App::GetRelativeAssetsDir() + L"wall.jpg";
		auto texture = BaseTexture::CreateTextureFlomFile(texFile);
		RegisterTexture(L"WALL_TX", texture);
		texFile = App::GetRelativeAssetsDir() + L"sky.jpg";
		texture = BaseTexture::CreateTextureFlomFile(texFile);
		RegisterTexture(L"SKY_TX", texture);

		texFile = App::GetRelativeAssetsDir() + L"trace.png";
		texture = BaseTexture::CreateTextureFlomFile(texFile);
		RegisterTexture(L"TRACE_TX", texture);

		texFile = App::GetRelativeAssetsDir() + L"number.png";
		texture = BaseTexture::CreateTextureFlomFile(texFile);
		RegisterTexture(L"NUMBER_TX", texture);

		ResetActiveStage<GameStage>();
	}

	void Scene::OnEvent(const shared_ptr<SceneEvent>& event) {
		if (event->m_msgStr == L"ToGameStage") {
			//最初のアクティブステージの設定
			ResetActiveStage<GameStage>();
		}
	}

}
// end namespace basecross

/*!
@file Scene.cpp
@brief シーンクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void Scene::OnInit() {
		//テクスチャ
		auto texFile = App::GetRelativeAssetsDir() + L"wall.jpg";
		auto texture = CreateTextureFlomFile(texFile);
		AddTexture(L"WALL_TEX", texture);
		texFile = App::GetRelativeAssetsDir() + L"sky.jpg";
		texture = CreateTextureFlomFile(texFile);
		AddTexture(L"SKY_TEX", texture);

		texFile = App::GetRelativeAssetsDir() + L"trace.png";
		texture = CreateTextureFlomFile(texFile);
		AddTexture(L"TRACE_TEX", texture);

		texFile = App::GetRelativeAssetsDir() + L"number.png";
		texture = CreateTextureFlomFile(texFile);
		AddTexture(L"NUMBER_TEX", texture);

		ResetActiveStage<GameStage>();
	}

}
// end namespace basecross

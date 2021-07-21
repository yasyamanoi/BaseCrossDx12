/*!
@file Scene.cpp
@brief シーンクラス
@copyright Copyright (c) 2021 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void Scene::OnInit() {
		//親クラスの初期化
		BaseScene::OnInit();
		ResetActiveStage<GameStage>();
	}




}
// end basecross
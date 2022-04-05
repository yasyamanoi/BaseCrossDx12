/*!
@file GameStage.cpp
@brief ゲームステージクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//固定のボックスの作成
	void GameStage::CreateFixedBox() {
		//配列の初期化
		vector< vector<Vec3> > vec = {
			{
				Vec3(50.0f, 1.0f, 50.0f),
				Vec3(0.0f, 0.0f, 0.0f),
				Vec3(0.0f, -0.5f, 0.0f)
			},
		};
		//オブジェクトの作成
		for (auto v : vec) {
			AddGameObject<FixedBox>(v[0], v[1], v[2]);
		}
	}


	void GameStage::OnInit() {
		//カメラとライトの設定
		m_camera = BaseCamera::CreateCamera<BaseCamera>(Vec3(0, 7.0f, -7.0f), Vec3(0, 0, 0));
		m_lightSet = LightSet::CreateDefaultLights();
		//配置するオブジェクトの初期化
		CreateFixedBox();
		AddGameObject<WallObject>();
		AddGameObject<PcSphere>(Vec3(-7.0f, 0.5f, 5.0f));
		AddGameObject<PnSphere>(Vec3(-5.0f, 0.5f, 5.0f));
		AddGameObject<PtSphere>(Vec3(-3.0f, 0.5f, 5.0f));
		AddGameObject<PctSphere>(Vec3(-1.0f, 0.5f, 5.0f));
		AddGameObject<PntSphere>(Vec3(1.0f, 0.5f, 5.0f), true);

		AddGameObject<WallSprite>(L"WALL_TEX", false,
			Vec2(320.0f, 320.0f), Vec3(400.0f, 0.0f, 0.1f));
		AddGameObject<ScrollSprite>(L"TRACE_TEX", true,
			Vec2(240.0f, 60.0f), Vec3(400.0f, 0.0f, 0.0f));
		AddGameObject<ScoreSprite>(4,
			L"NUMBER_TEX",
			true,
			Vec2(320.0f, 80.0f),
			Vec3(0.0f, 0.0f, 0.0f));

	}

	void GameStage::OnUpdate() {
		float elapsedTime = App::GetElapsedTime();
		m_totalTime += elapsedTime;
		if (m_totalTime >= 10000.0f) {
			m_totalTime = 0.0f;
		}
		//スコアを更新する
		auto ptrScor = GetSharedGameObject<ScoreSprite>(L"ScoreSprite");
		ptrScor->SetScore(m_totalTime);
	}


}
//end namespace basecross


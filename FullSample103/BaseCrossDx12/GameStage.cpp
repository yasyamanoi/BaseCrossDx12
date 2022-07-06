/*!
@file GameStage.cpp
@brief ゲームステージクラス
*/


#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//固定のボックスの作成
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
		//オブジェクトの作成
		for (auto v : vec) {
			AddGameObject<FixedBox>(v[0], v[1], v[2]);
		}
	}

	//敵の作成
	void GameStage::CreateEnemy() {
		//オブジェクトのグループを作成する
		auto Group = CreateSharedObjectGroup(L"EnemyGroup");
		//配列の初期化
		vector<Vec3> Vec = {
			{ 0, 0.25f, 10.0f },
			{ 10.0f, 0.25f, 0.0f },
			{ -10.0f, 0.25f, 0.0f },
			{ 0, 0.25f, -10.0f },
		};
		//配置オブジェクトの作成
		for (auto v : Vec) {
			AddGameObject<Enemy1>(v);
		}

		//配列の初期化
		vector<Vec3> Vec2 = {
			{ 15.0f, 0.25f, 0.0f },
			{ -15.0f, 0.25f, 0.0f },
			{ 0.0f, 0.25f, 15.0f },
			{ 0.0f, 0.25f, -15.0f },
		};
		//配置オブジェクトの作成
		for (auto v : Vec2) {
			AddGameObject<Enemy2>(v);
		}

		//配列の初期化
		vector<Vec3> Vec3 = {
			{ 10.0f, 0.25f, -10.0f },
		};
		//配置オブジェクトの作成
		for (auto v : Vec3) {
			AddGameObject<Enemy3>(v);
		}



	}


	//プレイヤーの作成
	void GameStage::CreatePlayer() {
		//プレーヤーの作成
		auto ptrPlayer = AddGameObject<Player>();
		//シェア配列にプレイヤーを追加
		SetSharedGameObject(L"Player", ptrPlayer);
		ptrPlayer->AddTag(L"Player");
	}



	void GameStage::OnCreate() {
		//カメラとライトの設定
		m_camera = ObjectFactory::Create<MyCamera>();
		m_camera->SetEye(Vec3(0, 3.43f, -6.37f));
		m_camera->SetAt(Vec3(0, 0.125f, 0));
		m_lightSet = ObjectFactory::Create<LightSet>();
		//固定のボックスの作成
		CreateFixedBox();
		//プレーヤーの作成
		CreatePlayer();
		//敵の作成
		CreateEnemy();
	}

}
//end namespace basecross


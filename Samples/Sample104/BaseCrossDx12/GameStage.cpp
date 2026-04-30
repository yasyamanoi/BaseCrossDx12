/*!
@file GameStage.cpp
@brief ゲームステージクラス　実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	// タイトルステージ
	//--------------------------------------------------------------------------------------
	void TitleStage::OnCreate() {
		//カメラとライトの設定（2次元しか使わなくても必要）
		m_camera = ObjectFactory::Create<PerspecCamera>();
		m_camera->SetEye(Vec3(0, -10, 0));
		m_camera->SetAt(Vec3(0, 0, 0.0f));
		m_camera->SetUp(Vec3(0, 1, 0));
		m_lightSet = ObjectFactory::Create<LightSet>();
		//スプライトオブジェクトの作成
		CreateSprite();
	}

	//スプライトの作成
	void TitleStage::CreateSprite() {
		AddGameObject<TitleSprite>();
	}

	void TitleStage::OnUpdate(double elapsedTime) {
		//コントローラチェックして入力があればコマンド呼び出し
		m_InputHandler.PushHandle(GetThis<TitleStage>());
	}


	void TitleStage::OnPushA() {
	}

	void TitleStage::OnPushB() {
		//ステージ推移
		Scene::Get()->ResetActiveStage<GameStage>(m_pDevice);
	}


	//--------------------------------------------------------------------------------------
	// ゲームステージ
	//--------------------------------------------------------------------------------------

	//ボックスオブジェクトの作成
	void GameStage::CreateFixedBox() {
		TransParam param;
		param.scale = Vec3(50.0f, 1.0f, 50.0f);
		param.position = Vec3(0.0f, -0.5, 0.0f);
		AddGameObject<FixedBox>(param);


	}

	void GameStage::OnCreate() {
		//カメラとライトの設定
		m_camera = ObjectFactory::Create<MyCamera>();
		m_camera->SetEye(Vec3(0, 3.43f, -6.37f));
		m_camera->SetAt(Vec3(0, 0.125f, 0));
		m_lightSet = ObjectFactory::Create<LightSet>();
		//ボックスオブジェクトの作成
		CreateFixedBox();
		//プレイヤーの作成
		CreatePlayer();
	}


	//プレイヤーの作成
	void GameStage::CreatePlayer() {
		TransParam param;
		param.scale = Vec3(0.25f, 0.25f, 0.25f);
		param.quaternion = Quat();
		param.position = Vec3(0.0f, 0.125f, 0.0f);
		AddGameObject<Player>(param);
	}




}
// end namespace basecross

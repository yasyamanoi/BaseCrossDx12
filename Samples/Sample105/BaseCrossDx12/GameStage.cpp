/*!
@file GameStage.cpp
@brief ゲームステージクラス　実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {


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

		auto XAPtr = Scene::Get()->GetXAudio2Manager();
		m_BGM = XAPtr->Start(L"Nanika", XAUDIO2_LOOP_INFINITE, 0.1f);

	}


	//プレイヤーの作成
	void GameStage::CreatePlayer() {
		TransParam param;
		param.scale = Vec3(0.25f, 0.25f, 0.25f);
		param.quaternion = Quat();
		param.position = Vec3(0.0f, 0.125f, 0.0f);
		AddGameObject<Player>(param);
	}

	void GameStage::OnDestroy() {
		//BGMのストップ
		auto XAPtr = Scene::Get()->GetXAudio2Manager();
		XAPtr->Stop(m_BGM);
	}





}
// end namespace basecross

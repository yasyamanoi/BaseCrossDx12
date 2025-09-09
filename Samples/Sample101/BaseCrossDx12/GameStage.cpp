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

	//追いかけるオブジェクトの作成
	void GameStage::CreateSeekObject() {
		//オブジェクトのグループを作成する
		auto group = CreateSharedObjectGroup(L"SeekGroup");
		//配列の初期化
		//配列の初期化
		std::vector<Vec3> vec = {
			{ 0, 0.125f, 10.0f },
			{ 10.0f, 0.125f, 0.0f },
			{ -10.0f, 0.125f, 0.0f },
			{ 0, 0.125f, -10.0f },
		};

		//配置オブジェクトの作成
		for (size_t count = 0; count < vec.size(); count++) {
			AddGameObject<SeekObject>(vec[count]);
		}

	}


	void GameStage::OnCreate() {
		//カメラとライトの設定
		m_camera = ObjectFactory::Create<MyCamera>();
		m_camera->SetEye(Vec3(0, 3.43f, -6.37f));
		m_camera->SetAt(Vec3(0, 0.125f, 0));
		m_lightSet = ObjectFactory::Create<LightSet>();
		TransParam param;
		param.scale = Vec3(1.0f, 1.0f, 1.0f);
	//	param.rotOrigin = Vec3(0.0f, 0.0f, 0.0f);
		auto quat = XMQuaternionIdentity();
		param.quaternion = Quat(quat);
		param.position = Vec3(5.0f, 2.0f, 5.0f);
		AddGameObject<WallBox>(param);
		param.scale = Vec3(50.0f, 1.0f, 50.0f);
		param.position = Vec3(0.0f, -0.5, 0.0f);
		AddGameObject<FixedBox>(param);

		param.scale = Vec3(5.0f, 1.0f, 5.0f);
		param.position = Vec3(10.0f, 0.0, 10.0f);
		AddGameObject<FixedBox>(param);

		param.position = Vec3(10.0f, 0.0, 10.0f);
		param.quaternion = Quat(Vec3(-1, 0, 1), XM_PIDIV4);
		AddGameObject<FixedBox>(param);

		param.position = Vec3(-10.0f, 0.0, 10.0f);
		param.quaternion = Quat(Vec3(0, 1, 1), XM_PIDIV4);
		AddGameObject<FixedBox>(param);


		CreateSeekObject();
		param.scale = Vec3(0.25f, 0.25f, 0.25f);
		param.quaternion = Quat();
		param.position = Vec3(0.0f, 0.125f, 0.0f);
		AddGameObject<Player>(param);
	}


}
// end namespace basecross

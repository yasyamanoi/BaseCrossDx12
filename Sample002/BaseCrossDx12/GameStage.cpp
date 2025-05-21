/*!
@file GameStage.cpp
@brief ゲームステージクラス　実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	using namespace std;
	using namespace bsm;


	void GameStage::OnCreate() {
		m_myCamera = shared_ptr<PerspecCamera>(new PerspecCamera());
		m_myLightSet = shared_ptr<LightSet>(new LightSet());
		TransParam param;
		param.scale = Vec3(1.0f, 1.0f, 1.0f);
		param.rotOrigin = Vec3(0.0f, 0.0f, 0.0f);
		auto quat = XMQuaternionIdentity();
		param.quaternion = bsm::Quat(quat);
		param.position = Vec3(0.0f, 5.0f, 0.0f);
		AddGameObject<WallBox>(param);
		param.scale = Vec3(50.0f, 1.0f, 50.0f);
		param.position = Vec3(0.0f, -0.5, 0.0f);
		AddGameObject<SkyGround>(param);
	}
}
// end namespace basecross

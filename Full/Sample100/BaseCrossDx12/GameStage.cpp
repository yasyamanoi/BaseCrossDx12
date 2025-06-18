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


	void GameStage::OnCreate() {
		//カメラとライトの設定
		m_camera = ObjectFactory::Create<PerspecCamera>();
		m_camera->SetEye(Vec3(0,5.0f, -10.0f));
		m_camera->SetAt(Vec3(0, 0, 0));
		m_lightSet = ObjectFactory::Create<LightSet>();
	}


}
// end namespace basecross

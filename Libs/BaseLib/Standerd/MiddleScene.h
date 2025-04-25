/*!
@file MiddleScene.h
@brief 中間のシーンクラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#pragma once
#include "stdafx.h"


namespace basecross {

	using namespace SceneEnums;

	//--------------------------------------------------------------------------------------
	// 中間のシーン
	//--------------------------------------------------------------------------------------
	class MiddleScene : public BaseScene {
	protected:
		std::shared_ptr<Stage> m_activeStage;
		MiddleScene(UINT frameCount, PrimDevice* pSample);
		virtual ~MiddleScene();


	};

}
// end namespace basecross

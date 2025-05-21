/*!
@file Scene.h
@brief シーンクラス
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	// シーン
	//--------------------------------------------------------------------------------------
	class Scene :public BaseScene {
	public:
		Scene() : BaseScene() {}
		virtual ~Scene() {}
		virtual void OnCreate() override;
		virtual void OnEvent(const shared_ptr<SceneEvent>& event) override;
	};
}
//end basecross



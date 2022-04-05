/*!
@file Scene.h
@brief シーンクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
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
		virtual void OnInit() override;
	};
}
//end basecross



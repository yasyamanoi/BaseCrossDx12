/*!
@file Scene.h
@brief �V�[���N���X
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	// �V�[��
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


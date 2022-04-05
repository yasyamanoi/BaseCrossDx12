/*!
@file Component.cpp
@brief コンポーネント親
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"

namespace basecross {

	Component::Component(const shared_ptr<GameObject>& gameObjectPtr) :
		m_gameObject(gameObjectPtr), 
		m_updateActive(true), 
		m_renderActive(false),
		m_samplerKey(L"LinearClamp"),
		m_frameParamIndex(0),
		m_baseMesh(nullptr),
		m_baseTexture(nullptr)
	{}


	shared_ptr<Stage> Component::GetStage() const {
		return GetGameObject()->GetStage();
	}

	void Component::AttachGameObject(const shared_ptr<GameObject>& GameObjectPtr) {
		m_gameObject = GameObjectPtr;
	}

	void Component::OnRender() {
		auto pDefaultDev = App::GetBaseDevice();
		auto pFrame = pDefaultDev->GetCurrentBaseFrame();
		PopulateCommandList(pFrame);
	}



}
//end basecross

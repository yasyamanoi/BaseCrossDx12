/*!
@file Component.cpp
@brief コンポーネント親クラス　実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#pragma once
#include "stdafx.h"


namespace basecross {


	//--------------------------------------------------------------------------------------
	// コンポーネントの親
	//--------------------------------------------------------------------------------------
	Component::Component(const std::shared_ptr<GameObject>& gameObjectPtr):
		m_gameObject(gameObjectPtr)
	{}
	Component::~Component() {}
	void Component::OnCreate(ID3D12GraphicsCommandList* pCommandList){}
	void Component::OnUpdate(double elapsedTime){}

}
// end namespace basecross

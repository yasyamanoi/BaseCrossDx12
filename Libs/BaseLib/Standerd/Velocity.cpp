/*!
@file Velocity.cpp
@brief 速度コンポーネント
@copyright Copyright (c) 2021 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"

namespace basecross {

	Velocity::Velocity(const shared_ptr<GameObject>& gameObjectPtr, const bsm::Vec3& gravity) :
		Component(gameObjectPtr),
		m_liniaFource(0.0f),
		m_liniaVelocity(0.0f),
		m_quatVelocity(),
		m_gravity(gravity)
	{}

	Velocity::~Velocity() {}

	void Velocity::OnUpdate() {
		auto objPtr = GetGameObject();
		auto transPtr = objPtr->GetComponent<Transform>();

	}



}
// end basecross

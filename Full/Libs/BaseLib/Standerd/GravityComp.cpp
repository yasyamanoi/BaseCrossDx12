/*!
@file GravityComp.cpp
@brief 重力コンポーネント 実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#pragma once
#include "stdafx.h"
#include "Project.h"

namespace basecross {
	

	//--------------------------------------------------------------------------------------
	///	 重力コンポーネント
	//--------------------------------------------------------------------------------------
	GravityComp::GravityComp(const std::shared_ptr<GameObject>& GameObjectPtr, const bsm::Vec3& gravity) :
		Component(GameObjectPtr),
		m_Gravity(gravity),
		m_GravityVelocity(0)
	{
	}

	GravityComp::~GravityComp() {}

	bsm::Vec3 GravityComp::GetGravity() const {
		return m_Gravity;
	}
	void GravityComp::SetGravity(const bsm::Vec3& gravity) {
		m_Gravity = gravity;
	}
	void GravityComp::SetGravityZero() {
		m_Gravity = bsm::Vec3(0);
	}
	bsm::Vec3 GravityComp::GetGravityVelocity() const {
		return m_GravityVelocity;
	}
	void GravityComp::SetGravityVerocity(const bsm::Vec3& GravityVerocity) {
		m_GravityVelocity = GravityVerocity;
	}
	void GravityComp::SetGravityVerocityZero() {
		m_GravityVelocity = bsm::Vec3(0);
	}

	void GravityComp::StartJump(const bsm::Vec3& JumpVerocity) {
		m_GravityVelocity = JumpVerocity;
	}


	void GravityComp::OnUpdate(double elapsedTime) {
		//コリジョンがあって、スリープ状態なら更新しない
/*
		auto PtrCollision = GetGameObject()->GetComponent<Collision>(false);
		if (PtrCollision && PtrCollision->IsSleep()) {
			return;
		}
*/
		auto ptrTransform = GetGameObject()->GetComponent<Transform>();
		m_GravityVelocity += m_Gravity * (float)elapsedTime;
		auto Pos = ptrTransform->GetPosition();
		//		auto Pos = PtrTransform->GetWorldPosition();
		Pos += m_GravityVelocity * (float)elapsedTime;
		ptrTransform->SetPosition(Pos);
	}


}
// end namespace basecross

/*!
@file Gravity.h
@brief 重力コンポーネント
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	 重力コンポーネント
	//--------------------------------------------------------------------------------------
	Gravity::Gravity(const shared_ptr<GameObject>& GameObjectPtr, const bsm::Vec3& gravity) :
		Component(GameObjectPtr),
		m_Gravity(0),
		m_GravityVelocity(0)
	{
		m_Gravity = gravity;
	}

	Gravity::~Gravity() {}

	bsm::Vec3 Gravity::GetGravity() const {
		return m_Gravity;
	}
	void Gravity::SetGravity(const bsm::Vec3& gravity) {
		m_Gravity = gravity;
	}
	void Gravity::SetGravityZero() {
		m_Gravity = bsm::Vec3(0);
	}
	bsm::Vec3 Gravity::GetGravityVelocity() const {
		return m_GravityVelocity;
	}
	void Gravity::SetGravityVerocity(const bsm::Vec3& GravityVerocity) {
		m_GravityVelocity = GravityVerocity;
	}
	void Gravity::SetGravityVerocityZero() {
		m_GravityVelocity = bsm::Vec3(0);
	}

	void Gravity::StartJump(const bsm::Vec3& JumpVerocity) {
		m_GravityVelocity = JumpVerocity;
	}


	void Gravity::OnUpdate() {
		//コリジョンがあって、スリープ状態なら更新しない
		auto PtrCollision = GetGameObject()->GetComponent<Collision>(false);
		if (PtrCollision && PtrCollision->IsSleep()) {
			return;
		}
		auto PtrTransform = GetGameObject()->GetComponent<Transform>();
		//前回のターンからの時間
		float ElapsedTime = App::GetElapsedTime();
		m_GravityVelocity += m_Gravity * ElapsedTime;
		auto Pos = PtrTransform->GetPosition();
		//		auto Pos = PtrTransform->GetWorldPosition();
		Pos += m_GravityVelocity * ElapsedTime;
		PtrTransform->SetPosition(Pos);
	}


}
// end namespace basecross

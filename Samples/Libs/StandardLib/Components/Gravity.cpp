/*!
@file Gravity.cpp
@brief 重力コンポーネント　実体
*/
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	 重力コンポーネント
	//--------------------------------------------------------------------------------------
	Gravity::Gravity(const std::shared_ptr<GameObject>& GameObjectPtr, const Vec3& gravity) :
		Component(GameObjectPtr),
		m_Gravity(0),
		m_GravityVelocity(0)
	{
		m_Gravity = gravity;
	}

	Gravity::~Gravity() {}

	Vec3 Gravity::GetGravity() const {
		return m_Gravity;
	}
	void Gravity::SetGravity(const Vec3& gravity) {
		m_Gravity = gravity;
	}
	void Gravity::SetGravityZero() {
		m_Gravity = Vec3(0);
	}
	Vec3 Gravity::GetGravityVelocity() const {
		return m_GravityVelocity;
	}
	void Gravity::SetGravityVerocity(const Vec3& GravityVerocity) {
		m_GravityVelocity = GravityVerocity;
	}
	void Gravity::SetGravityVerocityZero() {
		m_GravityVelocity = Vec3(0);
	}

	void Gravity::StartJump(const Vec3& JumpVerocity) {
		m_GravityVelocity = JumpVerocity;
	}


	void Gravity::OnUpdate(double elapsedTime) {
		//コリジョンがあって、スリープ状態なら更新しない
		//auto PtrCollision = GetGameObject()->GetComponent<Collision>(false);
		//if (PtrCollision && PtrCollision->IsSleep()) {
		//	return;
		//}
		auto PtrTransform = GetGameObject()->GetComponent<Transform>();
		//前回のターンからの時間
		float ElapsedTime = (float)Scene::GetElapsedTime();
		m_GravityVelocity += m_Gravity * ElapsedTime;
		auto Pos = PtrTransform->GetPosition();
		//		auto Pos = PtrTransform->GetWorldPosition();
		Pos += m_GravityVelocity * ElapsedTime;
		PtrTransform->SetPosition(Pos);
	}


}
// end namespace basecross

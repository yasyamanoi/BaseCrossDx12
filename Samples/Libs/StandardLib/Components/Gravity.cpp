/*!
@file Gravity.cpp
@brief �d�̓R���|�[�l���g�@����
*/
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	 �d�̓R���|�[�l���g
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
		//�R���W�����������āA�X���[�v��ԂȂ�X�V���Ȃ�
		//auto PtrCollision = GetGameObject()->GetComponent<Collision>(false);
		//if (PtrCollision && PtrCollision->IsSleep()) {
		//	return;
		//}
		auto PtrTransform = GetGameObject()->GetComponent<Transform>();
		//�O��̃^�[������̎���
		float ElapsedTime = (float)Scene::GetElapsedTime();
		m_GravityVelocity += m_Gravity * ElapsedTime;
		auto Pos = PtrTransform->GetPosition();
		//		auto Pos = PtrTransform->GetWorldPosition();
		Pos += m_GravityVelocity * ElapsedTime;
		PtrTransform->SetPosition(Pos);
	}


}
// end namespace basecross

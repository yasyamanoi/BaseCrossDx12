/*!
@file Gravity.h
@brief �d�̓R���|�[�l���g
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	struct Gravity::Impl;
	//	�p�r: �R���|�[�l���gImpl�N���X
	//--------------------------------------------------------------------------------------
	struct Gravity::Impl {
		bsm::Vec3 m_Gravity;
		bsm::Vec3 m_GravityVelocity;
		Impl() :
			m_Gravity(0),
			m_GravityVelocity(0)
		{}
		~Impl() {}
	};


	//--------------------------------------------------------------------------------------
	///	 �d�̓R���|�[�l���g
	//--------------------------------------------------------------------------------------
	Gravity::Gravity(const shared_ptr<GameObject>& GameObjectPtr, const bsm::Vec3& gravity) :
		Component(GameObjectPtr),
		pImpl(new Impl())
	{
		pImpl->m_Gravity = gravity;
	}

	Gravity::~Gravity() {}

	bsm::Vec3 Gravity::GetGravity() const {
		return pImpl->m_Gravity;
	}
	void Gravity::SetGravity(const bsm::Vec3& gravity) {
		pImpl->m_Gravity = gravity;
	}
	void Gravity::SetGravityZero() {
		pImpl->m_Gravity = bsm::Vec3(0);
	}
	bsm::Vec3 Gravity::GetGravityVelocity() const {
		return pImpl->m_GravityVelocity;
	}
	void Gravity::SetGravityVerocity(const bsm::Vec3& GravityVerocity) {
		pImpl->m_GravityVelocity = GravityVerocity;
	}
	void Gravity::SetGravityVerocityZero() {
		pImpl->m_GravityVelocity = bsm::Vec3(0);
	}

	void Gravity::StartJump(const bsm::Vec3& JumpVerocity) {
		pImpl->m_GravityVelocity = JumpVerocity;
	}


	void Gravity::OnUpdate() {
		//�R���W�����������āA�X���[�v��ԂȂ�X�V���Ȃ�
		auto PtrCollision = GetGameObject()->GetComponent<Collision>(false);
		if (PtrCollision && PtrCollision->IsSleep()) {
			return;
		}
		auto PtrTransform = GetGameObject()->GetComponent<Transform>();
		//�O��̃^�[������̎���
		float ElapsedTime = App::GetElapsedTime();
		pImpl->m_GravityVelocity += pImpl->m_Gravity * ElapsedTime;
		auto Pos = PtrTransform->GetPosition();
		//		auto Pos = PtrTransform->GetWorldPosition();
		Pos += pImpl->m_GravityVelocity * ElapsedTime;
		PtrTransform->SetPosition(Pos);
	}


}
// end namespace basecross
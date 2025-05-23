/*!
@file GravityComp.h
@brief 重力コンポーネント
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	 重力コンポーネント
	//--------------------------------------------------------------------------------------
	class GravityComp : public Component {
		bsm::Vec3 m_Gravity;
		bsm::Vec3 m_GravityVelocity;
	public:
		explicit GravityComp(const std::shared_ptr<GameObject>& GameObjectPtr,
			const bsm::Vec3& gravity = bsm::Vec3(0, -9.8f, 0));
		virtual ~GravityComp();
		bsm::Vec3 GetGravity() const;
		void SetGravity(const bsm::Vec3& gravity);
		void SetGravityZero();
		bsm::Vec3 GetGravityVelocity() const;
		void SetGravityVerocity(const bsm::Vec3& GravityVerocity);
		void SetGravityVerocityZero();
		void StartJump(const bsm::Vec3& JumpVerocity);
		virtual void OnUpdate(double elapsedTime)override;
		virtual void OnCreate()override {}
		virtual void OnDestroy()override {}
	};

}
// end namespace basecross

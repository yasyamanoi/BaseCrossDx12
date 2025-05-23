/*!
@file GravityComp.h
@brief 重力コンポーネント
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	 重力コンポーネント
	//--------------------------------------------------------------------------------------
	class GravityComp : public Component {
		Vec3 m_Gravity;
		Vec3 m_GravityVelocity;
	public:
		explicit GravityComp(const std::shared_ptr<GameObject>& GameObjectPtr,
			const Vec3& gravity = Vec3(0, -9.8f, 0));
		virtual ~GravityComp();
		Vec3 GetGravity() const;
		void SetGravity(const Vec3& gravity);
		void SetGravityZero();
		Vec3 GetGravityVelocity() const;
		void SetGravityVerocity(const Vec3& GravityVerocity);
		void SetGravityVerocityZero();
		void StartJump(const Vec3& JumpVerocity);
		virtual void OnUpdate(double elapsedTime)override;
		virtual void OnCreate()override {}
		virtual void OnDestroy()override {}
	};

}
// end namespace basecross

/*!
@file SteeringBehavior.cpp
@brief ステアリング行動コンポーネント　実体
*/
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	 ステアリング行動コンポーネント（親）
	//--------------------------------------------------------------------------------------
	SteeringBehavior::SteeringBehavior(const std::shared_ptr<GameObject>& GameObjectPtr) :
		Component(GameObjectPtr),
		m_force(0.0f),
		m_mass(1.0),
		m_max_force(30.0),
		m_max_speed(10.0),
		m_weight(1.0)
	{
	}

	SteeringBehavior::~SteeringBehavior() {}


	void SteeringBehavior::OnUpdate(double elapsedTime) {
	}

	bool SteeringBehavior::AccumulateForce(Vec3& RunningTot,Vec3& ForceToAdd) {
		double MagnitudeSoFar = RunningTot.length();
		double MagnitudeRemaining = m_max_force - MagnitudeSoFar;
		if (MagnitudeRemaining <= 0.0) return false;
		double MagnitudeToAdd = ForceToAdd.length();
		if (MagnitudeToAdd < MagnitudeRemaining)
		{
			RunningTot += ForceToAdd;
		}
		else
		{
			ForceToAdd.normalize();
			RunningTot += (bsmUtil::normalize(ForceToAdd) * (float)MagnitudeRemaining);
		}

		return true;

	}

	void SteeringBehavior::ApplyForce() {

	}



	//--------------------------------------------------------------------------------------
	///	 Seek
	//--------------------------------------------------------------------------------------
	Seek::Seek(const std::shared_ptr<GameObject>& GameObjectPtr):
		SteeringBehavior(GameObjectPtr)
	{}

	Vec3 Seek::Execute(const Vec3& velocity, const Vec3& target, const Vec3& pos) {
		Vec3 DesiredVelocity
			= bsmUtil::normalize(target - pos) * (float)m_max_speed;
		return (DesiredVelocity - velocity);
	}

	//--------------------------------------------------------------------------------------
	///	 Arrive
	//--------------------------------------------------------------------------------------
	Arrive::Arrive(const std::shared_ptr<GameObject>& GameObjectPtr) :
		SteeringBehavior(GameObjectPtr),
		m_Decl(3.0f)
	{
	}

	Vec3 Arrive::Execute(const Vec3& Force, const Vec3& Velocity, const Vec3& Target, const Vec3& Pos) {
		Vec3 ToTarget = Target - Pos;
		Vec3 TempForce = Force;
		Vec3 WorkForce;
		float dist = bsmUtil::length(ToTarget);
		if (dist > 0) {
			const float DecelerationTweaker = 0.3f;
			float speed = dist / (m_Decl * DecelerationTweaker);
			speed = Util::Minimum(speed, (float)m_max_speed);
			Vec3 DesiredVelocity = ToTarget * speed / dist;
			WorkForce = DesiredVelocity - Velocity;
			AccumulateForce(TempForce, WorkForce);
			return TempForce;
		}
		return Vec3(0, 0, 0);
	}




	//--------------------------------------------------------------------------------------
	///	 Separation
	//--------------------------------------------------------------------------------------
	Separation::Separation(const std::shared_ptr<GameObject>& GameObjectPtr) :
		SteeringBehavior(GameObjectPtr)
	{
	}

	Vec3 Separation::Execute(const Vec3& Force) {
		Vec3 TempForce = Force;
		auto GameObj = GetGameObject();

		Vec3 SteeringForce(0, 0, 0);
		auto Vec = GetGameObjectGroup()->GetGroupVector();
		for (auto Ptr : Vec) {
			auto PtrObj = Ptr.lock();
			if (PtrObj) {
				if (PtrObj != GameObj) {
					//PtrObj->GetComponent<Transform>();
					Vec3 ToAgent
						= GameObj->GetComponent<Transform>()->GetWorldPosition()
						- PtrObj->GetComponent<Transform>()->GetWorldPosition();
					SteeringForce += bsmUtil::normalize(ToAgent) / bsmUtil::length(ToAgent);
				}
			}
		}
		AccumulateForce(TempForce, SteeringForce);
		return TempForce;
	}




}
// end namespace basecross

/*!
@file BehaviorSteering.cpp
@brief ���Ǎs���N���X����
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#include "stdafx.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	///	���Ǎs���̐e�N���X
	//--------------------------------------------------------------------------------------
	SteeringBehavior::SteeringBehavior(const shared_ptr<GameObject>& GameObjectPtr) :
		Behavior(GameObjectPtr),
		m_Weight(1.0f),
		m_MaxSpeed(10.0f),
		m_MaxForce(30.0f)
	{}
	SteeringBehavior::~SteeringBehavior() {}

	float SteeringBehavior::GetWeight() const { return m_Weight; }
	void SteeringBehavior::SetWeight(float f) { m_Weight = f; }

	float SteeringBehavior::GetMaxSpeed() const { return m_MaxSpeed; }
	void SteeringBehavior::SetMaxSpeed(float f) { m_MaxSpeed = f; }

	float SteeringBehavior::GetMaxForce() const { return m_MaxForce; }
	void SteeringBehavior::SetMaxForce(float f) { m_MaxForce = f; }



	//--------------------------------------------------------------------------------------
	///	SeekSteering�s���N���X
	//--------------------------------------------------------------------------------------
	SeekSteering::SeekSteering(const shared_ptr<GameObject>& GameObjectPtr) :
		SteeringBehavior(GameObjectPtr)
	{}
	SeekSteering::~SeekSteering() {}

	bsm::Vec3 SeekSteering::Execute(const bsm::Vec3& Force, const bsm::Vec3& Velocity, const bsm::Vec3& TargetPos) {
		auto TransPtr = GetGameObject()->GetComponent<Transform>();
		bsm::Vec3 TempForce = Force;
		bsm::Vec3 WorkForce;
		WorkForce = Steering::Seek(Velocity, TargetPos,
			TransPtr->GetWorldPosition(), GetMaxSpeed()) * GetWeight();
		Steering::AccumulateForce(TempForce, WorkForce, GetMaxForce());
		return TempForce;
	}


	//--------------------------------------------------------------------------------------
	///	ArriveSteering�s���N���X
	//--------------------------------------------------------------------------------------
	ArriveSteering::ArriveSteering(const shared_ptr<GameObject>& GameObjectPtr) :
		SteeringBehavior(GameObjectPtr),
		m_Decl(3.0f)
	{}
	ArriveSteering::~ArriveSteering() {}

	float ArriveSteering::GetDecl() const { return m_Decl; }
	void ArriveSteering::SetDecl(float f) { m_Decl = f; }

	bsm::Vec3 ArriveSteering::Execute(const bsm::Vec3& Force, const bsm::Vec3& Velocity, const bsm::Vec3& TargetPos) {
		auto TransPtr = GetGameObject()->GetComponent<Transform>();
		bsm::Vec3 TempForce = Force;
		bsm::Vec3 WorkForce;
		WorkForce = Steering::Arrive(Velocity, TargetPos,
			TransPtr->GetWorldPosition(), GetMaxSpeed(), m_Decl) * GetWeight();
		Steering::AccumulateForce(TempForce, WorkForce, GetMaxForce());
		return TempForce;
	}

	//--------------------------------------------------------------------------------------
	///	PursuitSteering�i�ǐՁj�s���N���X
	//--------------------------------------------------------------------------------------

	PursuitSteering::PursuitSteering(const shared_ptr<GameObject>& GameObjectPtr) :
		SteeringBehavior(GameObjectPtr)
	{}
	PursuitSteering::~PursuitSteering() {}

	bsm::Vec3 PursuitSteering::Execute(const bsm::Vec3& Force, const bsm::Vec3& Velocity, const bsm::Vec3& TargetPos, const bsm::Vec3& TargetVelocity, const bsm::Vec3& TargetRotation) {
		auto TransPtr = GetGameObject()->GetComponent<Transform>();
		bsm::Vec3 TempForce = Force;
		bsm::Vec3 WorkForce;
		WorkForce = Steering::Pursuit(Velocity, TransPtr->GetWorldPosition(),
			bsm::normalize(TransPtr->GetRotation()), GetMaxSpeed(),
			TargetVelocity, TargetPos,
			bsm::normalize(TargetRotation)) * GetWeight();
		Steering::AccumulateForce(TempForce, WorkForce, GetMaxForce());
		return TempForce;
	}

#ifdef test

	//--------------------------------------------------------------------------------------
	//	struct WanderSteering::Impl;
	//--------------------------------------------------------------------------------------
	struct WanderSteering::Impl {
		bsm::Vec3 m_WanderTarget;	//�p�j�ڕW�̉�]�W���i���ǂɂ���ď�����������j
		float m_WanderRadius;	//�p�j���a
		float m_WanderDistance;	//�p�j�~�܂ł̋���
		float m_WanderJitter;	//�����_���ψق̍ő�l
		Impl() :
			m_WanderTarget(0, 0, 0),
			m_WanderRadius(1.5f),
			m_WanderDistance(1.0f),
			m_WanderJitter(0.5f)
		{}
		~Impl() {}
	};

	//--------------------------------------------------------------------------------------
	///	WanderSteering�i�p�j�j�s��
	//--------------------------------------------------------------------------------------
	WanderSteering::WanderSteering(const shared_ptr<GameObject>& GameObjectPtr) :
		SteeringBehavior(GameObjectPtr),
		pImpl(new Impl())
	{}
	WanderSteering::~WanderSteering() {}
	//�A�N�Z�T
	const bsm::Vec3& WanderSteering::GetWanderTarget() const {
		return m_WanderTarget;
	}
	void WanderSteering::SetWanderTarget(const bsm::Vec3& target) {
		m_WanderTarget = target;
	}
	float WanderSteering::GetWanderRadius() const {
		return m_WanderRadius;
	}
	void WanderSteering::SetWanderRadius(float f) {
		m_WanderRadius = f;
	}
	float WanderSteering::GetWanderDistance() const {
		return m_WanderDistance;
	}
	void WanderSteering::SetWanderDistance(float f) {
		m_WanderDistance = f;
	}
	float WanderSteering::GetWanderJitter() const {
		return m_WanderJitter;
	}
	void WanderSteering::SetWanderJitter(float f) {
		m_WanderJitter = f;
	}


	void WanderSteering::Execute() {
		auto RigidPtr = GetGameObject()->GetComponent<Rigidbody>();
		auto TransPtr = GetGameObject()->GetComponent<Transform>();
		bsm::Vec3 Force = RigidPtr->GetForce();
		bsm::Vec3 WorkForce;
		WorkForce = Steering::Wander(TransPtr->GetWorldMatrix(),
			m_WanderRadius, m_WanderDistance, m_WanderJitter, m_WanderTarget) * GetWeight();
		Steering::AccumulateForce(Force, WorkForce, RigidPtr->GetMaxForce());
		RigidPtr->SetForce(Force);
	}



#endif


	//--------------------------------------------------------------------------------------
	///	WallAvoidanceSteering�i�ǉ���j�s��
	//--------------------------------------------------------------------------------------
	WallAvoidanceSteering::WallAvoidanceSteering(const shared_ptr<GameObject>& GameObjectPtr) :
		SteeringBehavior(GameObjectPtr)
	{}
	WallAvoidanceSteering::~WallAvoidanceSteering() {}

	//�A�N�Z�T
	vector<PLANE>& WallAvoidanceSteering::GetPlaneVec(){
		return m_PlaneVec;
	}
	void WallAvoidanceSteering::SetPlaneVec(const vector<PLANE>& planevec) {
		//�K���N���A����
		m_PlaneVec.clear();
		for (auto& v : planevec) {
			m_PlaneVec.push_back(v);
		}
	}
	void WallAvoidanceSteering::SetPlaneVec(const vector<bsm::Plane4>& planevec) {
		//�K���N���A����
		m_PlaneVec.clear();
		for (auto& v : planevec) {
			PLANE p(v);
			m_PlaneVec.push_back(p);
		}
	}


	bsm::Vec3 WallAvoidanceSteering::Execute(const bsm::Vec3& Force, const bsm::Vec3& Velocity) {
		auto TransPtr = GetGameObject()->GetComponent<Transform>();
		bsm::Vec3 TempForce = Force;
		bsm::Vec3 WorkForce(0, 0, 0);
		WorkForce = Steering::WallAvoidance(TransPtr->GetWorldMatrix(),
			Velocity, GetMaxSpeed(), m_PlaneVec) * GetWeight();
		Steering::AccumulateForce(TempForce, WorkForce, GetMaxForce());
		return TempForce;
	}


	//--------------------------------------------------------------------------------------
	///	 ObstacleAvoidanceSteering�i��Q������j�s��
	//--------------------------------------------------------------------------------------
	ObstacleAvoidanceSteering::ObstacleAvoidanceSteering(const shared_ptr<GameObject>& GameObjectPtr) :
		SteeringBehavior(GameObjectPtr),
		m_RoadWidth(1.0f),
		m_RoadHeight(1.0f)
	{}
	ObstacleAvoidanceSteering::~ObstacleAvoidanceSteering() {}
	//�A�N�Z�T
	//��Q���̔z��
	const vector<SPHERE>& ObstacleAvoidanceSteering::GetObstacleSphereVec() const {
		return m_ObstacleSphereVec;
	}
	void ObstacleAvoidanceSteering::SetObstacleSphereVec(const vector<SPHERE>& spherevec) {
		m_ObstacleSphereVec.clear();
		for (auto sp : spherevec) {
			m_ObstacleSphereVec.push_back(sp);
		}
	}

	//���̍���
	float ObstacleAvoidanceSteering::GetRoadWidth() const {
		return m_RoadWidth;
	}
	void ObstacleAvoidanceSteering::SetRoadWidth(float f) {
		m_RoadWidth = f;
	}
	//����
	float ObstacleAvoidanceSteering::GetRoadHeight() const {
		return m_RoadHeight;
	}
	void ObstacleAvoidanceSteering::SetRoadHeight(float f) {
		m_RoadHeight = f;
	}

	bsm::Vec3 ObstacleAvoidanceSteering::Execute(const bsm::Vec3& Force, const bsm::Vec3& Velocity) {
		auto TransPtr = GetGameObject()->GetComponent<Transform>();
		bsm::Vec3 TempForce = Force;
		bsm::Vec3 WorkForce(0, 0, 0);
		WorkForce = Steering::ObstacleAvoidance(TransPtr->GetWorldMatrix(),
			Velocity, GetMaxSpeed(), m_RoadWidth, m_RoadHeight,
			m_ObstacleSphereVec) * GetWeight();
		Steering::AccumulateForce(TempForce, WorkForce, GetMaxForce());
		return TempForce;
	}

	//--------------------------------------------------------------------------------------
	///	 FollowPathSteering�i�o�H�Ǐ]�j�s��
	//--------------------------------------------------------------------------------------
	FollowPathSteering::FollowPathSteering(const shared_ptr<GameObject>& GameObjectPtr) :
		SteeringBehavior(GameObjectPtr),
		m_Decl(3.0f),
		m_WaypointSpan(2.0f)
	{}
	FollowPathSteering::~FollowPathSteering() {}

	//�A�N�Z�T
	void FollowPathSteering::SetPathList(const list<bsm::Vec3>& pathlist) {
		m_Path.SetList(pathlist);
	}

	float FollowPathSteering::GetDecl() const {
		return m_Decl;
	}
	void FollowPathSteering::SetDecl(float f) {
		m_Decl = f;
	}

	float FollowPathSteering::GetWaypointSpan() const {
		return m_WaypointSpan;
	}
	void FollowPathSteering::SetWaypointSpan(float f) {
		m_WaypointSpan = f;
	}

	bool FollowPathSteering::GetLooped() const {
		return m_Path.GetLooped();
	}
	bool FollowPathSteering::IsLooped() const {
		return m_Path.GetLooped();
	}
	void FollowPathSteering::SetLooped(bool b) {
		m_Path.SetLooped(b);
	}

	bool FollowPathSteering::IsFinished() const {
		return m_Path.IsFinished();
	}

	bsm::Vec3 FollowPathSteering::Execute(const bsm::Vec3& Force, const bsm::Vec3& Velocity) {
		auto TransPtr = GetGameObject()->GetComponent<Transform>();
		bsm::Vec3 TempForce = Force;
		if (IsFinished()) {
			//�I�����Ă���A�n���ꂽ�t�H�[�X�����̂܂ܕԂ�
			return TempForce;
		}
		bsm::Vec3 WorkForce(0, 0, 0);
		WorkForce = Steering::FollowPath(m_Path,
			m_WaypointSpan,
			TransPtr->GetWorldPosition(),
			Velocity,
			GetMaxSpeed(),
			m_Decl)
			* GetWeight();
		Steering::AccumulateForce(TempForce, WorkForce, GetMaxForce());
		return TempForce;
	}
#ifdef test

	//--------------------------------------------------------------------------------------
	//	struct AlignmentSteering::Impl;
	//--------------------------------------------------------------------------------------
	struct AlignmentSteering::Impl {
		weak_ptr<GameObjectGroup> m_Group;
		Impl()
		{}
		~Impl() {}
	};

	//--------------------------------------------------------------------------------------
	///	 AlignmentSteering�i����j�s��
	//--------------------------------------------------------------------------------------
	AlignmentSteering::AlignmentSteering(const shared_ptr<GameObject>& GameObjectPtr) :
		SteeringBehavior(GameObjectPtr),
		pImpl(new Impl())
	{}
	AlignmentSteering::~AlignmentSteering() {}

	//�A�N�Z�T
	shared_ptr<GameObjectGroup> AlignmentSteering::GetGameObjectGroup() const {
		auto shptr = m_Group.lock();
		if (!shptr) {
			throw BaseException(
				L"�O���[�v�͖����ł�",
				L"�O���[�v�̑��݂��m�F���ĉ�����",
				L"AlignmentSteering::GetGameObjectGroup()"
			);
		}
		return shptr;
	}
	void AlignmentSteering::SetGameObjectGroup(const shared_ptr<GameObjectGroup>& Group) {
		m_Group = Group;
	}


	void AlignmentSteering::Execute() {
		auto RigidPtr = GetGameObject()->GetComponent<Rigidbody>();
		auto TransPtr = GetGameObject()->GetComponent<Transform>();
		bsm::Vec3 Force = RigidPtr->GetForce();
		bsm::Vec3 WorkForce;
		WorkForce = Steering::Alignment(GetGameObjectGroup(), GetGameObject()) * GetWeight();
		Steering::AccumulateForce(Force, WorkForce, RigidPtr->GetMaxForce());
		RigidPtr->SetForce(Force);
	}



	//--------------------------------------------------------------------------------------
	//	struct CohesionSteering::Impl;
	//--------------------------------------------------------------------------------------
	struct CohesionSteering::Impl {
		weak_ptr<GameObjectGroup> m_Group;
		Impl()
		{}
		~Impl() {}
	};

	//--------------------------------------------------------------------------------------
	///	 CohesionSteering�i�����j�s��
	//--------------------------------------------------------------------------------------
	CohesionSteering::CohesionSteering(const shared_ptr<GameObject>& GameObjectPtr) :
		SteeringBehavior(GameObjectPtr),
		pImpl(new Impl())
	{}
	CohesionSteering::~CohesionSteering() {}

	//�A�N�Z�T
	shared_ptr<GameObjectGroup> CohesionSteering::GetGameObjectGroup() const {
		auto shptr = m_Group.lock();
		if (!shptr) {
			throw BaseException(
				L"�O���[�v�͖����ł�",
				L"�O���[�v�̑��݂��m�F���ĉ�����",
				L"CohesionSteering::GetGameObjectGroup()"
			);
		}
		return shptr;
	}
	void CohesionSteering::SetGameObjectGroup(const shared_ptr<GameObjectGroup>& Group) {
		m_Group = Group;
	}

	void CohesionSteering::Execute() {
		auto RigidPtr = GetGameObject()->GetComponent<Rigidbody>();
		auto TransPtr = GetGameObject()->GetComponent<Transform>();
		bsm::Vec3 Force = RigidPtr->GetForce();
		bsm::Vec3 WorkForce;
		WorkForce = Steering::Cohesion(GetGameObjectGroup(), GetGameObject(),
			RigidPtr->GetVelocity(), RigidPtr->GetMaxSpeed()) * GetWeight();
		Steering::AccumulateForce(Force, WorkForce, RigidPtr->GetMaxForce());
		RigidPtr->SetForce(Force);
	}


#endif





	//--------------------------------------------------------------------------------------
	///	 SeparationSteering�i�����j�s���N���X
	//--------------------------------------------------------------------------------------
	SeparationSteering::SeparationSteering(const shared_ptr<GameObject>& GameObjectPtr) :
		SteeringBehavior(GameObjectPtr)
	{}

	SeparationSteering::~SeparationSteering() {}
	//�A�N�Z�T
	shared_ptr<GameObjectGroup> SeparationSteering::GetGameObjectGroup() const {
		auto shptr = m_Group.lock();
		if (!shptr) {
			throw BaseException(
				L"�O���[�v�͖����ł�",
				L"�O���[�v�̑��݂��m�F���ĉ�����",
				L"SeparationSteering::GetGameObjectGroup()"
			);
		}
		return shptr;
	}
	void SeparationSteering::SetGameObjectGroup(const shared_ptr<GameObjectGroup>& Group) {
		m_Group = Group;
	}

	bsm::Vec3 SeparationSteering::Execute(const bsm::Vec3& Force) {
		bsm::Vec3 TempForce = Force;
		bsm::Vec3 WorkForce;
		WorkForce = Steering::Separation(GetGameObjectGroup(), GetGameObject()) * GetWeight();
		Steering::AccumulateForce(TempForce, WorkForce, GetMaxForce());
		return TempForce;
	}

}
// end namespace basecross
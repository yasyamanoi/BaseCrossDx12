/*!
@file Steering.cpp
@brief ���Ǌ֘A���[�e�B���e�B
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	struct Steering;
	//	�p�r: ���Ǌ֘A���[�e�B���e�B
	//	��static�Ăяo��������
	//--------------------------------------------------------------------------------------
	//�X�^�e�B�b�N�����o
	bool Steering::AccumulateForce(bsm::Vec3& Force, const bsm::Vec3& ForceToAdd, float MaxForce) {
		//���݂̗͂̒����𓾂�
		float MagnitudeSoFar = bsm::length(Force);
		//�ő�l�Ƃ̍������߂�
		float magnitudeRemaining = MaxForce - MagnitudeSoFar;
		//����0�ȉ��i�܂�ő�l�𒴂��Ă�����j
		//�ǉ����Ȃ��Ń��^�[��
		if (magnitudeRemaining <= 0.0f) {
			return false;
		}
		//�ǉ�����͂̑傫�������߂�
		float MagnitudeToAdd = bsm::length(ForceToAdd);
		//�͂̒ǉ�
		if (MagnitudeToAdd < magnitudeRemaining) {
			Force += ForceToAdd;
		}
		else {
			Force += (bsm::normalize(ForceToAdd) * MagnitudeToAdd);
		}
		//�ǉ����ꂽ�w�W��Ԃ�  
		return true;
	}

	//--------------------------------------------------------------------------------------
	bsm::Vec3 Steering::Seek(const bsm::Vec3& Velocity, const bsm::Vec3& Target, const bsm::Vec3& Pos, float MaxSpeed) {
		bsm::Vec3 DesiredVelocity
			= bsm::normalize(Target - Pos) * MaxSpeed;
		return (DesiredVelocity - Velocity);
	}

	//--------------------------------------------------------------------------------------
	bsm::Vec3 Steering::Flee(const bsm::Vec3& Velocity, const bsm::Vec3& Target,
		const bsm::Vec3& Pos, float MaxSpeed, float PanicDistance) {
		float PanicDistanceSq = PanicDistance * PanicDistance;
		if (bsm::lengthSqr(Pos - Target) > PanicDistanceSq) {
			return bsm::Vec3(0, 0, 0);
		}
		bsm::Vec3 DesiredVelocity
			= bsm::normalize(Pos - Target) * MaxSpeed;
		return (DesiredVelocity - Velocity);
	}

	//--------------------------------------------------------------------------------------
	bsm::Vec3 Steering::Arrive(const bsm::Vec3& Velocity, const bsm::Vec3& Target, const bsm::Vec3& Pos, float MaxSpeed, float Decl) {
		bsm::Vec3 ToTarget = Target - Pos;
		float dist = bsm::length(ToTarget);
		if (dist > 0) {
			const float DecelerationTweaker = 0.3f;
			float speed = dist / (Decl * DecelerationTweaker);
			speed = Util::Minimum(speed, MaxSpeed);
			bsm::Vec3 DesiredVelocity = ToTarget * speed / dist;
			return (DesiredVelocity - Velocity);
		}
		return bsm::Vec3(0, 0, 0);
	}

	//--------------------------------------------------------------------------------------
	bsm::Vec3 Steering::Pursuit(const bsm::Vec3& Velocity, const bsm::Vec3& Pos, const bsm::Vec3& Rot, float MaxSpeed,
		const bsm::Vec3& TargetVelocity, const bsm::Vec3& Target, const bsm::Vec3& TargetRot) {
		bsm::Vec3 ToEvader = Target - Pos;
		//		double RelativeHeading = bsm::dot(Rot,TargetRot);
		auto RelativeHeading = abs(bsm::angleBetweenNormals(Rot, TargetRot));
		if ((bsm::dot(ToEvader, Rot) > 0) &&
			(RelativeHeading < 0.95))  //acos(0.95)=18 degs
		{
			return Steering::Seek(Velocity, Target, Pos, MaxSpeed);
		}
		float LookAheadTime = bsm::length(ToEvader) /
			(MaxSpeed + bsm::length(TargetVelocity));
		return Steering::Seek(Velocity, Target + TargetVelocity * LookAheadTime, Pos, MaxSpeed);
	}


	//--------------------------------------------------------------------------------------
	bsm::Vec3 Steering::Wander(const bsm::Mat4x4 Matrix,
		float WanderRadius, float WanderDistance, float WanderJitter, bsm::Vec3& WanderTarget) {
		WanderTarget += bsm::Vec3(
			(Util::RandZeroToOne(true) * 2.0f - 1.0f) * WanderJitter,
			0,
			(Util::RandZeroToOne(true) * 2.0f - 1.0f) * WanderJitter
		);
		WanderTarget.normalize();
		WanderTarget *= WanderRadius;
		bsm::Vec3 target_Local = WanderTarget + bsm::Vec3(0, 0, WanderDistance);
		bsm::Vec3 Scale, Pos;
		bsm::Quat Qt;
		Matrix.decompose(Scale, Qt, Pos);
		bsm::Mat4x4 mat;
		mat.affineTransformation(Scale, target_Local, Qt, Pos);
		target_Local *= Matrix;
		return target_Local - Matrix.transInMatrix();
	}

	struct ObstacleAvoidanceSphere {
		SPHERE m_Sp;
		float Len;
		ObstacleAvoidanceSphere(const SPHERE& Sp, float len) :
			m_Sp(Sp), Len(len) {}
	};

	//--------------------------------------------------------------------------------------
	bool SortSphereObstacleAvoidanceHandle(ObstacleAvoidanceSphere& Left, ObstacleAvoidanceSphere& Right) {
		return Left.Len < Right.Len;
	}

	//--------------------------------------------------------------------------------------
	bsm::Vec3 Steering::ObstacleAvoidance(const bsm::Mat4x4 Matrix,
		const bsm::Vec3& Velocity, float MaxSpeed, float Width, float Height,
		const vector<SPHERE>& SphereVec) {
		//���݂̑��x�ƈʒu�Ɠ�������A�Փ˔���OBB���쐬����
		bsm::Vec3 Scale(Width, Height, bsm::length(Velocity));
		bsm::Mat4x4 ObbMat;
		ObbMat.affineTransformation(
			Scale,
			bsm::Vec3(0, 0, 0),
			Matrix.quatInMatrix(),
			Matrix.transInMatrix() + Velocity / 2.0f);
		OBB Obb(bsm::Vec3(1.0f, 1.0f, 1.0f), ObbMat);
		vector<ObstacleAvoidanceSphere> ChangeVec;
		for (size_t i = 0; i < SphereVec.size(); i++) {
			float len = bsm::length(SphereVec[i].m_Center - Matrix.transInMatrix());
			ObstacleAvoidanceSphere Sp(SphereVec[i], len);
			ChangeVec.push_back(Sp);
		}
		//������ύX�\�[�g
		std::sort(ChangeVec.begin(), ChangeVec.end(), SortSphereObstacleAvoidanceHandle);
		//�߂����Ɍ������ĉ����ƏՓ˂��Ă�����A�^�[�Q�b�g������SEEK
		for (size_t i = 0; i < ChangeVec.size(); i++) {
			bsm::Vec3 RetVec;
			if (HitTest::SPHERE_OBB(ChangeVec[i].m_Sp, Obb, RetVec)) {
				//�i�s������OBB�ƏՓ˂���
				//OBB�i�s�����̐�����RetVec�Ƃ̍ŋߐړ_�����߂�
				float t;
				bsm::Vec3 d;
				HitTest::ClosetPtPointSegment(RetVec, Matrix.transInMatrix(), Matrix.transInMatrix() + Velocity, t, d);
				//�ޔ�������v�Z����
				bsm::Vec3 AvoidanceVec = (d - RetVec);
				//���K��
				AvoidanceVec.normalize();
				AvoidanceVec *= (Width + MaxSpeed);
				return AvoidanceVec;
			}
		}
		return bsm::Vec3(0, 0, 0);
	}

	struct AvoidanceSegment {
		bsm::Vec3 m_PointA;
		bsm::Vec3 m_PointB;
		AvoidanceSegment() {}
		AvoidanceSegment(const bsm::Vec3& pa, const bsm::Vec3& pb) :
			m_PointA(pa),
			m_PointB(pb)
		{
		}
	};

	//--------------------------------------------------------------------------------------
	bsm::Vec3 Steering::WallAvoidance(const bsm::Mat4x4 Matrix,
		const bsm::Vec3& Velocity, float MaxSpeed, const vector<PLANE>& PlaneVec) {
		//�܂��G�o�ɂȂ�����z����쐬
		float Len = bsm::length(Velocity) * 0.5f;
		vector<AvoidanceSegment> Segments;
		Segments.push_back(AvoidanceSegment(bsm::Vec3(0, 0, Len), bsm::Vec3(0, 0, 0)));
		Segments.push_back(AvoidanceSegment(bsm::Vec3(cos(XM_PIDIV4) * Len, 0, sin(XM_PIDIV4) * Len), bsm::Vec3(0, 0, 0)));
		Segments.push_back(AvoidanceSegment(bsm::Vec3(-cos(XM_PIDIV4) * Len, 0, sin(XM_PIDIV4) * Len), bsm::Vec3(0, 0, 0)));
		Segments.push_back(AvoidanceSegment(bsm::Vec3(Len, 0, 0), bsm::Vec3(0, 0, 0)));
		Segments.push_back(AvoidanceSegment(bsm::Vec3(-Len, 0, 0), bsm::Vec3(0, 0, 0)));
		Segments.push_back(AvoidanceSegment(bsm::Vec3(cos(XM_PIDIV4) * Len, 0, -sin(XM_PIDIV4) * Len), bsm::Vec3(0, 0, 0)));
		Segments.push_back(AvoidanceSegment(bsm::Vec3(-cos(XM_PIDIV4) * Len, 0, -sin(XM_PIDIV4) * Len), bsm::Vec3(0, 0, 0)));
		Segments.push_back(AvoidanceSegment(bsm::Vec3(0, 0, -Len), bsm::Vec3(0, 0, 0)));
		for (size_t i = 0; i < Segments.size(); i++) {
			//�G�o�̊e���_�ɍs����v�Z���āA���[���h���W�ɕϊ�
			Segments[i].m_PointA *= Matrix;
			Segments[i].m_PointB *= Matrix;
			//�����ƕǂ̏Փ˔���
			bsm::Vec3 RetVec;
			float t;
			for (size_t j = 0; j < PlaneVec.size(); j++) {
				if (HitTest::InsidePtPlane(Matrix.transInMatrix(), PlaneVec[j])) {
					return PlaneVec[j].m_Normal * MaxSpeed;
				}
				if (HitTest::SEGMENT_PLANE(Segments[i].m_PointA, Segments[i].m_PointB, PlaneVec[j], t, RetVec)) {
					//�����Ɩʂ��Փ˂��Ă���
					//�ʂ̖@���̕����Ɍ��݂̑��x�Ńt�H�[�X��Ԃ�
					return PlaneVec[j].m_Normal * bsm::length(Velocity);
				}
			}
		}
		return bsm::Vec3(0, 0, 0);
	}



	//--------------------------------------------------------------------------------------
	bsm::Vec3 Steering::FollowPath(Path& rPath,
		float WaypointSeekDist,
		const bsm::Vec3& Pos, const bsm::Vec3& Velocity,
		float MaxSpeed, float Decl) {
		bsm::Vec3 Dis = Pos - rPath.GetCurWaypoint();
		float DisSq = bsm::lengthSqr(Dis);
		float WaypointSeekDistSq = WaypointSeekDist * WaypointSeekDist;

		if (DisSq < WaypointSeekDistSq) {
			rPath.SetNextWaypoint();
		}
		if (!rPath.IsFinished()) {
			return Seek(Velocity, rPath.GetCurWaypoint(), Pos, MaxSpeed);
		}
		else {
			return Arrive(Velocity, rPath.GetCurWaypoint(), Pos, MaxSpeed, Decl);
		}
	}

	bsm::Vec3 Steering::Separation(const shared_ptr<GameObjectGroup>& Group, const shared_ptr<GameObject>& MyObj) {
		bsm::Vec3 SteeringForce(0, 0, 0);
		auto Vec = Group->GetGroupVector();
		for (auto Ptr : Vec) {
			if (!Ptr.expired()) {
				auto PtrObj = Ptr.lock();
				if (PtrObj != MyObj) {
					PtrObj->GetComponent<Transform>();
					bsm::Vec3 ToAgent
						= MyObj->GetComponent<Transform>()->GetWorldPosition()
						- PtrObj->GetComponent<Transform>()->GetWorldPosition();
					SteeringForce += bsm::normalize(ToAgent) / bsm::length(ToAgent);
				}
			}
		}
		return SteeringForce;
	}




	//--------------------------------------------------------------------------------------
	//	static bsm::Vec3 Alignment(
	//	const shared_ptr<GameObjectGroup>& Group,	//�ݒ肷��O���[�v�̔z��
	//	const shared_ptr<GameObject>& MyObj				//�������g
	//	);
	//	�p�r: ����s��
	//	�߂�l: �t�H�[�X
	//--------------------------------------------------------------------------------------
	bsm::Vec3 Steering::Alignment(const shared_ptr<GameObjectGroup>& Group, const shared_ptr<GameObject>& MyObj) {
		bsm::Vec3 AverageHeading(0, 0, 0);
		auto Vec = Group->GetGroupVector();
		int count = 0;
		for (auto Ptr : Vec) {
			if (!Ptr.expired()) {
				auto PtrObj = Ptr.lock();
				if (PtrObj != MyObj) {
					auto PtrT = PtrObj->GetComponent<Transform>();
					AverageHeading += PtrT->GetQuaternion().toRotVec();
					count++;
				}
			}
		}
		if (count > 0) {
			AverageHeading /= (float)count;
			auto PtrT = MyObj->GetComponent<Transform>();
			AverageHeading -= PtrT->GetQuaternion().toRotVec();
		}
		return AverageHeading;
	}

	//--------------------------------------------------------------------------------------
	//	static bsm::Vec3 Cohesion(
	//	const shared_ptr<GameObjectGroup>& Group,	//�ݒ肷��O���[�v�̔z��
	//	const shared_ptr<GameObject>& MyObj			//�������g
	//	const bsm::Vec3& Velocity,	//���݂̑��x
	//	float MaxSpeed				//�ō����x
	//	);
	//	�p�r: �����s��
	//	�߂�l: �t�H�[�X
	//--------------------------------------------------------------------------------------
	bsm::Vec3 Steering::Cohesion(const shared_ptr<GameObjectGroup>& Group, const shared_ptr<GameObject>& MyObj,
		const bsm::Vec3& Velocity, float MaxSpeed) {
		auto Vec = Group->GetGroupVector();
		bsm::Vec3 SteeringForce(0, 0, 0);
		//�d�S
		bsm::Vec3 CenterOfMass(0, 0, 0);
		int count = 0;
		for (auto Ptr : Vec) {
			if (!Ptr.expired()) {
				auto PtrObj = Ptr.lock();
				if (PtrObj != MyObj) {
					auto PtrT = PtrObj->GetComponent<Transform>();
					CenterOfMass += PtrT->GetWorldPosition();
					count++;
				}
			}
		}
		if (count > 0) {
			CenterOfMass /= (float)count;
			auto PtrT = MyObj->GetComponent<Transform>();
			SteeringForce = Seek(Velocity, CenterOfMass, PtrT->GetWorldPosition(), MaxSpeed);
			SteeringForce.normalize();
		}
		return SteeringForce;
	}

}
// end namespace basecross
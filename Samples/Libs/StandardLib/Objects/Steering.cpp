/*!
@file Steering.cpp
@brief ���Ǌ֘A�@����
*/

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	struct Steering;
	//	�p�r: ���Ǌ֘A���[�e�B���e�B
	//	��static�Ăяo��������
	//--------------------------------------------------------------------------------------
	//�X�^�e�B�b�N�����o
	bool Steering::AccumulateForce(Vec3& Force, const Vec3& ForceToAdd, float MaxForce) {
		//���݂̗͂̒����𓾂�
		float MagnitudeSoFar = bsmUtil::length(Force);
		//�ő�l�Ƃ̍������߂�
		float magnitudeRemaining = MaxForce - MagnitudeSoFar;
		//����0�ȉ��i�܂�ő�l�𒴂��Ă�����j
		//�ǉ����Ȃ��Ń��^�[��
		if (magnitudeRemaining <= 0.0f) {
			return false;
		}
		//�ǉ�����͂̑傫�������߂�
		float MagnitudeToAdd = bsmUtil::length(ForceToAdd);
		//�͂̒ǉ�
		if (MagnitudeToAdd < magnitudeRemaining) {
			Force += ForceToAdd;
		}
		else {
			Force += (bsmUtil::normalize(ForceToAdd) * MagnitudeToAdd);
		}
		//�ǉ����ꂽ�w�W��Ԃ�  
		return true;
	}

	//--------------------------------------------------------------------------------------
	Vec3 Steering::Seek(const Vec3& Velocity, const Vec3& Target, const Vec3& Pos, float MaxSpeed) {
		Vec3 DesiredVelocity
			= bsmUtil::normalize(Target - Pos) * MaxSpeed;
		return (DesiredVelocity - Velocity);
	}

	//--------------------------------------------------------------------------------------
	Vec3 Steering::Flee(const Vec3& Velocity, const Vec3& Target,
		const Vec3& Pos, float MaxSpeed, float PanicDistance) {
		float PanicDistanceSq = PanicDistance * PanicDistance;
		if (bsmUtil::lengthSqr(Pos - Target) > PanicDistanceSq) {
			return Vec3(0, 0, 0);
		}
		Vec3 DesiredVelocity
			= bsmUtil::normalize(Pos - Target) * MaxSpeed;
		return (DesiredVelocity - Velocity);
	}

	//--------------------------------------------------------------------------------------
	Vec3 Steering::Arrive(const Vec3& Velocity, const Vec3& Target, const Vec3& Pos, float MaxSpeed, float Decl) {
		Vec3 ToTarget = Target - Pos;
		float dist = bsmUtil::length(ToTarget);
		if (dist > 0) {
			const float DecelerationTweaker = 0.3f;
			float speed = dist / (Decl * DecelerationTweaker);
			speed = Util::Minimum(speed, MaxSpeed);
			Vec3 DesiredVelocity = ToTarget * speed / dist;
			return (DesiredVelocity - Velocity);
		}
		return Vec3(0, 0, 0);
	}

	//--------------------------------------------------------------------------------------
	Vec3 Steering::Pursuit(const Vec3& Velocity, const Vec3& Pos, const Vec3& Rot, float MaxSpeed,
		const Vec3& TargetVelocity, const Vec3& Target, const Vec3& TargetRot) {
		Vec3 ToEvader = Target - Pos;
		//		double RelativeHeading = dot(Rot,TargetRot);
		auto RelativeHeading = abs(bsmUtil::angleBetweenNormals(Rot, TargetRot));
		if ((bsmUtil::dot(ToEvader, Rot) > 0) &&
			(RelativeHeading < 0.95))  //acos(0.95)=18 degs
		{
			return Steering::Seek(Velocity, Target, Pos, MaxSpeed);
		}
		float LookAheadTime = bsmUtil::length(ToEvader) /
			(MaxSpeed + bsmUtil::length(TargetVelocity));
		return Steering::Seek(Velocity, Target + TargetVelocity * LookAheadTime, Pos, MaxSpeed);
	}


	//--------------------------------------------------------------------------------------
	Vec3 Steering::Wander(const Mat4x4 Matrix,
		float WanderRadius, float WanderDistance, float WanderJitter, Vec3& WanderTarget) {
		WanderTarget += Vec3(
			(Util::RandZeroToOne(true) * 2.0f - 1.0f) * WanderJitter,
			0,
			(Util::RandZeroToOne(true) * 2.0f - 1.0f) * WanderJitter
		);
		WanderTarget.normalize();
		WanderTarget *= WanderRadius;
		Vec3 target_Local = WanderTarget + Vec3(0, 0, WanderDistance);
		Vec3 Scale, Pos;
		Quat Qt;
		Matrix.decompose(Scale, Qt, Pos);
		Mat4x4 mat;
		mat.affineTransformation(Scale, target_Local, Qt, Pos);
		target_Local *= Matrix;
		return target_Local - Matrix.transInMatrix();
	}

	struct ObstacleAvoidanceSphere {
		SPHERE m_Sp;
		float Len;
		ObstacleAvoidanceSphere(const SPHERE& Sp, float len) :
			m_Sp(Sp), Len(len) {
		}
	};

	//--------------------------------------------------------------------------------------
	bool SortSphereObstacleAvoidanceHandle(ObstacleAvoidanceSphere& Left, ObstacleAvoidanceSphere& Right) {
		return Left.Len < Right.Len;
	}

	//--------------------------------------------------------------------------------------
	Vec3 Steering::ObstacleAvoidance(const Mat4x4 Matrix,
		const Vec3& Velocity, float MaxSpeed, float Width, float Height,
		const std::vector<SPHERE>& SphereVec) {
		//���݂̑��x�ƈʒu�Ɠ�������A�Փ˔���OBB���쐬����
		Vec3 Scale(Width, Height, bsmUtil::length(Velocity));
		Mat4x4 ObbMat;
		ObbMat.affineTransformation(
			Scale,
			Vec3(0, 0, 0),
			Matrix.quatInMatrix(),
			Matrix.transInMatrix() + Velocity / 2.0f);
		OBB Obb(Vec3(1.0f, 1.0f, 1.0f), ObbMat);
		std::vector<ObstacleAvoidanceSphere> ChangeVec;
		for (size_t i = 0; i < SphereVec.size(); i++) {
			float len = bsmUtil::length(SphereVec[i].m_Center - Matrix.transInMatrix());
			ObstacleAvoidanceSphere Sp(SphereVec[i], len);
			ChangeVec.push_back(Sp);
		}
		//������ύX�\�[�g
		std::sort(ChangeVec.begin(), ChangeVec.end(), SortSphereObstacleAvoidanceHandle);
		//�߂����Ɍ������ĉ����ƏՓ˂��Ă�����A�^�[�Q�b�g������SEEK
		for (size_t i = 0; i < ChangeVec.size(); i++) {
			Vec3 RetVec;
			if (HitTest::SPHERE_OBB(ChangeVec[i].m_Sp, Obb, RetVec)) {
				//�i�s������OBB�ƏՓ˂���
				//OBB�i�s�����̐�����RetVec�Ƃ̍ŋߐړ_�����߂�
				float t;
				Vec3 d;
				HitTest::ClosetPtPointSegment(RetVec, Matrix.transInMatrix(), Matrix.transInMatrix() + Velocity, t, d);
				//�ޔ�������v�Z����
				Vec3 AvoidanceVec = (d - RetVec);
				//���K��
				AvoidanceVec.normalize();
				AvoidanceVec *= (Width + MaxSpeed);
				return AvoidanceVec;
			}
		}
		return Vec3(0, 0, 0);
	}

	struct AvoidanceSegment {
		Vec3 m_PointA;
		Vec3 m_PointB;
		AvoidanceSegment() {}
		AvoidanceSegment(const Vec3& pa, const Vec3& pb) :
			m_PointA(pa),
			m_PointB(pb)
		{
		}
	};

	//--------------------------------------------------------------------------------------
	Vec3 Steering::WallAvoidance(const Mat4x4 Matrix,
		const Vec3& Velocity, float MaxSpeed, const std::vector<PLANE>& PlaneVec) {
		//�܂��G�o�ɂȂ�����z����쐬
		float Len = bsmUtil::length(Velocity) * 0.5f;
		std::vector<AvoidanceSegment> Segments;
		Segments.push_back(AvoidanceSegment(Vec3(0, 0, Len), Vec3(0, 0, 0)));
		Segments.push_back(AvoidanceSegment(Vec3(cos(XM_PIDIV4) * Len, 0, sin(XM_PIDIV4) * Len), Vec3(0, 0, 0)));
		Segments.push_back(AvoidanceSegment(Vec3(-cos(XM_PIDIV4) * Len, 0, sin(XM_PIDIV4) * Len), Vec3(0, 0, 0)));
		Segments.push_back(AvoidanceSegment(Vec3(Len, 0, 0), Vec3(0, 0, 0)));
		Segments.push_back(AvoidanceSegment(Vec3(-Len, 0, 0), Vec3(0, 0, 0)));
		Segments.push_back(AvoidanceSegment(Vec3(cos(XM_PIDIV4) * Len, 0, -sin(XM_PIDIV4) * Len), Vec3(0, 0, 0)));
		Segments.push_back(AvoidanceSegment(Vec3(-cos(XM_PIDIV4) * Len, 0, -sin(XM_PIDIV4) * Len), Vec3(0, 0, 0)));
		Segments.push_back(AvoidanceSegment(Vec3(0, 0, -Len), Vec3(0, 0, 0)));
		for (size_t i = 0; i < Segments.size(); i++) {
			//�G�o�̊e���_�ɍs����v�Z���āA���[���h���W�ɕϊ�
			Segments[i].m_PointA *= Matrix;
			Segments[i].m_PointB *= Matrix;
			//�����ƕǂ̏Փ˔���
			Vec3 RetVec;
			float t;
			for (size_t j = 0; j < PlaneVec.size(); j++) {
				if (HitTest::InsidePtPlane(Matrix.transInMatrix(), PlaneVec[j])) {
					return PlaneVec[j].m_Normal * MaxSpeed;
				}
				if (HitTest::SEGMENT_PLANE(Segments[i].m_PointA, Segments[i].m_PointB, PlaneVec[j], t, RetVec)) {
					//�����Ɩʂ��Փ˂��Ă���
					//�ʂ̖@���̕����Ɍ��݂̑��x�Ńt�H�[�X��Ԃ�
					return PlaneVec[j].m_Normal * bsmUtil::length(Velocity);
				}
			}
		}
		return Vec3(0, 0, 0);
	}



	//--------------------------------------------------------------------------------------
	Vec3 Steering::FollowPath(Path& rPath,
		float WaypointSeekDist,
		const Vec3& Pos, const Vec3& Velocity,
		float MaxSpeed, float Decl) {
		Vec3 Dis = Pos - rPath.GetCurWaypoint();
		float DisSq = bsmUtil::lengthSqr(Dis);
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

	Vec3 Steering::Separation(const std::shared_ptr<GameObjectGroup>& Group, const std::shared_ptr<GameObject>& MyObj) {
		Vec3 SteeringForce(0, 0, 0);
		auto Vec = Group->GetGroupVector();
		for (auto Ptr : Vec) {
			if (!Ptr.expired()) {
				auto PtrObj = Ptr.lock();
				if (PtrObj != MyObj) {
					PtrObj->GetComponent<Transform>();
					Vec3 ToAgent
						= MyObj->GetComponent<Transform>()->GetWorldPosition()
						- PtrObj->GetComponent<Transform>()->GetWorldPosition();
					SteeringForce += bsmUtil::normalize(ToAgent) / bsmUtil::length(ToAgent);
				}
			}
		}
		return SteeringForce;
	}




	//--------------------------------------------------------------------------------------
	//	static Vec3 Alignment(
	//	const shared_ptr<GameObjectGroup>& Group,	//�ݒ肷��O���[�v�̔z��
	//	const shared_ptr<GameObject>& MyObj				//�������g
	//	);
	//	�p�r: ����s��
	//	�߂�l: �t�H�[�X
	//--------------------------------------------------------------------------------------
	Vec3 Steering::Alignment(const std::shared_ptr<GameObjectGroup>& Group, const std::shared_ptr<GameObject>& MyObj) {
		Vec3 AverageHeading(0, 0, 0);
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
	//	static Vec3 Cohesion(
	//	const shared_ptr<GameObjectGroup>& Group,	//�ݒ肷��O���[�v�̔z��
	//	const shared_ptr<GameObject>& MyObj			//�������g
	//	const Vec3& Velocity,	//���݂̑��x
	//	float MaxSpeed				//�ō����x
	//	);
	//	�p�r: �����s��
	//	�߂�l: �t�H�[�X
	//--------------------------------------------------------------------------------------
	Vec3 Steering::Cohesion(const std::shared_ptr<GameObjectGroup>& Group, const std::shared_ptr<GameObject>& MyObj,
		const Vec3& Velocity, float MaxSpeed) {
		auto Vec = Group->GetGroupVector();
		Vec3 SteeringForce(0, 0, 0);
		//�d�S
		Vec3 CenterOfMass(0, 0, 0);
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
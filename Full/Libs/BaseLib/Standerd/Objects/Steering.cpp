/*!
@file Steering.cpp
@brief 操舵関連　実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	struct Steering;
	//	用途: 操舵関連ユーティリティ
	//	＊static呼び出しをする
	//--------------------------------------------------------------------------------------
	//スタティックメンバ
	bool Steering::AccumulateForce(bsm::Vec3& Force, const bsm::Vec3& ForceToAdd, float MaxForce) {
		//現在の力の長さを得る
		float MagnitudeSoFar = bsm::bsmUtil::length(Force);
		//最大値との差を求める
		float magnitudeRemaining = MaxForce - MagnitudeSoFar;
		//差が0以下（つまり最大値を超えていたら）
		//追加しないでリターン
		if (magnitudeRemaining <= 0.0f) {
			return false;
		}
		//追加する力の大きさを求める
		float MagnitudeToAdd = bsm::bsmUtil::length(ForceToAdd);
		//力の追加
		if (MagnitudeToAdd < magnitudeRemaining) {
			Force += ForceToAdd;
		}
		else {
			Force += (bsm::bsmUtil::normalize(ForceToAdd) * MagnitudeToAdd);
		}
		//追加された指標を返す  
		return true;
	}

	//--------------------------------------------------------------------------------------
	bsm::Vec3 Steering::Seek(const bsm::Vec3& Velocity, const bsm::Vec3& Target, const bsm::Vec3& Pos, float MaxSpeed) {
		bsm::Vec3 DesiredVelocity
			= bsm::bsmUtil::normalize(Target - Pos) * MaxSpeed;
		return (DesiredVelocity - Velocity);
	}

	//--------------------------------------------------------------------------------------
	bsm::Vec3 Steering::Flee(const bsm::Vec3& Velocity, const bsm::Vec3& Target,
		const bsm::Vec3& Pos, float MaxSpeed, float PanicDistance) {
		float PanicDistanceSq = PanicDistance * PanicDistance;
		if (bsm::bsmUtil::lengthSqr(Pos - Target) > PanicDistanceSq) {
			return bsm::Vec3(0, 0, 0);
		}
		bsm::Vec3 DesiredVelocity
			= bsm::bsmUtil::normalize(Pos - Target) * MaxSpeed;
		return (DesiredVelocity - Velocity);
	}

	//--------------------------------------------------------------------------------------
	bsm::Vec3 Steering::Arrive(const bsm::Vec3& Velocity, const bsm::Vec3& Target, const bsm::Vec3& Pos, float MaxSpeed, float Decl) {
		bsm::Vec3 ToTarget = Target - Pos;
		float dist = bsm::bsmUtil::length(ToTarget);
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
		auto RelativeHeading = abs(bsm::bsmUtil::angleBetweenNormals(Rot, TargetRot));
		if ((bsm::bsmUtil::dot(ToEvader, Rot) > 0) &&
			(RelativeHeading < 0.95))  //acos(0.95)=18 degs
		{
			return Steering::Seek(Velocity, Target, Pos, MaxSpeed);
		}
		float LookAheadTime = bsm::bsmUtil::length(ToEvader) /
			(MaxSpeed + bsm::bsmUtil::length(TargetVelocity));
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
			m_Sp(Sp), Len(len) {
		}
	};

	//--------------------------------------------------------------------------------------
	bool SortSphereObstacleAvoidanceHandle(ObstacleAvoidanceSphere& Left, ObstacleAvoidanceSphere& Right) {
		return Left.Len < Right.Len;
	}

	//--------------------------------------------------------------------------------------
	bsm::Vec3 Steering::ObstacleAvoidance(const bsm::Mat4x4 Matrix,
		const bsm::Vec3& Velocity, float MaxSpeed, float Width, float Height,
		const std::vector<SPHERE>& SphereVec) {
		//現在の速度と位置と道幅から、衝突判定OBBを作成する
		bsm::Vec3 Scale(Width, Height, bsm::bsmUtil::length(Velocity));
		bsm::Mat4x4 ObbMat;
		ObbMat.affineTransformation(
			Scale,
			bsm::Vec3(0, 0, 0),
			Matrix.quatInMatrix(),
			Matrix.transInMatrix() + Velocity / 2.0f);
		OBB Obb(bsm::Vec3(1.0f, 1.0f, 1.0f), ObbMat);
		std::vector<ObstacleAvoidanceSphere> ChangeVec;
		for (size_t i = 0; i < SphereVec.size(); i++) {
			float len = bsm::bsmUtil::length(SphereVec[i].m_Center - Matrix.transInMatrix());
			ObstacleAvoidanceSphere Sp(SphereVec[i], len);
			ChangeVec.push_back(Sp);
		}
		//順序を変更ソート
		std::sort(ChangeVec.begin(), ChangeVec.end(), SortSphereObstacleAvoidanceHandle);
		//近い順に検査して何かと衝突していたら、ターゲットを決めSEEK
		for (size_t i = 0; i < ChangeVec.size(); i++) {
			bsm::Vec3 RetVec;
			if (HitTest::SPHERE_OBB(ChangeVec[i].m_Sp, Obb, RetVec)) {
				//進行方向のOBBと衝突した
				//OBB進行方向の線分とRetVecとの最近接点を求める
				float t;
				bsm::Vec3 d;
				HitTest::ClosetPtPointSegment(RetVec, Matrix.transInMatrix(), Matrix.transInMatrix() + Velocity, t, d);
				//退避方向を計算する
				bsm::Vec3 AvoidanceVec = (d - RetVec);
				//正規化
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
		const bsm::Vec3& Velocity, float MaxSpeed, const std::vector<PLANE>& PlaneVec) {
		//まず触覚になる線分配列を作成
		float Len = bsm::bsmUtil::length(Velocity) * 0.5f;
		std::vector<AvoidanceSegment> Segments;
		Segments.push_back(AvoidanceSegment(bsm::Vec3(0, 0, Len), bsm::Vec3(0, 0, 0)));
		Segments.push_back(AvoidanceSegment(bsm::Vec3(cos(XM_PIDIV4) * Len, 0, sin(XM_PIDIV4) * Len), bsm::Vec3(0, 0, 0)));
		Segments.push_back(AvoidanceSegment(bsm::Vec3(-cos(XM_PIDIV4) * Len, 0, sin(XM_PIDIV4) * Len), bsm::Vec3(0, 0, 0)));
		Segments.push_back(AvoidanceSegment(bsm::Vec3(Len, 0, 0), bsm::Vec3(0, 0, 0)));
		Segments.push_back(AvoidanceSegment(bsm::Vec3(-Len, 0, 0), bsm::Vec3(0, 0, 0)));
		Segments.push_back(AvoidanceSegment(bsm::Vec3(cos(XM_PIDIV4) * Len, 0, -sin(XM_PIDIV4) * Len), bsm::Vec3(0, 0, 0)));
		Segments.push_back(AvoidanceSegment(bsm::Vec3(-cos(XM_PIDIV4) * Len, 0, -sin(XM_PIDIV4) * Len), bsm::Vec3(0, 0, 0)));
		Segments.push_back(AvoidanceSegment(bsm::Vec3(0, 0, -Len), bsm::Vec3(0, 0, 0)));
		for (size_t i = 0; i < Segments.size(); i++) {
			//触覚の各頂点に行列を計算して、ワールド座標に変換
			Segments[i].m_PointA *= Matrix;
			Segments[i].m_PointB *= Matrix;
			//線分と壁の衝突判定
			bsm::Vec3 RetVec;
			float t;
			for (size_t j = 0; j < PlaneVec.size(); j++) {
				if (HitTest::InsidePtPlane(Matrix.transInMatrix(), PlaneVec[j])) {
					return PlaneVec[j].m_Normal * MaxSpeed;
				}
				if (HitTest::SEGMENT_PLANE(Segments[i].m_PointA, Segments[i].m_PointB, PlaneVec[j], t, RetVec)) {
					//線分と面が衝突している
					//面の法線の方向に現在の速度でフォースを返す
					return PlaneVec[j].m_Normal * bsm::bsmUtil::length(Velocity);
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
		float DisSq = bsm::bsmUtil::lengthSqr(Dis);
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

	bsm::Vec3 Steering::Separation(const std::shared_ptr<GameObjectGroup>& Group, const std::shared_ptr<GameObject>& MyObj) {
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
					SteeringForce += bsm::bsmUtil::normalize(ToAgent) / bsm::bsmUtil::length(ToAgent);
				}
			}
		}
		return SteeringForce;
	}




	//--------------------------------------------------------------------------------------
	//	static bsm::Vec3 Alignment(
	//	const shared_ptr<GameObjectGroup>& Group,	//設定するグループの配列
	//	const shared_ptr<GameObject>& MyObj				//自分自身
	//	);
	//	用途: 整列行動
	//	戻り値: フォース
	//--------------------------------------------------------------------------------------
	bsm::Vec3 Steering::Alignment(const std::shared_ptr<GameObjectGroup>& Group, const std::shared_ptr<GameObject>& MyObj) {
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
	//	const shared_ptr<GameObjectGroup>& Group,	//設定するグループの配列
	//	const shared_ptr<GameObject>& MyObj			//自分自身
	//	const bsm::Vec3& Velocity,	//現在の速度
	//	float MaxSpeed				//最高速度
	//	);
	//	用途: 結合行動
	//	戻り値: フォース
	//--------------------------------------------------------------------------------------
	bsm::Vec3 Steering::Cohesion(const std::shared_ptr<GameObjectGroup>& Group, const std::shared_ptr<GameObject>& MyObj,
		const bsm::Vec3& Velocity, float MaxSpeed) {
		auto Vec = Group->GetGroupVector();
		bsm::Vec3 SteeringForce(0, 0, 0);
		//重心
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
/*!
@file RigidComp.cpp
@brief 物理演算コンポーネント　実体
*/


#pragma once
#include "stdafx.h"
#include "Project.h"

namespace basecross {

	using namespace std;
	using namespace bsm;

	//--------------------------------------------------------------------------------------
	///	 物理演算コンポーネント
	//--------------------------------------------------------------------------------------
	RigidComp::RigidComp(const shared_ptr<GameObject>& GameObjectPtr) :
		Component(GameObjectPtr)
	{
	}

	RigidComp::~RigidComp() {}


	void RigidComp::OnCreate() {
	}

	void RigidComp::OnUpdate(double elapsedTime) {
	}
	void RigidComp::OnDestroy() {
	}

	//--------------------------------------------------------------------------------------
	///	 スタティック物理演算コンポーネント
	//--------------------------------------------------------------------------------------
	RigidStaticComp::RigidStaticComp(const shared_ptr<GameObject>& GameObjectPtr,
		const PhysxCreateParam& pxParam) :
		RigidComp(GameObjectPtr),
		m_pxParam(pxParam)
	{
	}

	RigidStaticComp::~RigidStaticComp() {}


	void RigidStaticComp::OnCreate() {
		auto ptrGameObject = GetGameObject();
		auto ptrGameStage = dynamic_pointer_cast<GameStage>(ptrGameObject->GetStage());
		auto ptrPxPhysics = ptrGameStage->GetPxPhysics();
		//Transformコンポーネントを取り出す
		auto ptrTrans = ptrGameObject->GetComponent<Transform>();
		auto& param = ptrTrans->GetTransParam();
		auto pose = bsmUtil::ToPxTransform(param.position, param.quaternion);
		//physx::PxRigidStaticの作成
		m_pRigidStatic
			= ptrPxPhysics->createRigidStatic(
				pose
			);
		//shapeの作成
		physx::PxShape* shape
			= ptrPxPhysics->createShape(
				*m_pxParam.pGeometry,
				*ptrPxPhysics->createMaterial(
					m_pxParam.staticFriction,
					m_pxParam.dynamicFriction,
					m_pxParam.restitution
				)
			);
		shape->setLocalPose(m_pxParam.localPose);
		m_pRigidStatic->attachShape(*shape);
		ptrGameStage->GetPxScene()->addActor(*m_pRigidStatic);
	}

	void RigidStaticComp::OnUpdate(double elapsedTime) {
	}


	void RigidStaticComp::OnDestroy() {
		if (m_pRigidStatic) {
			m_pRigidStatic->release();
		}
	}

	//--------------------------------------------------------------------------------------
	///	 ダイナミック物理演算コンポーネント
	//--------------------------------------------------------------------------------------
	RigidDynamicComp::RigidDynamicComp(const shared_ptr<GameObject>& GameObjectPtr,
		const PhysxCreateParam& pxParam) :
		RigidComp(GameObjectPtr),
		m_pxParam(pxParam)
	{
	}

	RigidDynamicComp::~RigidDynamicComp() {}


	void RigidDynamicComp::OnCreate() {
		auto ptrGameObject = GetGameObject();
		auto ptrGameStage = dynamic_pointer_cast<GameStage>(ptrGameObject->GetStage());
		auto ptrPxPhysics = ptrGameStage->GetPxPhysics();
		//Transformコンポーネントを取り出す
		auto ptrTrans = ptrGameObject->GetComponent<Transform>();
		auto& param = ptrTrans->GetTransParam();
		auto pose = bsmUtil::ToPxTransform(param.position, param.quaternion);
		//physx::PxRigidDynamicの作成
		m_pRigidDynamic
			= ptrPxPhysics->createRigidDynamic(
				pose
			);
		//shapeの作成
		physx::PxShape* shape
			= ptrPxPhysics->createShape(
				*m_pxParam.pGeometry,
				*ptrPxPhysics->createMaterial(
					m_pxParam.staticFriction,
					m_pxParam.dynamicFriction,
					m_pxParam.restitution
				)
			);
		shape->setLocalPose(m_pxParam.localPose);
		m_pRigidDynamic->attachShape(*shape);
		ptrGameStage->GetPxScene()->addActor(*m_pRigidDynamic);
	}

	void RigidDynamicComp::OnUpdate(double elapsedTime) {
		auto ptrGameObject = GetGameObject();
		auto ptrGameStage = dynamic_pointer_cast<GameStage>(ptrGameObject->GetStage());
		auto ptrPxPhysics = ptrGameStage->GetPxPhysics();
		//Transformコンポーネントを取り出す
		auto ptrTrans = ptrGameObject->GetComponent<Transform>();
		auto& param = ptrTrans->GetTransParam();
		physx::PxTransform pose = m_pRigidDynamic->getGlobalPose();
		param.position = bsmUtil::ToVec3(pose.p);
		param.quaternion = bsmUtil::ToQuat(pose.q);

	}


	void RigidDynamicComp::OnDestroy() {
		if (m_pRigidDynamic) {
			m_pRigidDynamic->release();
		}
	}


}
// end namespace basecross

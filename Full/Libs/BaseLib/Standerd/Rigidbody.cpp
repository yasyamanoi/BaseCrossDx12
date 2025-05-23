/*!
@file Rigidbody.cpp
@brief 物理演算コンポーネント　実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#pragma once
#include "stdafx.h"
//#include "Project.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	///	 物理演算コンポーネント
	//--------------------------------------------------------------------------------------
	Rigidbody::Rigidbody(const std::shared_ptr<GameObject>& GameObjectPtr) :
		Component(GameObjectPtr)
	{
	}

	Rigidbody::~Rigidbody() {}


	void Rigidbody::OnCreate() {
	}

	void Rigidbody::OnUpdate(double elapsedTime) {
	}
	void Rigidbody::OnDestroy() {
	}

	//--------------------------------------------------------------------------------------
	///	 スタティック物理演算コンポーネント
	//--------------------------------------------------------------------------------------
	RigidbodyStatic::RigidbodyStatic(const std::shared_ptr<GameObject>& GameObjectPtr,
		const PhysxCreateParam& pxParam) :
		Rigidbody(GameObjectPtr),
		m_pxParam(pxParam)
	{
	}

	RigidbodyStatic::~RigidbodyStatic() {}


	void RigidbodyStatic::OnCreate() {
		auto ptrGameObject = GetGameObject();
		auto ptrStage = ptrGameObject->GetStage();
		auto ptrPxPhysics = ptrStage->GetPxPhysics();
		//Transformコンポーネントを取り出す
		auto ptrTrans = ptrGameObject->GetComponent<Transform>();
		auto& param = ptrTrans->GetTransParam();
		auto pose = bsm::bsmUtil::ToPxTransform(param.position, param.quaternion);
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
		ptrStage->GetPxScene()->addActor(*m_pRigidStatic);
		shape->release();
	}

	void RigidbodyStatic::OnUpdate(double elapsedTime) {
	}


	void RigidbodyStatic::OnDestroy() {
		if (m_pRigidStatic) {
			m_pRigidStatic->release();
		}
	}

	//--------------------------------------------------------------------------------------
	///	 ダイナミック物理演算コンポーネント
	//--------------------------------------------------------------------------------------
	RigidbodyDynamic::RigidbodyDynamic(const std::shared_ptr<GameObject>& GameObjectPtr,
		const PhysxCreateParam& pxParam) :
		Rigidbody(GameObjectPtr),
		m_pxParam(pxParam)
	{
	}

	RigidbodyDynamic::~RigidbodyDynamic() {}


	void RigidbodyDynamic::OnCreate() {
		auto ptrGameObject = GetGameObject();
		auto ptrStage = ptrGameObject->GetStage();
		auto ptrPxPhysics = ptrStage->GetPxPhysics();
		//Transformコンポーネントを取り出す
		auto ptrTrans = ptrGameObject->GetComponent<Transform>();
		auto& param = ptrTrans->GetTransParam();
		auto pose = bsm::bsmUtil::ToPxTransform(param.position, param.quaternion);
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
		ptrStage->GetPxScene()->addActor(*m_pRigidDynamic);
		shape->release();

	}

	void RigidbodyDynamic::OnUpdate(double elapsedTime) {
		auto ptrGameObject = GetGameObject();
		auto ptrStage = ptrGameObject->GetStage();
		auto ptrPxPhysics = ptrStage->GetPxPhysics();
		//Transformコンポーネントを取り出す
		auto ptrTrans = ptrGameObject->GetComponent<Transform>();
		auto& param = ptrTrans->GetTransParam();
		physx::PxTransform pose = m_pRigidDynamic->getGlobalPose();
		param.position = bsm::bsmUtil::ToVec3(pose.p);
		param.quaternion = bsm::bsmUtil::ToQuat(pose.q);

	}


	void RigidbodyDynamic::OnDestroy() {
		if (m_pRigidDynamic) {
			m_pRigidDynamic->release();
		}
	}


}
// end namespace basecross

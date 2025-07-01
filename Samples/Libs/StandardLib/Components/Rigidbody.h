/*!
@file Rigidbody.h
@brief 物理演算コンポーネント
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	physxの構築パラメータ
	//--------------------------------------------------------------------------------------
	struct PhysxCreateParam {
		physx::PxGeometry* pGeometry;
		physx::PxReal staticFriction;
		physx::PxReal dynamicFriction;
		physx::PxReal restitution;
		physx::PxTransform localPose;
		PhysxCreateParam() :
			pGeometry(nullptr),
			staticFriction(0.5f),
			dynamicFriction(0.5f),
			restitution(0.5f),
			localPose(physx::PxIdentity)
		{
		}
	};


	//--------------------------------------------------------------------------------------
	///	 物理演算コンポーネント（親）
	//--------------------------------------------------------------------------------------
	class Rigidbody : public Component {
	protected:
		explicit Rigidbody(const std::shared_ptr<GameObject>& GameObjectPtr);
		virtual ~Rigidbody();
	public:
		virtual void OnCreate()override;
		virtual void OnUpdate(double elapsedTime)override;
		virtual void OnDestroy()override;
	};

	//--------------------------------------------------------------------------------------
	///	 スタティック物理演算コンポーネント
	//--------------------------------------------------------------------------------------
	class RigidbodyStatic : public Rigidbody {
	protected:
		physx::PxRigidStatic* m_pRigidStatic = nullptr;
		PhysxCreateParam m_pxParam;
	public:
		RigidbodyStatic(const std::shared_ptr<GameObject>& GameObjectPtr,
			const PhysxCreateParam& pxParam);
		virtual ~RigidbodyStatic();
		virtual void OnCreate()override;
		virtual void OnUpdate(double elapsedTime)override;
		virtual void OnDestroy()override;
	};

	//--------------------------------------------------------------------------------------
	///	 ダイナミック物理演算コンポーネント
	//--------------------------------------------------------------------------------------
	class RigidbodyDynamic : public Rigidbody {
	protected:
		physx::PxRigidDynamic* m_pRigidDynamic = nullptr;
		PhysxCreateParam m_pxParam;
	public:
		explicit RigidbodyDynamic(const std::shared_ptr<GameObject>& GameObjectPtr,
			const PhysxCreateParam& pxParam);
		virtual ~RigidbodyDynamic();
		physx::PxRigidDynamic* GetRigidDynamic() {
			return m_pRigidDynamic;
		}
		virtual void OnCreate()override;
		virtual void OnUpdate(double elapsedTime)override;
		virtual void OnDestroy()override;
	};

}
// end namespace basecross

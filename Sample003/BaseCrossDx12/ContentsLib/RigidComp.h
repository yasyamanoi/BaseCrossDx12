/*!
@file RigidComp.h
@brief 物理演算コンポーネント
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	using namespace std;
	using namespace bsm;

	//--------------------------------------------------------------------------------------
	///	physxの構築パラメータ
	//--------------------------------------------------------------------------------------
	struct PhysxCreateParam {
		physx::PxGeometry* pGeometry;
		physx::PxReal staticFriction;
		physx::PxReal dynamicFriction;
		physx::PxReal restitution;
		physx::PxTransform localPose;
		PhysxCreateParam():
			pGeometry(nullptr),
			staticFriction(0.5f),
			dynamicFriction(0.5f),
			restitution(0.5f),
			localPose(physx::PxIdentity)
		{}
	};


	//--------------------------------------------------------------------------------------
	///	 物理演算コンポーネント（親）
	//--------------------------------------------------------------------------------------
	class RigidComp : public Component {
	protected:
		explicit RigidComp(const shared_ptr<GameObject>& GameObjectPtr);
		virtual ~RigidComp();
	public:
		virtual void OnCreate()override;
		virtual void OnUpdate(double elapsedTime)override;
		virtual void OnDestroy()override;
	};

	//--------------------------------------------------------------------------------------
	///	 スタティック物理演算コンポーネント
	//--------------------------------------------------------------------------------------
	class RigidStaticComp : public RigidComp {
	protected:
		physx::PxRigidStatic* m_pRigidStatic = nullptr;
		PhysxCreateParam m_pxParam;
	public:
		RigidStaticComp(const shared_ptr<GameObject>& GameObjectPtr,
			const PhysxCreateParam& pxParam);
		virtual ~RigidStaticComp();
		virtual void OnCreate()override;
		virtual void OnUpdate(double elapsedTime)override;
		virtual void OnDestroy()override;
	};

	//--------------------------------------------------------------------------------------
	///	 ダイナミック物理演算コンポーネント
	//--------------------------------------------------------------------------------------
	class RigidDynamicComp : public RigidComp {
	protected:
		physx::PxRigidDynamic* m_pRigidDynamic = nullptr;
		PhysxCreateParam m_pxParam;
	public:
		explicit RigidDynamicComp(const shared_ptr<GameObject>& GameObjectPtr,
			const PhysxCreateParam& pxParam);
		virtual ~RigidDynamicComp();
		physx::PxRigidDynamic* GetRigidDynamic(){
			return m_pRigidDynamic;
		}
		virtual void OnCreate()override;
		virtual void OnUpdate(double elapsedTime)override;
		virtual void OnDestroy()override;
	};

}
// end namespace basecross

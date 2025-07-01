/*!
@file Rigidbody.h
@brief �������Z�R���|�[�l���g
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	physx�̍\�z�p�����[�^
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
	///	 �������Z�R���|�[�l���g�i�e�j
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
	///	 �X�^�e�B�b�N�������Z�R���|�[�l���g
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
	///	 �_�C�i�~�b�N�������Z�R���|�[�l���g
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

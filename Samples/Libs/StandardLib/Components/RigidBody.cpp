
/*!
@file RigidBody.cpp
@brief 重力コンポーネント　実体
*/
#include "stdafx.h"


#include "stdafx.h"
#include "RigidBody.h"
#include "JoltManager.h"

namespace basecross
{
	RigidBody::RigidBody(const std::shared_ptr<GameObject>& owner)
		: Component(owner),
		m_bodyID(JPH::BodyID::cInvalidBodyID)
	{
	}

	void RigidBody::Initialize(const Settings& settings)
	{
		m_settings = settings;

		m_pPhysicsSystem = JoltManager::GetActiveSystem();
		if (!m_pPhysicsSystem) return;

		JPH::BodyInterface& bodyInterface = m_pPhysicsSystem->GetBodyInterface();

		// 1. 同期元のTransformを取得
		auto transComp = GetGameObject()->GetComponent<Transform>();
		if (!transComp) return;

		Vec3 pos = transComp->GetPosition();
		Quat q = transComp->GetQuaternion();

		// 2. DX11の座標系(DirectXMath)からJoltの座標系(JPH)へ変換
		JPH::Vec3 joltPos(pos.x, pos.y, pos.z);
		JPH::Quat joltRot(q.x, q.y, q.z, q.w);

		// 3. JoltのBodyを作成
		JPH::BodyCreationSettings creationSettings(
			m_settings.shape,
			joltPos,
			joltRot,
			m_settings.motionType,
			m_settings.objectLayer
		);
		creationSettings.mRestitution = m_settings.restitution;
		creationSettings.mFriction = m_settings.friction;

		// 質量の手動上書き（Shapeから自動計算も可能）
		if (m_settings.motionType == JPH::EMotionType::Dynamic) {
			creationSettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
			creationSettings.mMassPropertiesOverride.mMass = m_settings.mass;
		}

		// 4. 物理世界へ追加
		m_bodyID = bodyInterface.CreateAndAddBody(
			creationSettings,
			m_settings.motionType == JPH::EMotionType::Dynamic ? JPH::EActivation::Activate : JPH::EActivation::DontActivate
		);
	}

	void RigidBody::OnCreate()
	{
	}

	void RigidBody::OnUpdate(double elapsedTime)
	{
		if (!m_pPhysicsSystem || m_bodyID.IsInvalid()) return;

		JPH::BodyInterface& bodyInterface = m_pPhysicsSystem->GetBodyInterface();

		// 静的(Static)なオブジェクトは動かないので同期をスキップして最適化
		if (m_settings.motionType == JPH::EMotionType::Static) return;

		// 物理計算の結果、Bodyが動いていればTransformを更新する
		if (bodyInterface.IsActive(m_bodyID)) {
			JPH::RVec3 joltPos = bodyInterface.GetCenterOfMassPosition(m_bodyID);
			JPH::Quat joltRot = bodyInterface.GetRotation(m_bodyID);

			auto pTransform = GetGameObject()->GetComponent<Transform>();
			if (pTransform) {
				// Jolt(物理結果) -> エンジン(Transform)への書き戻し
				pTransform->SetPosition(joltPos.GetX(), joltPos.GetY(), joltPos.GetZ());
				pTransform->SetQuaternion(Quat(joltRot.GetX(), joltRot.GetY(), joltRot.GetZ(), joltRot.GetW()));
			}
		}
	}


	RigidBody::~RigidBody()
	{
		m_pPhysicsSystem = JoltManager::GetActiveSystem(); // このタイミングではPhysicsSystemが終了している場合があるので最新のものを取得する
		if (!m_pPhysicsSystem || m_bodyID.IsInvalid()) return;

		JPH::BodyInterface& bodyInterface = m_pPhysicsSystem->GetBodyInterface();

		// 必ずRemoveしてからDestroyする（Joltの厳格なルール）
		bodyInterface.RemoveBody(m_bodyID);
		bodyInterface.DestroyBody(m_bodyID);

		m_bodyID = JPH::BodyID(JPH::BodyID::cInvalidBodyID);
	}

	void RigidBody::AddForce(const DirectX::XMFLOAT3& force)
	{
		if (!m_pPhysicsSystem || m_bodyID.IsInvalid()) return;
		m_pPhysicsSystem->GetBodyInterface().AddForce(m_bodyID, JPH::Vec3(force.x, force.y, force.z));
	}

	void RigidBody::SetLinearVelocity(const DirectX::XMFLOAT3& velocity)
	{
		if (!m_pPhysicsSystem || m_bodyID.IsInvalid()) return;
		m_pPhysicsSystem->GetBodyInterface().SetLinearVelocity(m_bodyID, JPH::Vec3(velocity.x, velocity.y, velocity.z));
	}
}
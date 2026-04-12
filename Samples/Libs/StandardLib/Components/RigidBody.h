/*!
@file RigidBody.h
@brief 物理演算コンポーネント
*/
#pragma once
#include "stdafx.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>

#include "JoltLayers.h"

namespace basecross
{
    class RigidBody : public Component
    {
    public:
        struct Settings {
            JPH::ShapeRefC shape;
            JPH::EMotionType motionType = JPH::EMotionType::Dynamic;
            JPH::ObjectLayer objectLayer = Layers::MOVING;
            float mass = 1.0f;
            float restitution = 0.5f;
            float friction = 0.5f;
        };

        explicit RigidBody(const std::shared_ptr<GameObject>& owner);

        void Initialize(const Settings& settings);

        virtual void OnCreate() override;
        virtual void OnUpdate(double elapsedTime) override;
        virtual ~RigidBody();

        void AddForce(const DirectX::XMFLOAT3& force);
        void SetLinearVelocity(const DirectX::XMFLOAT3& velocity);

    private:
        JPH::BodyID m_bodyID;
        Settings m_settings;
        JPH::PhysicsSystem* m_pPhysicsSystem = nullptr;
    };
}
//end  namespace basecross

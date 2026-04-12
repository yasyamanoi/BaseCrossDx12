#pragma once
#include "stdafx.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>

/**
 * オブジェクトレイヤーの定義
 * 剛体がどのグループに属するかを決定します。
 */
namespace Layers {
    static constexpr JPH::ObjectLayer NON_MOVING = 0; // 動かないオブジェクト（床、壁など）
    static constexpr JPH::ObjectLayer MOVING = 1;     // 動くオブジェクト（プレイヤー、箱など）
    static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
}

/**
 * ブロードフェーズレイヤーの定義
 * 衝突判定の第一段階（大まかな判定）で使用されるグループ化です。
 */
namespace BroadPhaseLayers {
    static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
    static constexpr JPH::BroadPhaseLayer MOVING(1);
    static constexpr JPH::uint NUM_LAYERS = 2;
}

/**
 * ObjectLayer と BroadPhaseLayer を紐付けるクラス
 */
class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface {
public:
    BPLayerInterfaceImpl() {
        mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
        mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
    }

    virtual JPH::uint GetNumBroadPhaseLayers() const override {
        return BroadPhaseLayers::NUM_LAYERS;
    }

    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override {
        JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
        return mObjectToBroadPhase[inLayer];
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override {
        switch ((JPH::BroadPhaseLayer::Type)inLayer) {
        case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING: return "NON_MOVING";
        case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:     return "MOVING";
        default: JPH_ASSERT(false); return "INVALID";
        }
    }
#endif

private:
    JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

/**
 * オブジェクトレイヤーとブロードフェーズレイヤーの衝突フィルタ
 * どのレイヤー同士が大まかに衝突すべきかを定義します。
 */
class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter {
public:
    virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override {
        switch (inLayer1) {
        case Layers::NON_MOVING:
            return inLayer2 == BroadPhaseLayers::MOVING; // 静止物は動く物とだけ判定
        case Layers::MOVING:
            return true; // 動く物は全て（静止物・動く物）と判定
        default:
            JPH_ASSERT(false);
            return false;
        }
    }
};

/**
 * オブジェクトレイヤー同士の衝突フィルタ
 * 最終的な衝突判定を行うかどうかを定義します。
 */
class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter {
public:
    virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override {
        switch (inObject1) {
        case Layers::NON_MOVING:
            return inObject2 == Layers::MOVING; // 静止物 vs 動く物 のみ判定
        case Layers::MOVING:
            return true; // 動く物 vs 全て を判定
        default:
            JPH_ASSERT(false);
            return false;
        }
    }
};
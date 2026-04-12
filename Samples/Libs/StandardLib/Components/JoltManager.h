#pragma once

#include "stdafx.h"

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <thread>
#include <memory>

// レイヤー定義の前方宣言（詳細は PhysicsLayers.h にて）
class BPLayerInterfaceImpl;
class ObjectVsBroadPhaseLayerFilterImpl;
class ObjectLayerPairFilterImpl;



/**
 * 物理システムを統合管理するクラス
 * * 使用方法:
 * 1. アプリ起動時に PhysicsManager::StaticInitialize() を呼ぶ
 * 2. 各シーンクラスで PhysicsManager m_physics; を持ち、m_physics.Initialize() を呼ぶ
 * 3. コンポーネントからは PhysicsManager::GetActiveSystem() でアクセス可能
 * 4. アプリ終了時に PhysicsManager::StaticTerminate() を呼ぶ
 */
class JoltManager {
public:
    JoltManager();
    ~JoltManager();

    // コピー禁止
    JoltManager(const JoltManager&) = delete;
    JoltManager& operator=(const JoltManager&) = delete;

    // --- Global Static Methods (アプリ全体で1回) ---
    static void StaticInitialize();
    static void StaticTerminate();
    static JPH::PhysicsSystem* GetActiveSystem();

    // --- Instance Methods (シーンごと) ---
    bool Initialize();
    void Update(float deltaTime);

    JPH::PhysicsSystem& GetSystem() { return m_physicsSystem; }
    JPH::BodyInterface& GetBodyInterface() { return m_physicsSystem.GetBodyInterface(); }
	JPH::TempAllocator* GetTempAllocator() { return m_tempAllocator.get(); }

private:
    // 現在アクティブな（更新されている）マネージャーへのポインタ
    static JoltManager* s_activeManager;

    // Jolt 物理システム本体
    JPH::PhysicsSystem m_physicsSystem;

    // 実行リソース
    std::unique_ptr<JPH::TempAllocatorImpl> m_tempAllocator;
    std::unique_ptr<JPH::JobSystemThreadPool> m_jobSystem;

    // レイヤーフィルター関連（実装クラスを隠蔽するため unique_ptr で保持）
    std::unique_ptr<BPLayerInterfaceImpl> m_bpLayerInterface;
    std::unique_ptr<ObjectVsBroadPhaseLayerFilterImpl> m_objVsBpFilter;
    std::unique_ptr<ObjectLayerPairFilterImpl> m_objVsObjFilter;
};



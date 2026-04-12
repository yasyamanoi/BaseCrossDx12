#include "stdafx.h"
#include "JoltManager.h"
#include "JoltLayers.h"

#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

#include <iostream>
#include <cstdarg>


// -----------------------------------------------------------------------
// JPH_DISABLE_CUSTOM_ALLOCATOR を定義した場合の必須実装
// -----------------------------------------------------------------------
// Jolt Physics がメモリ確保を行う際に、以下の関数が呼び出されます。
// 標準の malloc/free を割り当てることで解決します。
/*
namespace JPH {
    void* Allocate(size_t inSize) {
        return malloc(inSize);
    }

    void Free(void* inBlock) {
        free(inBlock);
    }

    void* AlignedAllocate(size_t inSize, size_t inAlignment) {
        return _aligned_malloc(inSize, inAlignment);
    }

    void AlignedFree(void* inBlock) {
        _aligned_free(inBlock);
    }
}
*/

// Joltのトレースコールバック関数
static void TraceImpl(const char* inFmt, ...)
{
    va_list list;
    va_start(list, inFmt);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), inFmt, list);
    va_end(list);

    // デバッグ出力（必要に応じてログシステムに変更可能）
    OutputDebugStringA(buffer);
    OutputDebugStringA("\n");
}

#ifdef JPH_ENABLE_ASSERTS
// Joltのアサートコールバック関数
static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, JPH::uint inLine)
{
    // エラー情報を出力
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "Jolt Assert Failed: %s:%u: (%s) %s\n",
        inFile, inLine, inExpression, inMessage ? inMessage : "");
    OutputDebugStringA(buffer);

    // true を返すとデバッガへブレーク
    return true;
}
#endif

// 静的メンバの定義
JoltManager* JoltManager::s_activeManager = nullptr;

// コンストラクタ
JoltManager::JoltManager()
{
}

// デストラクタ
JoltManager::~JoltManager()
{
    // アクティブマネージャーをクリア
    if (s_activeManager == this)
    {
        s_activeManager = nullptr;
    }
}

// 静的初期化（アプリ起動時に1回だけ呼び出す）
void JoltManager::StaticInitialize()
{
    JPH::RegisterDefaultAllocator();

    // Joltのトレースとアサートを登録
    JPH::Trace = TraceImpl;
#ifdef JPH_ENABLE_ASSERTS
    JPH::AssertFailed = AssertFailedImpl;
#endif

    // Jolt Factory の作成（型のシリアライズ等に必要）
    JPH::Factory::sInstance = new JPH::Factory();

    // 全てのJolt物理型を登録
    JPH::RegisterTypes();
}

// 静的終了処理（アプリ終了時に1回だけ呼び出す）
void JoltManager::StaticTerminate()
{
    // 型の登録解除
    JPH::UnregisterTypes();

    // Factory を破棄
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
}

// アクティブなPhysicsSystemを取得
JPH::PhysicsSystem* JoltManager::GetActiveSystem()
{
    if (s_activeManager != nullptr)
    {
        return &s_activeManager->m_physicsSystem;
    }
    return nullptr;
}

// インスタンス初期化（シーンごとに呼び出す）
bool JoltManager::Initialize()
{
    // テンポラリアロケータの作成（シミュレーション中の一時メモリ用）
    // 10MB を確保（必要に応じて調整）
    m_tempAllocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);

    // ジョブシステムの作成（マルチスレッド処理用）
    // 最大ジョブ数、最大バリア数、使用スレッド数を指定
    int numThreads = max(1, static_cast<int>(std::thread::hardware_concurrency()) - 1);
    m_jobSystem = std::make_unique<JPH::JobSystemThreadPool>(
        JPH::cMaxPhysicsJobs,
        JPH::cMaxPhysicsBarriers,
        numThreads
    );

    // レイヤーインターフェースの作成
    m_bpLayerInterface = std::make_unique<BPLayerInterfaceImpl>();
    m_objVsBpFilter = std::make_unique<ObjectVsBroadPhaseLayerFilterImpl>();
    m_objVsObjFilter = std::make_unique<ObjectLayerPairFilterImpl>();

    // 物理システムの初期化
    // パラメータ: 最大ボディ数、ボディミューテックス数、最大ボディペア数、最大接触制約数
    const JPH::uint cMaxBodies = 65536;
    const JPH::uint cNumBodyMutexes = 0; // デフォルト値を使用
    const JPH::uint cMaxBodyPairs = 65536;
    const JPH::uint cMaxContactConstraints = 10240;

    m_physicsSystem.Init(
        cMaxBodies,
        cNumBodyMutexes,
        cMaxBodyPairs,
        cMaxContactConstraints,
        *m_bpLayerInterface,
        *m_objVsBpFilter,
        *m_objVsObjFilter
    );

    // 重力の設定（デフォルトは Y軸下向き -9.81）
    m_physicsSystem.SetGravity(JPH::Vec3(0.0f, -9.81f, 0.0f));

    // このマネージャーをアクティブとして登録
    s_activeManager = this;

    return true;
}

// 物理シミュレーションの更新
void JoltManager::Update(float deltaTime)
{
    // deltaTime が 0 以下なら更新しない
    if (deltaTime <= 0.0f)
    {
        return;
    }

    // 衝突ステップ数（通常は1で十分、高速移動オブジェクトがある場合は増やす）
    const int cCollisionSteps = 1;

    // 物理システムの更新
    m_physicsSystem.Update(
        deltaTime,
        cCollisionSteps,
        m_tempAllocator.get(),
        m_jobSystem.get()
    );
}
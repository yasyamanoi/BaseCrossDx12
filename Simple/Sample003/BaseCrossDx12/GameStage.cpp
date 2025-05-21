/*!
@file GameStage.cpp
@brief ゲームステージクラス　実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	using namespace std;
	using namespace basecross::bsm;


	GameStage::~GameStage() {

		PxCloseExtensions();
		m_pScene->release();
		m_pDispatcher->release();
		m_pPhysics->release();
		m_pFoundation->release();
	}


	void GameStage::OnCreate() {
		m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_defaultAllocator, m_defaultErrorCallback);
		if (!m_pFoundation) {
			return;
		}
		m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, physx::PxTolerancesScale(), true);
		if(!m_pPhysics){
			return;
		}
		m_pDispatcher = physx::PxDefaultCpuDispatcherCreate(8);
		physx::PxSceneDesc scene_desc(m_pPhysics->getTolerancesScale());
		scene_desc.gravity = physx::PxVec3(0, -9, 0);
		scene_desc.filterShader = physx::PxDefaultSimulationFilterShader;
		scene_desc.cpuDispatcher = m_pDispatcher;
		m_pScene = m_pPhysics->createScene(scene_desc);
		//カメラとライト
		m_myCamera = shared_ptr<MyCamera>(new MyCamera());
		m_myCamera->SetEye(Vec3(0, 3.43f, -6.37f));
		m_myCamera->SetAt(Vec3(0, 0.125f, 0));
		m_myLightSet = shared_ptr<LightSet>(new LightSet());
		//コントローラー
		m_inputDevice.ResetControlerState();


		TransParam param;

		param.quaternion = XMQuaternionIdentity();
		param.scale = Vec3(50.0f, 1.0f, 50.0f);
		param.position = Vec3(0.0f, -0.5, 0.0f);
		AddGameObject<SkyGround>(param);



		param.scale = Vec3(1.0f, 1.0f, 1.0f);
		param.rotOrigin = Vec3(0.0f, 0.0f, 0.0f);
		Quat quat(Vec3(1,1,0),1.0f);
		param.quaternion = quat;
		param.position = Vec3(-4.0f, 5.0f, 5.0f);
		AddGameObject<WallBox>(param);

		quat = Quat(Vec3(0, 1, 1), 2.0);
		param.quaternion = quat;
		param.position = Vec3(-2.0f, 5.0f, 5.0f);
		AddGameObject<WallBox>(param);

		quat = Quat(Vec3(1, 0, 1), 1.5);
		param.quaternion = quat;
		param.position = Vec3(0.0f, 5.0f, 5.0f);
		AddGameObject<WallBox>(param);

		quat = Quat(Vec3(1, 1, 1), 1.0);
		param.quaternion = quat;
		param.position = Vec3(2.0f, 5.0f, 5.0f);
		AddGameObject<WallBox>(param);

		quat = Quat(Vec3(1, 0, 1), 0.5);
		param.quaternion = quat;
		param.position = Vec3(4.0f, 5.0f, 5.0f);
		AddGameObject<WallBox>(param);

		param.quaternion = XMQuaternionIdentity();
		param.scale = Vec3(0.25f, 0.25f, 0.25f);
		param.position = Vec3(0.0f, 0.125f, 0.0f);
		AddGameObject<Player>(param);


	}

	void GameStage::OnUpdate(double elapsedTime) {
		// シミュレーション速度を指定する
		m_pScene->simulate((float)elapsedTime);
		// PhysXの処理が終わるまで待つ
		m_pScene->fetchResults(true);
		Stage::OnUpdate(elapsedTime);
		m_myCamera->OnUpdate(elapsedTime);
	}

	void GameStage::OnDestroy() {
		Stage::OnDestroy();
	}


}
// end namespace basecross

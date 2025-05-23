/*!
@file GameStage.h
@brief ゲームステージクラス
*/


#pragma once


#pragma once
#include "stdafx.h"


namespace basecross {

	//--------------------------------------------------------------------------------------
	// 配置されるオブジェクトの親
	//--------------------------------------------------------------------------------------
	class GameStage : public Stage {
	protected:
		std::shared_ptr<MyCamera> m_myCamera;
		std::shared_ptr<LightSet> m_myLightSet;
		//コントローラ
		InputDevice m_inputDevice;

		// PhysX内で利用するアロケーター
		physx::PxDefaultAllocator m_defaultAllocator;
		// エラー時用のコールバックでエラー内容が入ってる
		physx::PxDefaultErrorCallback m_defaultErrorCallback;
		// 上位レベルのSDK(PxPhysicsなど)をインスタンス化する際に必要
		physx::PxFoundation* m_pFoundation = nullptr;
		// 実際に物理演算を行う
		physx::PxPhysics* m_pPhysics = nullptr;
		// シミュレーションをどう処理するかの設定でマルチスレッドの設定もできる
		physx::PxDefaultCpuDispatcher* m_pDispatcher = nullptr;
		// シミュレーションする空間の単位でActorの追加などもここで行う
		physx::PxScene* m_pScene = nullptr;

	public:
		GameStage(ID3D12Device* pDevice):
			Stage(pDevice)
		{}
		virtual ~GameStage();
		//アクセサ
		std::shared_ptr<MyCamera> GetMyCamera() const{
			return m_myCamera;
		}
		physx::PxPhysics* GetPxPhysics() {
			return m_pPhysics;
		}
		physx::PxScene* GetPxScene() {
			return m_pScene;
		}
		std::shared_ptr<LightSet> GetLightSet() const {
			return m_myLightSet;
		}
		virtual void OnCreate()override;
		virtual void OnUpdate(double elapsedTime)override;

		virtual void OnDestroy()override;
	};



}
//end namespace basecross

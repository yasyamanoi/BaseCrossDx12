/*!
@file Stage.h
@brief 親ステージクラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#pragma once

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	// 親ステージクラス
	//--------------------------------------------------------------------------------------
	class Stage : public ObjectInterface {
	protected:
		ID3D12Device* m_pDevice;
		std::vector<std::shared_ptr<GameObject>> m_gameObjectvec;
		Stage(ID3D12Device* pDevice) :
			m_pDevice(pDevice)
		{}
		virtual ~Stage() {}
	public:
		template<typename T, typename... Ts>
		std::shared_ptr<T> AddGameObject(Ts&&... params) {
			try {
				auto ptr = ObjectFactory::Create<T>(GetThis<Stage>(), params...);
				m_gameObjectvec.push_back(ptr);
				return ptr;
			}
			catch (...) {
				throw;
			}
		}

		virtual std::shared_ptr<Camera> GetTargetCamera() const = 0;
		virtual std::shared_ptr<LightSet> GetTargetLightSet() const = 0;

		virtual physx::PxPhysics* GetPxPhysics() = 0;
		virtual physx::PxScene* GetPxScene() = 0;


		virtual void OnUpdateConstantBuffers();
		virtual void OnCommitConstantBuffers();
		virtual void SetToBefore();

		virtual void OnCreate()override {}
		virtual void OnUpdate(double elapsedTime)override;
		virtual void OnUpdate2(double elapsedTime)override;
		virtual void OnShadowDraw()override;
		virtual void OnSceneDraw()override;
		virtual void OnDestroy()override;


	};



}
//end namespace basecross


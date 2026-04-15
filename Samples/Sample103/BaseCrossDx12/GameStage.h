/*!
@file GameStage.h
@brief ゲームステージクラス
*/


#pragma once


#pragma once
#include "stdafx.h"


namespace basecross {
	//--------------------------------------------------------------------------------------
	// ゲームステージ
	//--------------------------------------------------------------------------------------
	class GameStage : public Stage {
		std::shared_ptr<Camera> m_camera;
		std::shared_ptr<LightSet> m_lightSet;
	public:
		GameStage(ID3D12Device* pDevice) :
			Stage(pDevice)
		{
		}
		virtual ~GameStage() {}
		virtual void OnCreate()override;
		virtual std::shared_ptr<Camera> GetCamera() const override {
			return m_camera;
		}
		virtual std::shared_ptr<LightSet> GetLightSet() const override {
			return m_lightSet;
		}
		virtual void UpdateCameraLight(double elapsedTime)override {
			//カメラとライト
			m_camera->OnUpdate(elapsedTime);
			m_lightSet->OnUpdate(elapsedTime);
		}
		virtual void OnUpdate2(double elapsedTime)override;
	};





}
//end namespace basecross

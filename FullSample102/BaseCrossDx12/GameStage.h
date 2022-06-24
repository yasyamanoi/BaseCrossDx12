/*!
@file GameStage.h
@brief ゲームステージクラス
*/
#pragma once
#include "stdafx.h"

namespace basecross {

	class GameStage : public Stage {
		//カメラ
		shared_ptr<BaseCamera> m_camera;
		//ライト
		shared_ptr<LightSet> m_lightSet;
		//物理計算オブジェクトの作成
		void CreatePhysicsObjects();
		//プレイヤーの作成
		void CreatePlayer();

	public:
		GameStage() :Stage() {}
		virtual ~GameStage() {}
		virtual shared_ptr<BaseCamera> GetActiveCamera() const override {
			return m_camera;
		}
		virtual shared_ptr<LightSet> GetActiveLightSet() const override {
			return m_lightSet;
		}
		virtual void OnCreate()override;
		virtual void OnUpdateLightCamera() override {
			m_camera->OnUpdate();
		}

		//更新
		virtual void OnUpdate()override;


	};



}
//end namespace basecross

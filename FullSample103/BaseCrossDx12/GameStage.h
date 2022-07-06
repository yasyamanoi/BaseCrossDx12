/*!
@file GameStage.h
@brief ゲームステージクラス
*/
#pragma once
#include "stdafx.h"

namespace basecross {

	class GameStage : public Stage {
		//カメラ
		shared_ptr<MyCamera> m_camera;
		//ライト
		shared_ptr<LightSet> m_lightSet;
		//固定のボックスの作成
		void CreateFixedBox();
		//敵の作成
		void CreateEnemy();
		//プレイヤーの作成
		void CreatePlayer();
	public:
		GameStage() :Stage() {}
		virtual ~GameStage() {}

		virtual shared_ptr<Camera> GetActiveCamera() const override {
			return m_camera;
		}
		virtual shared_ptr<LightSet> GetActiveLightSet() const override {
			return m_lightSet;
		}
		virtual void OnCreate()override;
		virtual void OnUpdateLightCamera() override {
			m_camera->OnUpdate();
			m_lightSet->OnUpdate();
		}

	};

}
//end namespace basecross

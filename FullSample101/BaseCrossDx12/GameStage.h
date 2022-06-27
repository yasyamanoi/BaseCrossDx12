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
		//固定の球体の作成
		void CreateFixedSphere();
		//固定のカプセルの作成
		void CreateFixedCapsule();
		//行ったり来たりするボックスの作成
		void CreateMoveFixedBox();
		//移動するボックスの作成
		void CreateMoveBox();
		//追いかけるオブジェクトの作成
		void CreateSeekObject();
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
		}

	};

}
//end namespace basecross

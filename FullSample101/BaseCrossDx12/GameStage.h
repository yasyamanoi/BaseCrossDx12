/*!
@file GameStage.h
@brief ゲームステージクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/
#pragma once
#include "stdafx.h"

namespace basecross {

	class GameStage : public Stage {
		//カメラ
		shared_ptr<BaseCamera> m_camera;
		//ライト
		shared_ptr<LightSet> m_lightSet;
		//固定のボックスの作成
		void CreateFixedBox();
		//トータル時間
		float m_totalTime;
	public:
		GameStage() :Stage() {}
		virtual ~GameStage() {}
		virtual shared_ptr<BaseCamera> GetActiveCamera() const override {
			return m_camera;
		}
		virtual shared_ptr<LightSet> GetActiveLightSet() const override {
			return m_lightSet;
		}
		virtual void OnInit()override;
		virtual void OnUpdate()override;

	};

}
//end namespace basecross

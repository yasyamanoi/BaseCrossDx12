/*!
@file GameStage.h
@brief ゲームステージクラス
*/


#pragma once
#include "stdafx.h"


namespace basecross {

	//--------------------------------------------------------------------------------------
	// タイトルステージ
	//--------------------------------------------------------------------------------------
	class TitleStage : public Stage {
		std::shared_ptr<Camera> m_camera;
		std::shared_ptr<LightSet> m_lightSet;
		//入力ハンドラー
		InputHandler<TitleStage> m_InputHandler;

	public:
		TitleStage(ID3D12Device* pDevice) :
			Stage(pDevice)
		{}
		virtual ~TitleStage() {}
		//構築時処理
		virtual void OnCreate()override;
		//カメラをを得る
		virtual std::shared_ptr<Camera> GetCamera() const override {
			return m_camera;
		}
		//ライトセットを得る
		virtual std::shared_ptr<LightSet> GetLightSet() const override {
			return m_lightSet;
		}
		//カメラとライト更新
		virtual void UpdateCameraLight(double elapsedTime)override {
			m_camera->OnUpdate(elapsedTime);
			m_lightSet->OnUpdate(elapsedTime);
		}
		//スプライトオブジェクトの作成
		void CreateSprite();

		//更新時処理
		virtual void OnUpdate(double elapsedTime);


		//Aボタン
		void OnPushA();
		//Bボタン
		void OnPushB();

	};


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
		//構築時処理
		virtual void OnCreate()override;
		//カメラをを得る
		virtual std::shared_ptr<Camera> GetCamera() const override {
			return m_camera;
		}
		//ライトセットを得る
		virtual std::shared_ptr<LightSet> GetLightSet() const override {
			return m_lightSet;
		}
		//カメラとライト更新
		virtual void UpdateCameraLight(double elapsedTime)override {
			m_camera->OnUpdate(elapsedTime);
			m_lightSet->OnUpdate(elapsedTime);
		}
		//ボックスオブジェクトの作成
		void CreateFixedBox();
		//プレイヤーの作成
		void CreatePlayer();

	};

}
//end namespace basecross

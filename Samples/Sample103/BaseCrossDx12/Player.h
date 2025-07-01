/*!
@file Player.h
@brief プレイヤー
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	プレイヤー
	//--------------------------------------------------------------------------------------
	class Player : public GameObject {
		//		std::shared_ptr<BaseMesh> m_mesh;
		//		std::shared_ptr < BaseTexture> m_texture;
				//プレイヤーが使用するコントローラとキーボードの入力
		Vec2 GetInputState() const;
		// コントローラから方向ベクトルを得る
		Vec3 GetMoveVector() const;
		//プレイヤーの移動
		void MovePlayer();
		//入力ハンドラー
		InputHandler<Player> m_InputHandler;
		//スピード
		float m_Speed;
	public:
		Player(const std::shared_ptr<Stage>& StagePtr, const TransParam& param);
		virtual ~Player() {}
		virtual void OnCreate()override;
		virtual void OnUpdate(double elapsedTime);
		//Aボタン
		void OnPushA();
		//Bボタン
		void OnPushB() {}
	};

}
//end namespace basecross

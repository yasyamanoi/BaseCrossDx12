/*!
@file Player.h
@brief プレイヤー
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	using namespace std;
	using namespace basecross::bsm;

	//--------------------------------------------------------------------------------------
	// プレイヤー
	//--------------------------------------------------------------------------------------
	class Player : public  GameObject {
		shared_ptr<BaseMesh> m_mesh;
		shared_ptr < BaseTexture> m_texture;
		//プレイヤーが使用するコントローラ入力
		Vec2 GetInputState() const;
		// コントローラから方向ベクトルを得る
		Vec3 GetMoveVector() const;
		//プレイヤーの移動
		void MovePlayer(double elapsedTime);
		//スピード
		float m_Speed;
	protected:
	public:
		Player(const shared_ptr<Stage>& stage, const TransParam& param);
		virtual ~Player();
		virtual void OnUpdateConstantBuffers()override;
		virtual void OnCommitConstantBuffers()override;
		virtual void OnCreate();
		virtual void OnUpdate(double elapsedTime);
		virtual void OnShadowDraw()override;
		virtual void OnSceneDraw()override;
		virtual void OnDestroy();

	};

}
// end namespace basecross

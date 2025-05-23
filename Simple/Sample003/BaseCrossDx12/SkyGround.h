/*!
@file SkyGround.h
@brief 土台クラス
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	// 土台のオブジェクト
	//--------------------------------------------------------------------------------------
	class SkyGround : public GameObject {
		std::shared_ptr<BaseMesh> m_mesh;
		std::shared_ptr < BaseTexture> m_texture;
	public:
		SkyGround(const std::shared_ptr<Stage>& stage, const TransParam& param);
		virtual ~SkyGround();
		virtual void OnUpdateConstantBuffers()override;
		virtual void OnCommitConstantBuffers()override;
		virtual void OnCreate()override;
		virtual void OnUpdate(double elapsedTime)override;
		virtual void OnShadowDraw()override;
		virtual void OnSceneDraw()override;
		virtual void OnDestroy()override;
	};




}
// end namespace basecross

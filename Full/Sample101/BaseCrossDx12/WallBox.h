/*!
@file WallBox.h
@brief 四角のオブジェクト
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	// 四角のオブジェクト
	//--------------------------------------------------------------------------------------
	class WallBox : public  GameObject {
		double m_totalTime;
		std::shared_ptr<BaseMesh> m_mesh;
		std::shared_ptr < BaseTexture> m_texture;
	protected:
	public:
		WallBox(const std::shared_ptr<Stage>& stage, const TransParam& param);
		virtual ~WallBox();
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

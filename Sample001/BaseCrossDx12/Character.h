/*!
@file Character.h
@brief さまざまなオブジェクト
*/

#pragma once

#include "stdafx.h"

using namespace bsm;
namespace basecross{
	//--------------------------------------------------------------------------------------
	//	PcSpriteクラス
	//--------------------------------------------------------------------------------------
	class PcSprite : public GameObject {
		Vec3 m_scale;
		Vec3 m_position;
		double m_totalTime;

		std::shared_ptr<BaseMesh> m_spriteMesh;
		std::vector<VertexPositionColor> m_backupVertices;
		std::vector<ConstBuffParam> m_constBuffParamVec;
	public:
		//構築と破棄
		PcSprite();
		virtual ~PcSprite();
		//初期化（パラメータ付き）
		void OnCreate(ID3D12GraphicsCommandList* pCommandList,const Vec3& scale,const Vec3& position);
		//操作
		virtual void OnUpdate(double elapsedTime) override;

		virtual void OnSceneDraw(ID3D12GraphicsCommandList* pCommandList) override;

	};


}
using namespace basecross;
// end namespace basecross

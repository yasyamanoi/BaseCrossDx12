/*!
@file Character.h
@brief さまざまなオブジェクト
*/

#pragma once

#include "stdafx.h"


using namespace bsm;
namespace basecross{

	//--------------------------------------------------------------------------------------
	//	FixedBoxクラス
	//--------------------------------------------------------------------------------------
	class FixedBox : public GameObject {
		Vec3 m_scale;
		Quat m_quaternion;
		Vec3 m_firstPosition;
		Vec3 m_position;
		double m_totalTime;
		Quat m_spanQuat;

		std::shared_ptr<BaseMesh> m_box;
		std::shared_ptr<BaseTexture> m_texture;
		std::vector<ConstBuffParam> m_constBuffParamVec;
	public:
		//構築と破棄
		FixedBox();
		virtual ~FixedBox();
		//初期化（パラメータ付き）
		void OnCreate(ID3D12GraphicsCommandList* pCommandList, const Vec3& scale, const Quat& quaternion, const Vec3& position);
		//操作
		virtual void OnUpdate(double elapsedTime) override;
		virtual void OnSceneDraw(ID3D12GraphicsCommandList* pCommandList) override;
	};


}
using namespace basecross;
// end namespace basecross

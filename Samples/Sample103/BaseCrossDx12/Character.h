/*!
@file Character.h
@brief 配置オブジェクト
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	// ボックスオブジェクト
	//--------------------------------------------------------------------------------------
	class FixedBox : public GameObject {
	public:
		FixedBox(const std::shared_ptr<Stage>& stage, const TransParam& param);
		virtual ~FixedBox();
		virtual void OnCreate()override;
		virtual void OnUpdate(double elapsedTime)override {}
	};

	//--------------------------------------------------------------------------------------
	// モデルオブジェクト
	//--------------------------------------------------------------------------------------
	class FixedModel : public GameObject {
		std::shared_ptr<BaseMesh> m_baseMesh;
		double m_totalTime;
	public:
		FixedModel(const std::shared_ptr<Stage>& stage, const TransParam& param);
		virtual ~FixedModel();
		virtual void OnCreate()override;
		virtual void OnUpdate(double elapsedTime)override;
	};


}
// end namespace basecross

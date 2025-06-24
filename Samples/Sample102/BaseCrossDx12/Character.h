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
	// 四角のオブジェクト
	//--------------------------------------------------------------------------------------
	class WallBox : public  GameObject {
		double m_totalTime;
	protected:
	public:
		WallBox(const std::shared_ptr<Stage>& stage, const TransParam& param);
		virtual ~WallBox();
		virtual void OnCreate();
		virtual void OnUpdate(double elapsedTime);
	};


}
// end namespace basecross

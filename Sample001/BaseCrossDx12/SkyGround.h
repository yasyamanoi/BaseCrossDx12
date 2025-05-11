/*!
@file SkyGround.h
@brief 土台クラス
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	using namespace std;

	//--------------------------------------------------------------------------------------
	// 土台のオブジェクト
	//--------------------------------------------------------------------------------------
	class SkyGround : public MyObject {
	public:
		SkyGround(const TransParam& param);
		virtual ~SkyGround();
		virtual void OnCreate()override;
		virtual void OnUpdate(double elapsedTime)override {}
	};




}
// end namespace basecross

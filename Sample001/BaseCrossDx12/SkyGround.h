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
		virtual void OnCreate(ID3D12GraphicsCommandList* pCommandList);
		virtual void OnUpdate(double elapsedTime);

	};




}
// end namespace basecross

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
	class WallBox : public  MyObject {
		double m_totalTime;
	protected:
	public:
		WallBox(const TransParam& param);
		virtual ~WallBox();
		virtual void OnCreate(ID3D12GraphicsCommandList* pCommandList);
		virtual void OnUpdate(double elapsedTime);
	};

}
// end namespace basecross

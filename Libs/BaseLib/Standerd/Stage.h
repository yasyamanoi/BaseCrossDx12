/*!
@file Stage.h
@brief ステージクラス　親
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#pragma once
#include "stdafx.h"


namespace basecross {

	//--------------------------------------------------------------------------------------
	// ステージの親
	//--------------------------------------------------------------------------------------
	class Stage : public ObjectInterface {
	protected:
		Stage();
		virtual ~Stage();
	public:
		virtual void OnCreate(ID3D12GraphicsCommandList* pCommandList)override;
		virtual void OnUpdate(double elapsedTime)override;
	};


}
// end namespace basecross

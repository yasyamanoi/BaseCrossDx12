/*!
@file Stage.cpp
@brief ステージクラス　親　実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#pragma once
#include "stdafx.h"


namespace basecross {

	Stage::Stage() {}
	Stage::~Stage(){}
	void Stage::OnCreate(ID3D12GraphicsCommandList* pCommandList){}
	void Stage::OnUpdate(double elapsedTime){}

}
// end namespace basecross

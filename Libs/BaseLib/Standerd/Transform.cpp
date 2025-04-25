/*!
@file Transform.cpp
@brief 座標変換コンポーネント　実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	座標変換コンポーネント
	//--------------------------------------------------------------------------------------
	Transform::Transform(const std::shared_ptr<GameObject>& gameObjectPtr):
		Component(gameObjectPtr)
	{}
	Transform::~Transform(){}

}
// end namespace basecross

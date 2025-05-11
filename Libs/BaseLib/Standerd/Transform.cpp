/*!
@file Transform.cpp
@brief 座標変換コンポーネント
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"

namespace basecross {

	Transform::Transform(const shared_ptr<GameObject>& gameObjectPtr, const TransParam& param):
		Component(gameObjectPtr),
		m_param(param)
	{
	}


}
// end namespace basecross

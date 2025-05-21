/*!
@file Component.cpp
@brief コンポーネント親　実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"

namespace basecross {

	Component::Component(const std::shared_ptr<GameObject>& gameObjectPtr) :
		m_gameObject(gameObjectPtr)
	{
	}


}
// end namespace basecross

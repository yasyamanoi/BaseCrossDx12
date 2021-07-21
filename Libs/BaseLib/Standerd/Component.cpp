/*!
@file Component.cpp
@brief コンポーネント親
@copyright Copyright (c) 2021 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"

namespace basecross {

	shared_ptr<BaseStage> Component::GetBaseStage() const {
		return GetGameObject()->GetBaseStage();
	}

	void Component::AttachGameObject(const shared_ptr<GameObject>& GameObjectPtr) {
		m_gameObject = GameObjectPtr;
	}


}
//end basecross

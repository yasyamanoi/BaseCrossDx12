/*!
@file Component.cpp
@brief �R���|�[�l���g�e�@����
*/

#include "stdafx.h"

namespace basecross {

	Component::Component(const std::shared_ptr<GameObject>& gameObjectPtr) :
		m_gameObject(gameObjectPtr),
		m_updateActive(true),
		m_drawActive(false)
	{
	}


}
// end namespace basecross

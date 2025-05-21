/*!
@file GameObject.cpp
@brief ゲームオブジェクトクラス　実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"

namespace basecross {

	std::shared_ptr<Stage> GameObject::GetStage(bool exceptionActive) const {
		auto shptr = m_stage.lock();
		if (shptr) {
			return shptr;
		}
		else {
			if (exceptionActive) {
				throw BaseException(
					L"所属ステージがnullです。自分自身がステージではありませんか？",
					L"if (!shptr)",
					L"GameObject::GetStage()const"
				);
			}
			else {
				return nullptr;
			}
		}
		return nullptr;
	}

	void GameObject::OnPreCreate() {
		//Transform必須
		AddComponent<Transform>(m_tempParam);
	}

	void GameObject::SetToBefore() {
		auto transPtr = GetComponent<Transform>();
		transPtr->SetToBefore();
	}




}
// end namespace basecross

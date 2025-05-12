/*!
@file GameObject.cpp
@brief ゲームオブジェクトクラス　実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"

namespace basecross {

	shared_ptr<Stage> GameObject::GetStage(bool exceptionActive) const {
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


}
// end namespace basecross

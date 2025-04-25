/*!
@file Transform.h
@brief 座標変換コンポーネント
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	座標変換コンポーネント
	//--------------------------------------------------------------------------------------
	class Transform : public Component {
	public:
		explicit Transform(const std::shared_ptr<GameObject>& gameObjectPtr);
		virtual ~Transform();
	};

}
// end namespace basecross

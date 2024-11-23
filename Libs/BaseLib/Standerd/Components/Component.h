/*!
@file Component.h
@brief コンポーネント親
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	class Component :public ObjectInterface {
	protected:
		Component();
		virtual ~Component();
	public:
		virtual void OnCreate();
		virtual void OnUpdate();
		virtual void OnDraw();
		virtual void OnDestroy();
	};

}
// end namespace basecross


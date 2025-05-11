/*!
@file Component.h
@brief コンポーネント親
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#pragma once
#include "stdafx.h"


namespace basecross {

	using namespace std;
	using namespace bsm;

	class GameObject;

	//--------------------------------------------------------------------------------------
	///	オブジェクト変換用
	//--------------------------------------------------------------------------------------
	struct TransParam {
		Vec3 scale;
		Vec3 rotOrigin;
		Quat quaternion;
		Vec3 position;
	};




	//--------------------------------------------------------------------------------------
	// コンポーネントの親
	//--------------------------------------------------------------------------------------
	class Component : public ObjectInterface {
	protected:
		weak_ptr<GameObject> m_gameObject;
		explicit Component(const shared_ptr<GameObject>& gameObjectPtr);
		virtual ~Component() {}
	public:
		void AttachGameObject(const shared_ptr<GameObject>& gameObjectPtr) {
			m_gameObject = gameObjectPtr;
		}

	};



}
// end namespace basecross

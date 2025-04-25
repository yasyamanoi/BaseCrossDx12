/*!
@file Component.h
@brief コンポーネント親クラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#pragma once
#include "stdafx.h"


namespace basecross {

	class GameObject;

	//--------------------------------------------------------------------------------------
	// コンポーネントの親
	//--------------------------------------------------------------------------------------
	class Component : public ObjectInterface {
		std::weak_ptr<GameObject> m_gameObject;
	protected:
		Component(const std::shared_ptr<GameObject>& gameObjectPtr);
		virtual ~Component();
	public:
		virtual void OnCreate(ID3D12GraphicsCommandList* pCommandList)override;
		virtual void OnUpdate(double elapsedTime)override;
	};

}
// end namespace basecross

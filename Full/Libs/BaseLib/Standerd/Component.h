/*!
@file Component.h
@brief コンポーネント親
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#pragma once
#include "stdafx.h"


namespace basecross {

	
	

	class GameObject;

	//--------------------------------------------------------------------------------------
	///	オブジェクト変換用
	//--------------------------------------------------------------------------------------
	struct TransParam {
		bsm::Vec3 scale;
		bsm::Vec3 rotOrigin;
		bsm::Quat quaternion;
		bsm::Vec3 position;
	};




	//--------------------------------------------------------------------------------------
	// コンポーネントの親
	//--------------------------------------------------------------------------------------
	class Component : public ObjectInterface {
	protected:
		std::weak_ptr<GameObject> m_gameObject;
		explicit Component(const std::shared_ptr<GameObject>& gameObjectPtr);
		virtual ~Component() {}
	public:
		void AttachGameObject(const std::shared_ptr<GameObject>& gameObjectPtr) {
			m_gameObject = gameObjectPtr;
		}
		std::shared_ptr<GameObject> GetGameObject() const {
			auto ptr = m_gameObject.lock();
			if (!ptr) {
				throw BaseException(
					L"GameObjectは有効ではありません",
					L"if (!shptr)",
					L"Component::GetGameObject()const"
				);
			}
			else {
				return ptr;
			}
			return nullptr;
		}

		virtual void OnUpdateConstantBuffers(){}
		virtual void OnCommitConstantBuffers(){}

		virtual void OnPreCreate() override{}
		virtual void OnCreate()override {}
		virtual void OnUpdate(double elapsedTime)override {}
		virtual void OnShadowDraw()override {}
		virtual void OnSceneDraw()override {}
		virtual void OnDestroy()override {}

	};



}
// end namespace basecross

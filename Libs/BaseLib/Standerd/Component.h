/*!
@file Component.h
@brief コンポーネント親
@copyright Copyright (c) 2021 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	class GameObject;
	class BaseStage;

	//--------------------------------------------------------------------------------------
	///	コンポーネント親クラス
	//--------------------------------------------------------------------------------------
	class Component :public ObjectInterface {
		weak_ptr<GameObject> m_gameObject;
		bool m_updateActive{ true };
		bool m_renderActive{ true };
	protected:
		explicit Component(const shared_ptr<GameObject>& gameObjectPtr) :
			m_gameObject(gameObjectPtr) {}
		virtual ~Component() {}
	public:
		shared_ptr<GameObject> GetGameObject() const {
			auto ptr = m_gameObject.lock();
			if (!ptr) {
				throw BaseException(
					L"GameObjectは有効ではありません",
					L"if (!shptr)",
					L"Component::GetGameObject()"
				);
			}
			else {
				return ptr;
			}
			return nullptr;
		}
		shared_ptr<BaseStage> GetBaseStage() const;
		bool IsUpdateActive() const {
			return m_updateActive;
		}
		void SetUpdateActive(bool b) {
			m_updateActive = b;
		}
		bool IsRenderActive() const {
			return m_renderActive;
		}
		void SetRenderActive(bool b) {
			m_renderActive = b;
		}
		void AttachGameObject(const shared_ptr<GameObject>& GameObjectPtr);

	};



}
//end basecross

/*!
@file Component.h
@brief コンポーネント親
*/

#pragma once
#include "stdafx.h"


namespace basecross {

	class GameObject;

	//--------------------------------------------------------------------------------------
	// コンポーネントの親
	//--------------------------------------------------------------------------------------
	class Component : public ObjectInterface {
		bool m_updateActive;
		bool m_drawActive;

	protected:
		std::weak_ptr<GameObject> m_gameObject;
		std::weak_ptr<BaseMesh> m_mesh;
		std::weak_ptr<BaseTexture> m_texture;

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


		void SetBaseMesh(const std::shared_ptr<BaseMesh>& mesh) {
			m_mesh = mesh;
		}
		void SetBaseMesh(const std::wstring& key) {
			SetBaseMesh(BaseScene::Get()->GetMesh(key));
		}

		std::shared_ptr<BaseMesh> GetBaseMesh()const {
			return m_mesh.lock();
		}

		void SetBaseTexture(const std::shared_ptr<BaseTexture>& texture) {
			m_texture = texture;
		}
		void SetBaseTexture(const std::wstring& key) {
			SetBaseTexture(BaseScene::Get()->GetTexture(key));
		}

		std::shared_ptr<BaseTexture> GetBaseTexture() {
			return m_texture.lock();
		}




		//void SetOriginalBaseMesh(const std::wstring& key,const std::shared_ptr<BaseMesh>& mesh) {
		//	auto it = m_meshMap.find(key);
		//	if (it != m_meshMap.end()) {
		//		throw BaseException(
		//			L"指定のキーのメッシュはすでに登録されています",
		//			key,
		//			L"BaseScene::SetOriginalBaseMesh()"
		//		);
		//	}
		//	else {
		//		m_meshMap[key] = mesh;
		//	}
		//}
		///*
		//* BaseSceneから取り出す
		//*/
		//void SetBaseMesh(const std::wstring& key) {
		//	auto mesh = BaseScene::Get()->GetMesh(key);
		//	auto it = m_meshMap.find(key);
		//	if (it != m_meshMap.end()) {
		//		throw BaseException(
		//			L"指定のキーのメッシュはすでに登録されています",
		//			key,
		//			L"BaseScene::SetOriginalBaseMesh()"
		//		);
		//	}
		//	else {
		//		m_meshMap[key] = mesh;
		//	}
		//}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	updateが有効かどうか
		@return	有効ならtrue
		*/
		//--------------------------------------------------------------------------------------
		bool IsUpdateActive() const {
			return m_updateActive;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	updateが有効かどうかの設定
		@param[in]	b	trueかfalse
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetUpdateActive(bool b) {
			m_updateActive = b;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	drawが有効かどうか
		@return	有効ならtrue
		*/
		//--------------------------------------------------------------------------------------
		bool IsDrawActive() const {
			return m_drawActive;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	drawが有効かどうかの設定
		@param[in]	b	trueかfalse
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetDrawActive(bool b) {
			m_drawActive = b;
		}


		virtual void OnUpdateConstantBuffers() {}
		virtual void OnCommitConstantBuffers() {}

		virtual void OnPreCreate() override {}
		virtual void OnCreate()override {}
		virtual void OnUpdate(double elapsedTime)override {}
		virtual void OnShadowDraw(ID3D12GraphicsCommandList* pCommandList)override {}
		virtual void OnSceneDraw(ID3D12GraphicsCommandList* pCommandList)override {}
		virtual void OnDestroy()override {}

	};



}
// end namespace basecross

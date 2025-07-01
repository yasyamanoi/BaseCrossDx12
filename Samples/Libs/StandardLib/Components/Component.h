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

		std::vector<std::weak_ptr<BaseMesh>> m_meshVec;
		std::vector<std::weak_ptr<BaseTexture>> m_textureVec;

//		std::map<std::wstring,std::weak_ptr<BaseMesh>> m_meshMap;
//		std::map<std::wstring, std::weak_ptr<BaseTexture>> m_textureMap;

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


		size_t AddBaseMesh(const std::shared_ptr<BaseMesh>& mesh) {
			size_t size = m_meshVec.size();
			m_meshVec.push_back(mesh);
			return size;
		}
		size_t AddBaseMesh(const std::wstring& key) {
			return AddBaseMesh(BaseScene::Get()->GetMesh(key));
		}

		std::shared_ptr<BaseMesh> GetBaseMesh(size_t index) {
			if (index >= m_meshVec.size()) {
				throw BaseException(
					L"指定のインデックスが範囲外です",
					Util::SizeTToWStr(index),
					L"BaseScene::GetBaseMesh()"
				);
			}
			else {
				auto shptr = m_meshVec[index].lock();
				if (shptr) {
					return shptr;
				}
				else {
					throw BaseException(
						L"指定のメッシュは有効ではありません",
						Util::SizeTToWStr(index),
						L"BaseScene::GetMesh()"
					);

				}
			}
			return nullptr;
		}

		size_t AddBaseTexture(const std::shared_ptr<BaseTexture>& texture) {
			size_t size = m_textureVec.size();
			m_textureVec.push_back(texture);
			return size;
		}
		size_t AddBaseTexture(const std::wstring& key) {
			return AddBaseTexture(BaseScene::Get()->GetTexture(key));
		}

		std::shared_ptr<BaseTexture> GetBaseTexture(size_t index) {
			if (index >= m_textureVec.size()) {
				return nullptr;
				//throw BaseException(
				//	L"指定のインデックスが範囲外です",
				//	Util::SizeTToWStr(index),
				//	L"BaseScene::GetBaseTexture()"
				//);
			}
			else {
				auto shptr = m_textureVec[index].lock();
				if (shptr) {
					return shptr;
				}
				else {
					return nullptr;
					//throw BaseException(
					//	L"指定のテクスチャは有効ではありません",
					//	Util::SizeTToWStr(index),
					//	L"BaseScene::GetBaseTexture()"
					//);

				}
			}
			return nullptr;
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

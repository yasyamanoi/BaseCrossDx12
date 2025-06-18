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
		TransParam():
			scale(1.0f),
			rotOrigin(0.0f),
			quaternion(),
			position(0.0f)
		{ }
	};

	//--------------------------------------------------------------------------------------
	///	スプライト用コンスタントバッファ構造体
	//--------------------------------------------------------------------------------------
	struct SpriteConstant
	{
		/// ワールド行列
		bsm::Mat4x4 world;
		/// エミッシブ色
		bsm::Col4 emissive;
		/// デフューズ色
		bsm::Col4 diffuse;
		SpriteConstant()
		{
			memset(this, 0, sizeof(SpriteConstant));
			emissive = bsm::Col4(0.0f);
			diffuse = bsm::Col4(1.0f, 1.0f, 1.0f, 1.0f);
		};
	};

	//--------------------------------------------------------------------------------------
	///	SimpleConstantコンスタントバッファ構造体
	//--------------------------------------------------------------------------------------
	struct SimpleConstant
	{
		/// ワールド行列
		bsm::Mat4x4 world;
		/// ビュー行列
		bsm::Mat4x4 view;
		/// 射影行列
		bsm::Mat4x4 projection;
		/// エミッシブ色
		bsm::Col4 emissive;
		/// デフューズ色
		bsm::Col4 diffuse;
		/// スペキュラー
		bsm::Col4 specular;
		/// テクスチャ=xがアクティブかどうか
		XMUINT4 activeFlg;
		/// ライイト方向
		bsm::Vec4 lightDir;
		/// ライト位置
		bsm::Vec4 lightPos;
		/// eyeの位置
		bsm::Vec4 eyePos;
		/// ライトビュー行列
		bsm::Mat4x4 lightView;
		/// ライト射影行列
		bsm::Mat4x4 lightProjection;
		/// Bone配列
		bsm::Vec4 Bones[3 * 72];
		SimpleConstant() {
			memset(this, 0, sizeof(SimpleConstant));
			diffuse = bsm::Col4(1.0f, 1.0f, 1.0f, 1.0f);
		};
	};



	//--------------------------------------------------------------------------------------
	///	Basicシェーダー用コンスタントバッファ
	//--------------------------------------------------------------------------------------
	struct BasicConstant
	{
		bsm::Col4 diffuseColor;
		bsm::Col4 emissiveColor;
		bsm::Col4 specularColorAndPower;

		bsm::Vec4 lightDirection[3];
		bsm::Vec4 lightDiffuseColor[3];
		bsm::Vec4 lightSpecularColor[3];

		bsm::Vec4 eyePosition;

		bsm::Col4 fogColor;
		bsm::Vec4 fogVector;

		bsm::Mat4x4 world;
		bsm::Vec4 worldInverseTranspose[3];
		bsm::Mat4x4 worldViewProj;
		//汎用フラグ
		XMUINT4 activeFlg;
		//以下影
		bsm::Vec4 lightPos;
		bsm::Vec4 eyePos;
		bsm::Mat4x4 lightView;
		bsm::Mat4x4 lightProjection;

		bsm::Vec4 bones[3 * 72];
	};



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

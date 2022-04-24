/*!
@file Component.h
@brief コンポーネント親
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	class GameObject;
	class BaseStage;

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
			emissive = Col4(0.0f);
			diffuse = Col4(1.0f, 1.0f, 1.0f, 1.0f);
		};
	};


	//--------------------------------------------------------------------------------------
	///	Shadowコンスタントバッファ構造体
	//--------------------------------------------------------------------------------------
	struct ShadowConstant
	{
		/// ワールド行列
		bsm::Mat4x4 world;
		/// ビュー行列
		bsm::Mat4x4 view;
		/// 射影行列
		Mat4x4 projection;
		/// bone用
		bsm::Vec4 bones[3 * 72];
		ShadowConstant() {
			memset(this, 0, sizeof(ShadowConstant));
		};
	};


	//--------------------------------------------------------------------------------------
	///	SimpleConstantコンスタントバッファ構造体
	//--------------------------------------------------------------------------------------
	struct SimpleConstant
	{
		/// ワールド行列
		Mat4x4 world;
		/// ビュー行列
		Mat4x4 view;
		/// 射影行列
		Mat4x4 projection;
		/// エミッシブ色
		Col4 emissive;
		/// デフューズ色
		Col4 diffuse;
		/// スペキュラー
		Col4 specular;
		/// テクスチャ=xがアクティブかどうか
		XMUINT4 activeFlg;
		/// ライイト方向
		Vec4 lightDir;
		/// ライト位置
		Vec4 lightPos;
		/// eyeの位置
		Vec4 eyePos;
		/// ライトビュー行列
		Mat4x4 lightView;
		/// ライト射影行列
		Mat4x4 lightProjection;
		/// Bone配列
		Vec4 Bones[3 * 72];
		SimpleConstant() {
			memset(this, 0, sizeof(SimpleConstant));
			diffuse = Col4(1.0f, 1.0f, 1.0f, 1.0f);
		};
	};


	//--------------------------------------------------------------------------------------
	///	Basicシェーダー用コンスタントバッファ
	//--------------------------------------------------------------------------------------
	struct BasicConstants
	{
		Vec4 diffuseColor;
		Vec4 emissiveColor;
		Vec4 specularColorAndPower;

		Vec4 lightDirection[3];
		Vec4 lightDiffuseColor[3];
		Vec4 lightSpecularColor[3];

		Vec4 eyePosition;

		Vec4 fogColor;
		Vec4 fogVector;

		Mat4x4 world;
		Vec4 worldInverseTranspose[3];
		Mat4x4 worldViewProj;
		//汎用フラグ
		XMUINT4 activeFlg;
		//以下影
		Vec4 lightPos;
		Vec4 eyePos;
		Mat4x4 lightView;
		Mat4x4 lightProjection;

		Vec4 bones[3 * 72];
	};


	//--------------------------------------------------------------------------------------
	///	コンポーネント親クラス
	//--------------------------------------------------------------------------------------
	class Component :public ObjectInterface {
		weak_ptr<GameObject> m_gameObject;
		bool m_updateActive;
		bool m_renderActive;
		wstring m_samplerKey;
	protected:
		//フレームパラメータのインデックス(コンスタントバッファで使う)
		size_t m_frameParamIndex;
		explicit Component(const shared_ptr<GameObject>& gameObjectPtr);
		virtual ~Component() {}
		shared_ptr<BaseMesh> m_baseMesh;
		shared_ptr<BaseTexture> m_baseTexture;
		virtual void PopulateCommandList(BaseFrame* pBaseFrame) {}
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	メッシュの取得
		@return	メッシュ
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<BaseMesh> GetMesh() const {
			return m_baseMesh;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	メッシュの設定
		@param[in]	mesh	メッシュ
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetMesh(const shared_ptr<BaseMesh>& mesh) {
			m_baseMesh = mesh;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	メッシュの設定
		@param[in]	key	メッシュのリソースキー
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetMesh(const wstring& key) {
			SetMesh(App::GetBaseScene()->GetMesh(key));
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	頂点の変更
		@tparam	T	頂点の型
		@param[in]	vertices	頂点の配列
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		void UpdateVertices( const vector<T>& vertices) {
			if (!m_baseMesh) {
				throw BaseException(
					L"メッシュが有効ではありません",
					L"if (!m_baseMesh)",
					L"Component::UpdateVirtexBuffer()"
				);
			}
			auto comList = App::GetBaseDevice()->GetComandList();
			m_baseMesh->UpdateVirtexBuffer<T>(comList, vertices);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	テクスチャの取得
		@return	テクスチャ
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<BaseTexture> GetTexture() const {
			return m_baseTexture;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	テクスチャの設定
		@param[in]	baseTexture	テクスチャ
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetTexture(const shared_ptr<BaseTexture>& baseTexture) {
			m_baseTexture = baseTexture;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	テクスチャの設定
		@param[in]	key	テクスチャのリソースキー
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetTexture(const wstring& key) {
			SetTexture(App::GetBaseScene()->GetTexture(key));
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	サンプラーキーの取得
		@return	サンプラーキー
		*/
		//--------------------------------------------------------------------------------------
		wstring GetSamplerKey() const {
			return m_samplerKey;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	サンプラーキーの設定
		@param[in]	key	サンプラーキー
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetSamplerKey(const wstring& key) {
			m_samplerKey = key;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ゲームオブジェクトを得る
		@return	ゲームオブジェクト
		*/
		//--------------------------------------------------------------------------------------
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
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ステージを得る
		@return	ステージ
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<Stage> GetStage() const;
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
		@brief	renderが有効かどうか
		@return	有効ならtrue
		*/
		//--------------------------------------------------------------------------------------
		bool IsRenderActive() const {
			return m_renderActive;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	renderが有効かどうかの設定
		@param[in]	b	trueかfalse
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetRenderActive(bool b) {
			m_renderActive = b;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	フレームパラメータのインデックスを得る
		@return	フレームパラメータのインデックス
		*/
		//--------------------------------------------------------------------------------------
		size_t GetFrameParamIndex() const {
			return m_frameParamIndex;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ゲームオブジェクトの設定
		@param[in]	GameObjectPtr	ゲームオブジェクト
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void AttachGameObject(const shared_ptr<GameObject>& GameObjectPtr);

		virtual void OnInitFrame(BaseFrame* pBaseFrame) {}
		virtual void WriteConstantBuffers(BaseFrame* pBaseFrame){}
		virtual void OnRender()override;
	};



}
//end basecross

#pragma once
/*!
@file MyObject.h
@brief Myオブジェクトクラス
*/


#pragma once
#include "stdafx.h"


namespace basecross {

	class Stage;

	//--------------------------------------------------------------------------------------
	///	Shadowコンスタントバッファ構造体
	//--------------------------------------------------------------------------------------
	struct ShadowConstantBuffer
	{
		/// ワールド行列
		Mat4x4 world;
		/// ビュー行列
		Mat4x4 view;
		/// 射影行列
		Mat4x4 projection;
		/// bone用
		Vec4 bones[3 * 72];
		ShadowConstantBuffer() {
			memset(this, 0, sizeof(ShadowConstantBuffer));
		};
	};

	//--------------------------------------------------------------------------------------
	///	Basicシェーダー用コンスタントバッファ
	//--------------------------------------------------------------------------------------
	struct BasicConstantBuffer
	{
		Col4 diffuseColor;
		Col4 emissiveColor;
		Col4 specularColorAndPower;

		Vec4 lightDirection[3];
		Vec4 lightDiffuseColor[3];
		Vec4 lightSpecularColor[3];

		Vec4 eyePosition;

		Col4 fogColor;
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
	// 配置されるオブジェクトの親
	//--------------------------------------------------------------------------------------
	class MyObject : public GameObject {
	protected:
		BasicConstantBuffer m_constantBuffer;
		size_t m_constantBufferIndex;

		ShadowConstantBuffer m_shadowConstantBuffer;
		size_t m_shadowConstantBufferIndex;

		//フォグが有効かどうか
		bool m_fogEnabled = true;
		//フォグの開始位置
		float m_fogStart = -25.0f;
		//フォグの終了位置
		float m_fogEnd = -40.0f;
		//フォグ色
		XMFLOAT4 m_fogColor;
		//フォグベクトル
		XMFLOAT3 m_fogVector;
		MyObject(const std::shared_ptr<Stage>& stage, const TransParam& param) :
			GameObject(stage,param){
			m_fogColor = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
			m_fogVector = XMFLOAT3(0.0, 0.0, 1.0f);
		}
		virtual ~MyObject() {}
		std::shared_ptr<BaseMesh> m_mesh;
		std::shared_ptr<BaseTexture> m_texture;
	public:
		virtual void OnUpdateConstantBuffers()override;
		virtual void OnCommitConstantBuffers()override;
		virtual void OnCreate()override;
		virtual void OnShadowDraw()override;
		virtual void OnSceneDraw()override;
		virtual void OnDestroy()override {}
	};


}
// end namespace basecross

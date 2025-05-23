/*!
@file BcStaticDraw.h
@brief ベイシック描画コンポーネント
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	DECLARE_DX12SHADER(BcVSPNTStaticPL)
	DECLARE_DX12SHADER(BcPSPNTPL)
	DECLARE_DX12SHADER(BcVSPNTStaticPLShadow)
	DECLARE_DX12SHADER(BcPSPNTPLShadow)


	//--------------------------------------------------------------------------------------
	///	Basicシェーダー用コンスタントバッファ
	//--------------------------------------------------------------------------------------
	struct BasicConstantBuffer
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
	///	ベイシック描画コンポーネント
	//--------------------------------------------------------------------------------------
	class BcPNTStaticDraw : public Component {
	protected:
		std::weak_ptr<BaseMesh> m_mesh;
		std::weak_ptr <BaseTexture> m_texture;
		BasicConstantBuffer m_constantBuffer;
		size_t m_constantBufferIndex;

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

	public:
		void SetBaseMesh(const std::shared_ptr<BaseMesh>& mesh) {
			m_mesh = mesh;
		}
		void SetBaseTexture(const std::shared_ptr<BaseTexture>& texture) {
			m_texture = texture;
		}
		BcPNTStaticDraw(const std::shared_ptr<GameObject>& gameObjectPtr);
		virtual ~BcPNTStaticDraw() {}
		virtual void OnUpdateConstantBuffers()override;
		virtual void OnCommitConstantBuffers()override;
		virtual void OnCreate()override;
		virtual void OnSceneDraw()override;
	};

}
// end namespace basecross

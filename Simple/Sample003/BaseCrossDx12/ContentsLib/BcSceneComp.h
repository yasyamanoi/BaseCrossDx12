/*!
@file BcSceneComp.h
@brief ベイシックシーンコンポーネント
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	using namespace std;
	using namespace basecross::bsm;

	

	DECLARE_DX12SHADER(BcVSPNTStaticPL)
	DECLARE_DX12SHADER(BcPSPNTPL)
	DECLARE_DX12SHADER(BcVSPNTStaticPLShadow)
	DECLARE_DX12SHADER(BcPSPNTPLShadow)


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
	///	ベイシックシーンコンポーネント
	//--------------------------------------------------------------------------------------
	class BcSceneComp : public Component {
	protected:
		weak_ptr<BaseMesh> m_mesh;
		weak_ptr <BaseTexture> m_texture;
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
		void SetBaseMesh(const shared_ptr<BaseMesh>& mesh) {
			m_mesh = mesh;
		}
		void SetBaseTexture(const shared_ptr<BaseTexture>& texture) {
			m_texture = texture;
		}
		BcSceneComp(const shared_ptr<GameObject>& gameObjectPtr);
		virtual ~BcSceneComp() {}
		virtual void OnUpdateConstantBuffers()override;
		virtual void OnCommitConstantBuffers()override;
		virtual void OnCreate()override;
		virtual void OnSceneDraw()override;
	};

}
// end namespace basecross

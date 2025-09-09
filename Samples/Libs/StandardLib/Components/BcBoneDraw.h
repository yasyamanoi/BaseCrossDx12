/*!
@file BcBoneDraw.h
@brief ベイシックスタティック描画コンポーネント
*/


#pragma once
#include "stdafx.h"

namespace basecross {


	DECLARE_DX12SHADER(BcVSPNTBonePL)
	DECLARE_DX12SHADER(BcVSPNTBonePLShadow)


	//--------------------------------------------------------------------------------------
	///	BcBoneDrawコンポーネント
	//--------------------------------------------------------------------------------------
	class BcBoneDraw : public Component {
	protected:
		BasicConstant m_constantBuffer;
		size_t m_constantBufferIndex;
		//自分自身に影を描画するかどうか
		bool m_ownShadowActive;
		//フォグが有効かどうか
		bool m_fogEnabled = true;
		//フォグの開始位置
		float m_fogStart = -10.0f;
		//フォグの終了位置
		float m_fogEnd = -40.0f;
		//フォグ色
		XMFLOAT4 m_fogColor;
		//フォグベクトル
		XMFLOAT3 m_fogVector;

		std::vector<Mat4x4>  m_BoneTransforms;

	public:
		bool IsOwnShadowActive()const {
			return m_ownShadowActive;
		}
		void SetOwnShadowActive(bool b) {
			m_ownShadowActive = b;
		}
		bool IsSetFogEnabled()const {
			return m_fogEnabled;
		}
		void SetFogEnabled(bool b) {
			m_fogEnabled = b;
		}



		BcBoneDraw(const std::shared_ptr<GameObject>& gameObjectPtr);
		virtual ~BcBoneDraw() {}
		virtual void OnUpdateConstantBuffers()override;
		virtual void OnCommitConstantBuffers()override;
		virtual void OnCreate()override;
		virtual void OnSceneDraw()override;

		virtual bool UpdateAnimation(double animeTime);

	};

}
// end namespace basecross

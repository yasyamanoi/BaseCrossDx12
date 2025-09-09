/*!
@file BcStaticDraw.h
@brief ベイシックスタティック描画コンポーネント
*/


#pragma once
#include "stdafx.h"

namespace basecross {


	DECLARE_DX12SHADER(BcVSPNTStaticPL)
	DECLARE_DX12SHADER(BcPSPNTPL)
	DECLARE_DX12SHADER(BcVSPNTStaticPLShadow)
	DECLARE_DX12SHADER(BcPSPNTPLShadow)




	//--------------------------------------------------------------------------------------
	///	BcPNTStaticDrawコンポーネント
	//--------------------------------------------------------------------------------------
	class BcPNTStaticDraw : public Component {
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

		

		BcPNTStaticDraw(const std::shared_ptr<GameObject>& gameObjectPtr);
		virtual ~BcPNTStaticDraw() {}
		virtual void OnUpdateConstantBuffers()override;
		virtual void OnCommitConstantBuffers()override;
		virtual void OnCreate()override;
		virtual void OnSceneDraw()override;
	};

}
// end namespace basecross

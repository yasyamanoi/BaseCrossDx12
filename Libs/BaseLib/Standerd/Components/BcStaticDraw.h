/*!
@file BcStaticDraw.h
@brief Basicスタティック描画コンポーネント群
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	BcStatic描画コンポーネント
	//--------------------------------------------------------------------------------------
	class BcStaticDraw : public Component {
		//自分自身に影を描画するかどうか
		bool m_ownShadowActive;
		//フォグが有効かどうか
		bool m_fogEnabled;
		//フォグの開始位置
		float m_fogStart;
		//フォグの終了位置
		float m_fogEnd;
		//フォグ色
		bsm::Col4 m_fogColor;
		//フォグベクトル
		bsm::Vec3 m_fogVector;
	protected:
		explicit BcStaticDraw(const shared_ptr<GameObject>& gameObjectPtr) :
			Component(gameObjectPtr),
			m_ownShadowActive(false),
			m_fogEnabled(false),
			m_fogStart(-25.0f),
			m_fogEnd(-40.0f),
			m_fogColor(0.8f, 0.8f, 0.8f, 1.0f),
			m_fogVector(0.0, 0.0, 1.0f)
		{
			SetDrawActive(true);
		}
		virtual ~BcStaticDraw() {}
		//コンスタントバッファの登録
		void SetConstants(BasicConstants& constants, const shared_ptr<Transform>& transform);
	public:
		bool IsOwnShadowActive()const {
			return m_ownShadowActive;
		}
		void SetOwnShadowActive(bool b) {
			m_ownShadowActive = b;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	フォグを使うかどうか
		@return	フォグを使うならtrue
		*/
		//--------------------------------------------------------------------------------------
		bool IsFogEnabled() const {
			return m_fogEnabled;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	フォグを使うかどうか指定
		@param[in]	b	フォグを使うならtrue
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetFogEnabled(bool b) {
			m_fogEnabled = b;
		}
		virtual void OnInitFrame(BaseFrame* pBaseFrame)override;
		virtual void WriteConstantBuffers(BaseFrame* pBaseFrame)override;
	};

	DECLARE_DX12SHADER(BcVSPNTStaticPL)
	DECLARE_DX12SHADER(BcPSPNTPL)
	DECLARE_DX12SHADER(BcVSPNTStaticPLShadow)
	DECLARE_DX12SHADER(BcPSPNTPLShadow)

	class BcPNTStaticDraw : public BcStaticDraw {
		void CreatePipelineStates();
		ComPtr<ID3D12PipelineState> m_defaultPipelineState;
		ComPtr<ID3D12PipelineState> m_defaultShadowPipelineState;
		ComPtr<ID3D12PipelineState> m_alphaPipelineState;
		ComPtr<ID3D12PipelineState> m_alphaShadowPipelineState;
	protected:
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		explicit BcPNTStaticDraw(const shared_ptr<GameObject>& gameObjectPtr);
		BcPNTStaticDraw(const shared_ptr<GameObject>& gameObjectPtr,
			vector<VertexPositionNormalTexture>& vertices, vector<uint32_t>& indices);
		virtual ~BcPNTStaticDraw() {}
		//操作
		virtual void OnCreate()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};


}
// end namespace basecross

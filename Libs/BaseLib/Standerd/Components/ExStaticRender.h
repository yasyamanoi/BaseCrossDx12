/*!
@file ExStaticRender.h
@brief Extraスタティック描画コンポーネント群
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	DECLARE_DX12SHADER(ExVSPNTTn)
	DECLARE_DX12SHADER(ExPSPNTTn)

	//--------------------------------------------------------------------------------------
	///	ExStatic描画コンポーネント
	//--------------------------------------------------------------------------------------
	class ExStaticRender : public Component {
		/// エミッシブ色
		Col4 m_emissive;
		/// デフューズ色
		Col4 m_diffuse;
		/// スペキュラー
		Col4 m_specular;
		//自分自身に影を描画するかどうか
		bool m_ownShadowActive;
		void CreatePipelineStates();
		ComPtr<ID3D12PipelineState> m_defaultPipelineState;
	protected:
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		Col4 GetEmissive()const {
			return m_emissive;
		}
		void SetEmissive(const Col4& col) {
			m_emissive = col;
		}
		Col4 GetDiffuse()const {
			return m_diffuse;
		}
		void SetDiffuse(const Col4& col) {
			m_diffuse = col;
		}
		Col4 GetSpecular()const {
			return m_specular;
		}
		void SetSpecular(const Col4& col) {
			m_specular = col;
		}
		bool IsOwnShadowActive()const {
			return m_ownShadowActive;
		}
		void SetOwnShadowActive(bool b) {
			m_ownShadowActive = b;
		}


		explicit ExStaticRender(const shared_ptr<GameObject>& gameObjectPtr);
		virtual ~ExStaticRender() {}
		virtual void OnInitFrame(BaseFrame* pBaseFrame)override;
		void SetConstant(ExConstant& constant, const shared_ptr<Transform>& transform);
		virtual void WriteConstantBuffers(BaseFrame* pBaseFrame)override;
		virtual void OnCreate()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}

	};



}
//end namespace basecross

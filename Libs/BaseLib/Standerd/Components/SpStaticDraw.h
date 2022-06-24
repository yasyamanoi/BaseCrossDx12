/*!
@file SpStaticDraw.h
@brief Simpleスタティック描画コンポーネント群
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	SpStatic描画コンポーネント(親)
	//--------------------------------------------------------------------------------------
	class SpStaticDraw : public Component {
		/// エミッシブ色
		Col4 m_emissive;
		/// デフューズ色
		Col4 m_diffuse;
		/// スペキュラー
		Col4 m_specular;
		//自分自身に影を描画するかどうか
		bool m_ownShadowActive;
	protected:
		explicit SpStaticDraw(const shared_ptr<GameObject>& gameObjectPtr) :
			Component(gameObjectPtr),
			m_emissive(0.0f),
			m_diffuse(1.0f),
			m_specular(0.0f),
			m_ownShadowActive(false)
		{}
		virtual ~SpStaticDraw() {}
		//コンスタントバッファの登録
		void SetConstant(SimpleConstant& constant, const shared_ptr<Transform>& transform);
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
		virtual void OnInitFrame(BaseFrame* pBaseFrame)override;
		virtual void WriteConstantBuffers(BaseFrame* pBaseFrame)override;
	};

	//--------------------------------------------------------------------------------------
	///	SpPCStatic描画コンポーネント
	//--------------------------------------------------------------------------------------

	DECLARE_DX12SHADER(SpVSPCStatic)
	DECLARE_DX12SHADER(SpPSPCStatic)

	class SpPCStaticDraw : public SpStaticDraw {
		void CreatePipelineStates();
	protected:
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		explicit SpPCStaticDraw(const shared_ptr<GameObject>& gameObjectPtr);
		SpPCStaticDraw(const shared_ptr<GameObject>& gameObjectPtr,
			vector<VertexPositionColor>& vertices, vector<uint32_t>& indices);
		virtual ~SpPCStaticDraw() {}
		//操作
		virtual void OnCreate()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};

	//--------------------------------------------------------------------------------------
	///	SpWireframeDraw描画コンポーネント(ワイヤフレーム)
	//--------------------------------------------------------------------------------------
	class SpWireframeDraw : public SpStaticDraw {
		void CreatePipelineStates();
	protected:
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		explicit SpWireframeDraw(const shared_ptr<GameObject>& gameObjectPtr);
		SpWireframeDraw(const shared_ptr<GameObject>& gameObjectPtr,
			vector<VertexPositionColor>& vertices, vector<uint32_t>& indices);
		virtual ~SpWireframeDraw() {}
		//操作
		virtual void OnCreate()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};



	//--------------------------------------------------------------------------------------
	///	SpPNTStatic描画コンポーネント
	//--------------------------------------------------------------------------------------

	DECLARE_DX12SHADER(SpVSPNTStatic)
	DECLARE_DX12SHADER(SpVSPNTStaticShadow)

	DECLARE_DX12SHADER(SpPSPNTStatic)
	DECLARE_DX12SHADER(SpPSPNTStaticShadow)

	class SpPNTStaticDraw : public SpStaticDraw {
		void CreatePipelineStates();
	protected:
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		explicit SpPNTStaticDraw(const shared_ptr<GameObject>& gameObjectPtr);
		SpPNTStaticDraw(const shared_ptr<GameObject>& gameObjectPtr,
			vector<VertexPositionNormalTexture>& vertices, vector<uint32_t>& indices);
		virtual ~SpPNTStaticDraw() {}
		//操作
		virtual void OnCreate()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};



}
// end namespace basecross

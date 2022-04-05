/*!
@file SpStaticRender.h
@brief Simpleスタティック描画コンポーネント群
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	Shadowmap描画コンポーネント
	//--------------------------------------------------------------------------------------
	class ShadowmapRender : public Component {
		void CreatePipelineStates();
		static ComPtr<ID3D12PipelineState> m_defaultPipelineState;
	protected:
		//コンスタントバッファの登録
		void SetConstant(ShadowConstant& constant, const shared_ptr<Transform>& transform);
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		virtual void OnInitFrame(BaseFrame* pBaseFrame)override;
		virtual void WriteConstantBuffers(BaseFrame* pBaseFrame)override;
		explicit ShadowmapRender(const shared_ptr<GameObject>& gameObjectPtr);
		virtual ~ShadowmapRender() {}
		//操作
		virtual void OnInit()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};



	//--------------------------------------------------------------------------------------
	///	SpStatic描画コンポーネント(親)
	//--------------------------------------------------------------------------------------
	class SpStaticRender : public Component {
		/// エミッシブ色
		Col4 m_emissive;
		/// デフューズ色
		Col4 m_diffuse;
		/// スペキュラー
		Col4 m_specular;
	protected:
		explicit SpStaticRender(const shared_ptr<GameObject>& gameObjectPtr) :
			Component(gameObjectPtr),
			m_emissive(0.0f),
			m_diffuse(1.0f),
			m_specular(0.0f)
		{}
		virtual ~SpStaticRender() {}
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
		virtual void OnInitFrame(BaseFrame* pBaseFrame)override;
		virtual void WriteConstantBuffers(BaseFrame* pBaseFrame)override;
	};

	//--------------------------------------------------------------------------------------
	///	SpPCStatic描画コンポーネント
	//--------------------------------------------------------------------------------------

	DECLARE_DX12SHADER(SpVSPCStatic)
	DECLARE_DX12SHADER(SpPSPCStatic)

	class SpPCStaticRender : public SpStaticRender {
		void CreatePipelineStates();
		static ComPtr<ID3D12PipelineState> m_defaultPipelineState;
		static ComPtr<ID3D12PipelineState> m_alphaPipelineState;
	protected:
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		explicit SpPCStaticRender(const shared_ptr<GameObject>& gameObjectPtr);
		SpPCStaticRender(const shared_ptr<GameObject>& gameObjectPtr,
			vector<VertexPositionColor>& vertices, vector<uint32_t>& indices);
		virtual ~SpPCStaticRender() {}
		//操作
		virtual void OnInit()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};

	//--------------------------------------------------------------------------------------
	///	SpPTStatic描画コンポーネント
	//--------------------------------------------------------------------------------------

	DECLARE_DX12SHADER(SpVSPTStatic)
	DECLARE_DX12SHADER(SpPSPTStatic)

	class SpPTStaticRender : public SpStaticRender {
		void CreatePipelineStates();
		static ComPtr<ID3D12PipelineState> m_defaultPipelineState;
		static ComPtr<ID3D12PipelineState> m_alphaPipelineState;
	protected:
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		explicit SpPTStaticRender(const shared_ptr<GameObject>& gameObjectPtr);
		SpPTStaticRender(const shared_ptr<GameObject>& gameObjectPtr,
			vector<VertexPositionTexture>& vertices, vector<uint32_t>& indices);
		virtual ~SpPTStaticRender() {}
		//操作
		virtual void OnInit()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};

	//--------------------------------------------------------------------------------------
	///	SpPNStatic描画コンポーネント
	//--------------------------------------------------------------------------------------

	DECLARE_DX12SHADER(SpVSPNStatic)
	DECLARE_DX12SHADER(SpPSPNStatic)

	class SpPNStaticRender : public SpStaticRender {
		void CreatePipelineStates();
		static ComPtr<ID3D12PipelineState> m_defaultPipelineState;
		static ComPtr<ID3D12PipelineState> m_alphaPipelineState;
	protected:
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		explicit SpPNStaticRender(const shared_ptr<GameObject>& gameObjectPtr);
		SpPNStaticRender(const shared_ptr<GameObject>& gameObjectPtr,
			vector<VertexPositionNormal>& vertices, vector<uint32_t>& indices);
		virtual ~SpPNStaticRender() {}
		//操作
		virtual void OnInit()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};

	//--------------------------------------------------------------------------------------
	///	SpPCTStatic描画コンポーネント
	//--------------------------------------------------------------------------------------

	DECLARE_DX12SHADER(SpVSPCTStatic)
	DECLARE_DX12SHADER(SpPSPCTStatic)

	class SpPCTStaticRender : public SpStaticRender {
		void CreatePipelineStates();
		static ComPtr<ID3D12PipelineState> m_defaultPipelineState;
		static ComPtr<ID3D12PipelineState> m_alphaPipelineState;
	protected:
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		explicit SpPCTStaticRender(const shared_ptr<GameObject>& gameObjectPtr);
		SpPCTStaticRender(const shared_ptr<GameObject>& gameObjectPtr,
			vector<VertexPositionColorTexture>& vertices, vector<uint32_t>& indices);
		virtual ~SpPCTStaticRender() {}
		//操作
		virtual void OnInit()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};

	//--------------------------------------------------------------------------------------
	///	SpPNTStatic描画コンポーネント
	//--------------------------------------------------------------------------------------

	DECLARE_DX12SHADER(SpVSPNTStatic)
	DECLARE_DX12SHADER(SpPSPNTStatic)

	class SpPNTStaticRender : public SpStaticRender {
		void CreatePipelineStates();
		static ComPtr<ID3D12PipelineState> m_defaultPipelineState;
		static ComPtr<ID3D12PipelineState> m_alphaPipelineState;
	protected:
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		explicit SpPNTStaticRender(const shared_ptr<GameObject>& gameObjectPtr);
		SpPNTStaticRender(const shared_ptr<GameObject>& gameObjectPtr,
			vector<VertexPositionNormalTexture>& vertices, vector<uint32_t>& indices);
		virtual ~SpPNTStaticRender() {}
		//操作
		virtual void OnInit()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};




}
// end namespace basecross

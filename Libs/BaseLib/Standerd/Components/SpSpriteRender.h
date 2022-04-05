/*!
@file SpSpriteRender.h
@brief Simpleスプライト描画コンポーネント群
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	DECLARE_DX12SHADER(SpVSPCSprite)
	DECLARE_DX12SHADER(SpPSPCSprite)

	DECLARE_DX12SHADER(SpVSPTSprite)
	DECLARE_DX12SHADER(SpPSPTSprite)

	DECLARE_DX12SHADER(SpVSPCTSprite)
	DECLARE_DX12SHADER(SpPSPCTSprite)


	//--------------------------------------------------------------------------------------
	///	SpSprite描画コンポーネント
	//--------------------------------------------------------------------------------------
	class SpSpriteRender : public Component {
		//エミッシブ色
		Col4 m_emissive;
		//デフューズ色
		Col4 m_diffuse;
	protected:
		explicit SpSpriteRender(const shared_ptr<GameObject>& gameObjectPtr);
		virtual ~SpSpriteRender() {}
		//コンスタントバッファの登録
		void SetConstant(SpriteConstant& constant, const shared_ptr<Transform>& transform);
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
		virtual void OnInitFrame(BaseFrame* pBaseFrame)override;
		virtual void WriteConstantBuffers(BaseFrame* pBaseFrame)override;
	};



	//--------------------------------------------------------------------------------------
	///	SpPCSprite描画コンポーネント
	//--------------------------------------------------------------------------------------
	class SpPCSpriteRender : public SpSpriteRender {
		void CreatePipelineStates();
		static ComPtr<ID3D12PipelineState> m_defaultPipelineState;
		static ComPtr<ID3D12PipelineState> m_alphaPipelineState;
	protected:
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		explicit SpPCSpriteRender(const shared_ptr<GameObject>& gameObjectPtr);
		SpPCSpriteRender(const shared_ptr<GameObject>& gameObjectPtr,
			vector<VertexPositionColor>& vertices, vector<uint32_t>& indices);
		virtual ~SpPCSpriteRender() {}
		//操作
		virtual void OnInit()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};


	//--------------------------------------------------------------------------------------
	///	SpPTSprite描画コンポーネント
	//--------------------------------------------------------------------------------------
	class SpPTSpriteRender : public SpSpriteRender {
		void CreatePipelineStates();
		static ComPtr<ID3D12PipelineState> m_defaultPipelineState;
		static ComPtr<ID3D12PipelineState> m_alphaPipelineState;
	protected:
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		explicit SpPTSpriteRender(const shared_ptr<GameObject>& gameObjectPtr);
		SpPTSpriteRender(const shared_ptr<GameObject>& gameObjectPtr,
			vector<VertexPositionTexture>& vertices, vector<uint32_t>& indices);
		virtual ~SpPTSpriteRender() {}
		//操作
		virtual void OnInit()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};


	//--------------------------------------------------------------------------------------
	///	SpPCTSprite描画コンポーネント
	//--------------------------------------------------------------------------------------
	class SpPCTSpriteRender : public SpSpriteRender {
		void CreatePipelineStates();
		static ComPtr<ID3D12PipelineState> m_defaultPipelineState;
		static ComPtr<ID3D12PipelineState> m_alphaPipelineState;
	protected:
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		explicit SpPCTSpriteRender(const shared_ptr<GameObject>& gameObjectPtr);
		SpPCTSpriteRender(const shared_ptr<GameObject>& gameObjectPtr,
			vector<VertexPositionColorTexture>& vertices, vector<uint32_t>& indices);
		virtual ~SpPCTSpriteRender() {}
		//操作
		virtual void OnInit()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};




}
//end namespace basecross

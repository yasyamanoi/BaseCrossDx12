/*!
@file SpSpriteRender.h
@brief Simple�X�v���C�g�`��R���|�[�l���g�Q
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
	///	SpSprite�`��R���|�[�l���g
	//--------------------------------------------------------------------------------------
	class SpSpriteRender : public Component {
		//�G�~�b�V�u�F
		Col4 m_emissive;
		//�f�t���[�Y�F
		Col4 m_diffuse;
	protected:
		explicit SpSpriteRender(const shared_ptr<GameObject>& gameObjectPtr);
		virtual ~SpSpriteRender() {}
		//�R���X�^���g�o�b�t�@�̓o�^
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
	///	SpPCSprite�`��R���|�[�l���g
	//--------------------------------------------------------------------------------------
	class SpPCSpriteRender : public SpSpriteRender {
		void CreatePipelineStates();
		ComPtr<ID3D12PipelineState> m_defaultPipelineState;
		ComPtr<ID3D12PipelineState> m_alphaPipelineState;
	protected:
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		explicit SpPCSpriteRender(const shared_ptr<GameObject>& gameObjectPtr);
		SpPCSpriteRender(const shared_ptr<GameObject>& gameObjectPtr,
			vector<VertexPositionColor>& vertices, vector<uint32_t>& indices);
		virtual ~SpPCSpriteRender() {}
		//����
		virtual void OnInit()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};


	//--------------------------------------------------------------------------------------
	///	SpPTSprite�`��R���|�[�l���g
	//--------------------------------------------------------------------------------------
	class SpPTSpriteRender : public SpSpriteRender {
		void CreatePipelineStates();
		ComPtr<ID3D12PipelineState> m_defaultPipelineState;
		ComPtr<ID3D12PipelineState> m_alphaPipelineState;
	protected:
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		explicit SpPTSpriteRender(const shared_ptr<GameObject>& gameObjectPtr);
		SpPTSpriteRender(const shared_ptr<GameObject>& gameObjectPtr,
			vector<VertexPositionTexture>& vertices, vector<uint32_t>& indices);
		virtual ~SpPTSpriteRender() {}
		//����
		virtual void OnInit()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};


	//--------------------------------------------------------------------------------------
	///	SpPCTSprite�`��R���|�[�l���g
	//--------------------------------------------------------------------------------------
	class SpPCTSpriteRender : public SpSpriteRender {
		void CreatePipelineStates();
		ComPtr<ID3D12PipelineState> m_defaultPipelineState;
		ComPtr<ID3D12PipelineState> m_alphaPipelineState;
	protected:
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		explicit SpPCTSpriteRender(const shared_ptr<GameObject>& gameObjectPtr);
		SpPCTSpriteRender(const shared_ptr<GameObject>& gameObjectPtr,
			vector<VertexPositionColorTexture>& vertices, vector<uint32_t>& indices);
		virtual ~SpPCTSpriteRender() {}
		//����
		virtual void OnInit()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};




}
//end namespace basecross
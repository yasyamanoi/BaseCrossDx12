/*!
@file SpStaticRender.h
@brief Simple�X�^�e�B�b�N�`��R���|�[�l���g�Q
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	Shadowmap�`��R���|�[�l���g
	//--------------------------------------------------------------------------------------
	DECLARE_DX12SHADER(PNTShadowmap)

	class Shadowmap : public Component {
		void CreatePipelineStates();
		ComPtr<ID3D12PipelineState> m_PNTPipelineState;

		const float m_lightHeight{ 200.0f };
		const float m_lightNear{ 1.0f };
		const float m_lightFar{ 300.0f };
		const float m_viewWidth{ 32.0f };
		const float m_viewHeight{ 32.0f };
		const float m_posAdjustment{ 0.1f };

	protected:
		//�R���X�^���g�o�b�t�@�̓o�^
		void SetConstant(ShadowConstant& constant, const shared_ptr<Transform>& transform);
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		virtual void OnInitFrame(BaseFrame* pBaseFrame)override;
		virtual void WriteConstantBuffers(BaseFrame* pBaseFrame)override;
		explicit Shadowmap(const shared_ptr<GameObject>& gameObjectPtr);
		virtual ~Shadowmap() {}
		//����
		virtual void OnInit()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};



	//--------------------------------------------------------------------------------------
	///	SpStatic�`��R���|�[�l���g(�e)
	//--------------------------------------------------------------------------------------
	class SpStaticRender : public Component {
		/// �G�~�b�V�u�F
		Col4 m_emissive;
		/// �f�t���[�Y�F
		Col4 m_diffuse;
		/// �X�y�L�����[
		Col4 m_specular;
		//�������g�ɉe��`�悷�邩�ǂ���
		bool m_ownShadowActive;
	protected:
		explicit SpStaticRender(const shared_ptr<GameObject>& gameObjectPtr) :
			Component(gameObjectPtr),
			m_emissive(0.0f),
			m_diffuse(1.0f),
			m_specular(0.0f),
			m_ownShadowActive(false)
		{}
		virtual ~SpStaticRender() {}
		//�R���X�^���g�o�b�t�@�̓o�^
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
	///	SpPCStatic�`��R���|�[�l���g
	//--------------------------------------------------------------------------------------

	DECLARE_DX12SHADER(SpVSPCStatic)
	DECLARE_DX12SHADER(SpPSPCStatic)

	class SpPCStaticRender : public SpStaticRender {
		void CreatePipelineStates();
		ComPtr<ID3D12PipelineState> m_defaultPipelineState;
		ComPtr<ID3D12PipelineState> m_alphaPipelineState;
	protected:
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		explicit SpPCStaticRender(const shared_ptr<GameObject>& gameObjectPtr);
		SpPCStaticRender(const shared_ptr<GameObject>& gameObjectPtr,
			vector<VertexPositionColor>& vertices, vector<uint32_t>& indices);
		virtual ~SpPCStaticRender() {}
		//����
		virtual void OnInit()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};

	//--------------------------------------------------------------------------------------
	///	SpPTStatic�`��R���|�[�l���g
	//--------------------------------------------------------------------------------------

	DECLARE_DX12SHADER(SpVSPTStatic)
	DECLARE_DX12SHADER(SpPSPTStatic)

	class SpPTStaticRender : public SpStaticRender {
		void CreatePipelineStates();
		ComPtr<ID3D12PipelineState> m_defaultPipelineState;
		ComPtr<ID3D12PipelineState> m_alphaPipelineState;
	protected:
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		explicit SpPTStaticRender(const shared_ptr<GameObject>& gameObjectPtr);
		SpPTStaticRender(const shared_ptr<GameObject>& gameObjectPtr,
			vector<VertexPositionTexture>& vertices, vector<uint32_t>& indices);
		virtual ~SpPTStaticRender() {}
		//����
		virtual void OnInit()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};

	//--------------------------------------------------------------------------------------
	///	SpPNStatic�`��R���|�[�l���g
	//--------------------------------------------------------------------------------------

	DECLARE_DX12SHADER(SpVSPNStatic)
	DECLARE_DX12SHADER(SpPSPNStatic)

	class SpPNStaticRender : public SpStaticRender {
		void CreatePipelineStates();
		ComPtr<ID3D12PipelineState> m_defaultPipelineState;
		ComPtr<ID3D12PipelineState> m_alphaPipelineState;
	protected:
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		explicit SpPNStaticRender(const shared_ptr<GameObject>& gameObjectPtr);
		SpPNStaticRender(const shared_ptr<GameObject>& gameObjectPtr,
			vector<VertexPositionNormal>& vertices, vector<uint32_t>& indices);
		virtual ~SpPNStaticRender() {}
		//����
		virtual void OnInit()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};

	//--------------------------------------------------------------------------------------
	///	SpPCTStatic�`��R���|�[�l���g
	//--------------------------------------------------------------------------------------

	DECLARE_DX12SHADER(SpVSPCTStatic)
	DECLARE_DX12SHADER(SpPSPCTStatic)

	class SpPCTStaticRender : public SpStaticRender {
		void CreatePipelineStates();
		ComPtr<ID3D12PipelineState> m_defaultPipelineState;
		ComPtr<ID3D12PipelineState> m_alphaPipelineState;
	protected:
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		explicit SpPCTStaticRender(const shared_ptr<GameObject>& gameObjectPtr);
		SpPCTStaticRender(const shared_ptr<GameObject>& gameObjectPtr,
			vector<VertexPositionColorTexture>& vertices, vector<uint32_t>& indices);
		virtual ~SpPCTStaticRender() {}
		//����
		virtual void OnInit()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};

	//--------------------------------------------------------------------------------------
	///	SpPNTStatic�`��R���|�[�l���g
	//--------------------------------------------------------------------------------------

	DECLARE_DX12SHADER(SpVSPNTStatic)
	DECLARE_DX12SHADER(SpVSPNTStaticShadow)

	DECLARE_DX12SHADER(SpPSPNTStatic)
	DECLARE_DX12SHADER(SpPSPNTStaticShadow)

	class SpPNTStaticRender : public SpStaticRender {
		void CreatePipelineStates();
		ComPtr<ID3D12PipelineState> m_defaultPipelineState;
		ComPtr<ID3D12PipelineState> m_defaultShadowPipelineState;
		ComPtr<ID3D12PipelineState> m_alphaPipelineState;
		ComPtr<ID3D12PipelineState> m_alphaShadowPipelineState;
	protected:
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		explicit SpPNTStaticRender(const shared_ptr<GameObject>& gameObjectPtr);
		SpPNTStaticRender(const shared_ptr<GameObject>& gameObjectPtr,
			vector<VertexPositionNormalTexture>& vertices, vector<uint32_t>& indices);
		virtual ~SpPNTStaticRender() {}
		//����
		virtual void OnInit()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};




}
// end namespace basecross
/*!
@file SpStaticDraw.h
@brief Simple�X�^�e�B�b�N�`��R���|�[�l���g�Q
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	SpStatic�`��R���|�[�l���g(�e)
	//--------------------------------------------------------------------------------------
	class SpStaticDraw : public Component {
		/// �G�~�b�V�u�F
		Col4 m_emissive;
		/// �f�t���[�Y�F
		Col4 m_diffuse;
		/// �X�y�L�����[
		Col4 m_specular;
		//�������g�ɉe��`�悷�邩�ǂ���
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

	class SpPCStaticDraw : public SpStaticDraw {
		void CreatePipelineStates();
	protected:
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		explicit SpPCStaticDraw(const shared_ptr<GameObject>& gameObjectPtr);
		SpPCStaticDraw(const shared_ptr<GameObject>& gameObjectPtr,
			vector<VertexPositionColor>& vertices, vector<uint32_t>& indices);
		virtual ~SpPCStaticDraw() {}
		//����
		virtual void OnCreate()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};

	//--------------------------------------------------------------------------------------
	///	SpWireframeDraw�`��R���|�[�l���g(���C���t���[��)
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
		//����
		virtual void OnCreate()override;
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

	class SpPNTStaticDraw : public SpStaticDraw {
		void CreatePipelineStates();
	protected:
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		explicit SpPNTStaticDraw(const shared_ptr<GameObject>& gameObjectPtr);
		SpPNTStaticDraw(const shared_ptr<GameObject>& gameObjectPtr,
			vector<VertexPositionNormalTexture>& vertices, vector<uint32_t>& indices);
		virtual ~SpPNTStaticDraw() {}
		//����
		virtual void OnCreate()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};



}
// end namespace basecross
/*!
@file BcStaticDraw.h
@brief Basic�X�^�e�B�b�N�`��R���|�[�l���g�Q
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	BcStatic�`��R���|�[�l���g
	//--------------------------------------------------------------------------------------
	class BcStaticDraw : public Component {
		//�������g�ɉe��`�悷�邩�ǂ���
		bool m_ownShadowActive;
		//�t�H�O���L�����ǂ���
		bool m_fogEnabled;
		//�t�H�O�̊J�n�ʒu
		float m_fogStart;
		//�t�H�O�̏I���ʒu
		float m_fogEnd;
		//�t�H�O�F
		bsm::Col4 m_fogColor;
		//�t�H�O�x�N�g��
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
		//�R���X�^���g�o�b�t�@�̓o�^
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
		@brief	�t�H�O���g�����ǂ���
		@return	�t�H�O���g���Ȃ�true
		*/
		//--------------------------------------------------------------------------------------
		bool IsFogEnabled() const {
			return m_fogEnabled;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�t�H�O���g�����ǂ����w��
		@param[in]	b	�t�H�O���g���Ȃ�true
		@return	�Ȃ�
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
//		ComPtr<ID3D12PipelineState> m_defaultPipelineState;
//		ComPtr<ID3D12PipelineState> m_defaultShadowPipelineState;
//		ComPtr<ID3D12PipelineState> m_alphaPipelineState;
//		ComPtr<ID3D12PipelineState> m_alphaShadowPipelineState;
	protected:
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		explicit BcPNTStaticDraw(const shared_ptr<GameObject>& gameObjectPtr);
		BcPNTStaticDraw(const shared_ptr<GameObject>& gameObjectPtr,
			vector<VertexPositionNormalTexture>& vertices, vector<uint32_t>& indices);
		virtual ~BcPNTStaticDraw() {}
		//����
		virtual void OnCreate()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};


}
// end namespace basecross
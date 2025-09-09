/*!
@file BcBoneDraw.h
@brief �x�C�V�b�N�X�^�e�B�b�N�`��R���|�[�l���g
*/


#pragma once
#include "stdafx.h"

namespace basecross {


	DECLARE_DX12SHADER(BcVSPNTBonePL)
	DECLARE_DX12SHADER(BcVSPNTBonePLShadow)


	//--------------------------------------------------------------------------------------
	///	BcBoneDraw�R���|�[�l���g
	//--------------------------------------------------------------------------------------
	class BcBoneDraw : public Component {
	protected:
		BasicConstant m_constantBuffer;
		size_t m_constantBufferIndex;
		//�������g�ɉe��`�悷�邩�ǂ���
		bool m_ownShadowActive;
		//�t�H�O���L�����ǂ���
		bool m_fogEnabled = true;
		//�t�H�O�̊J�n�ʒu
		float m_fogStart = -10.0f;
		//�t�H�O�̏I���ʒu
		float m_fogEnd = -40.0f;
		//�t�H�O�F
		XMFLOAT4 m_fogColor;
		//�t�H�O�x�N�g��
		XMFLOAT3 m_fogVector;

		std::vector<Mat4x4>  m_BoneTransforms;

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



		BcBoneDraw(const std::shared_ptr<GameObject>& gameObjectPtr);
		virtual ~BcBoneDraw() {}
		virtual void OnUpdateConstantBuffers()override;
		virtual void OnCommitConstantBuffers()override;
		virtual void OnCreate()override;
		virtual void OnSceneDraw()override;

		virtual bool UpdateAnimation(double animeTime);

	};

}
// end namespace basecross

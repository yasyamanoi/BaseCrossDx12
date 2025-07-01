/*!
@file SpStaticDraw.h
@brief Simple�X�^�e�B�b�N�`��R���|�[�l���g�Q
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
		SimpleConstant m_simpleConstant;
		size_t m_constantIndex;
		//�I���W�i�����b�V��
		std::shared_ptr<BaseMesh> m_mesh;
		explicit SpStaticDraw(const std::shared_ptr<GameObject>& gameObjectPtr) :
			Component(gameObjectPtr),
			m_emissive(0.0f),
			m_diffuse(1.0f),
			m_specular(0.0f),
			m_ownShadowActive(false)
		{
		}
		virtual ~SpStaticDraw() {}
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
		virtual void OnCreate()override;
	};




	//--------------------------------------------------------------------------------------
	///	SpPNTStatic�`��R���|�[�l���g
	//--------------------------------------------------------------------------------------

	DECLARE_DX12SHADER(SpVSPNTStatic)
	DECLARE_DX12SHADER(SpVSPNTStaticShadow)

	DECLARE_DX12SHADER(SpPSPNTStatic)
	DECLARE_DX12SHADER(SpPSPNTStaticShadow)

	class SpPNTStaticDraw : public SpStaticDraw {
	protected:
	public:
		explicit SpPNTStaticDraw(const std::shared_ptr<GameObject>& gameObjectPtr);
		SpPNTStaticDraw(const std::shared_ptr<GameObject>& gameObjectPtr,
			std::vector<VertexPositionNormalTexture>& vertices, std::vector<uint32_t>& indices);
		virtual ~SpPNTStaticDraw() {}
		//����
		virtual void OnUpdate(double elapsedTime)override {}
		virtual void OnUpdateConstantBuffers()override;
		virtual void OnCommitConstantBuffers()override;
		virtual void OnSceneDraw()override;
		virtual void OnDestroy()override {}
	};



}
// end namespace basecross
